/**
 * @file    ketCube_batMeas.c
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-08-19
 * @brief   This file contains definitions for the KETCube batMeas module
 *
* @attention
 *
 * <h2><center>&copy; Copyright (c) 2018 University of West Bohemia in Pilsen
 * All rights reserved.</center></h2>
 *
 * Developed by:
 * The SmartCampus Team
 * Department of Technologies and Measurement
 * www.smartcampus.cz | www.zcu.cz
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), 
 * to deal with the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software 
 * is furnished to do so, subject to the following conditions:
 *
 *    - Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimers.
 *    
 *    - Redistributions in binary form must reproduce the above copyright notice, 
 *      this list of conditions and the following disclaimers in the documentation 
 *      and/or other materials provided with the distribution.
 *    
 *    - Neither the names of The SmartCampus Team, Department of Technologies and Measurement
 *      and Faculty of Electrical Engineering University of West Bohemia in Pilsen, 
 *      nor the names of its contributors may be used to endorse or promote products 
 *      derived from this Software without specific prior written permission. 
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT HOLDERS 
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
 * OR THE USE OR OTHER DEALINGS WITH THE SOFTWARE. 
 */

#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_lptim.h"

#include "hw.h"
#include "timeServer.h"
#include "ketCube_common.h"
#include "ketCube_timer.h"
#include "ketCube_batMeas.h"
#include "ketCube_ad.h"
#include "ketCube_terminal.h"

#ifdef KETCUBE_CFG_INC_MOD_BATMEAS

ketCube_batMeas_moduleCfg_t ketCube_batMeas_moduleCfg; /*!< Module configuration storage */

/**
  *
  *  @brief List of supported batteries
  *
  */
ketCube_batMeas_battery_t ketCube_batMeas_batList[] = {
    {((char *) &("CR2032")),
     ((char *) &("Up to 560mAh battery")),
     3300,
     2900},

    {((char *) &("LS33600")),
     ((char *) &("15 Ah battery")),
     3600,
     2900}
};

/**
 * @brief  Initializes Battery Measurement
 * 
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_batMeas_Init(ketCube_InterModMsg_t *** msg)
{
    // Init AD driver
    ketCube_AD_Init();
    
    // check if selected battery is legal
    if (ketCube_batMeas_moduleCfg.selectedBattery >= KETCUBE_BATMEAS_BATLIST_LAST) {
        ketCube_batMeas_moduleCfg.selectedBattery = KETCUBE_BATMEAS_BATLIST_CR2032;
    }
    
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_BATMEAS, "Selected battery is: %s (%s)",
                                 ketCube_batMeas_batList[ketCube_batMeas_moduleCfg.selectedBattery].batName,
                                 ketCube_batMeas_batList[ketCube_batMeas_moduleCfg.selectedBattery].batDescr);

    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief  DeInit BatMeas
 * 
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_batMeas_DeInit(ketCube_InterModMsg_t ***
                                              msg)
{
    return KETCUBE_CFG_MODULE_OK;
}

/**
  * @brief This function returns the battery level
  * @param none
  
  * @retval battery level scaled to 1B - 1 (very low) to 254 (fully charged)
  */
uint8_t ketCube_batMeas_GetBatteryByte(void)
{
    uint8_t batteryLevel = 0;
    uint32_t batteryLevelmV = ketCube_AD_GetBatLevelmV();

    uint16_t batMax =
        ketCube_batMeas_batList
        [ketCube_batMeas_moduleCfg.selectedBattery].batCharged;
    uint16_t batMin =
        ketCube_batMeas_batList
        [ketCube_batMeas_moduleCfg.selectedBattery].batDischarged;

    ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_BATMEAS, "Value %d mV", batteryLevelmV);
        
    if (batteryLevelmV > batMax) {
        batteryLevel = 254;
    } else if (batteryLevelmV < batMin) {
        batteryLevel = 0;
    } else {
        batteryLevel =
            (((uint32_t) (batteryLevelmV - batMin) * 254) /
             (batMax - batMin));
    }

    return batteryLevel;
}

/**
  * @brief Read battery data
  *
  * @param buffer pointer to fuffer for storing the result of milivolt mesurement
  * @param len data len in bytes
  *
  * @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_batMeas_ReadData(uint8_t * buffer,
                                                uint8_t * len)
{
    // write to buffer
    *len = 1;
    buffer[0] = ketCube_batMeas_GetBatteryByte();

    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_BATMEAS, "Encoded value: %d",
                                 buffer[0]);

    return KETCUBE_CFG_MODULE_OK;
}


#endif                          /* KETCUBE_CFG_INC_MOD_BATMES */
