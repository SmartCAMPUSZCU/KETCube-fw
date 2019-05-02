/**
 * @file    ketCube_[NAME_LOWERCASE].c
 * @author  [AUTHOR]
 * @version [VERSION]
 * @date    [DATE]
 * @brief   This file contains definitions for the [NAME] driver
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

#include "ketCube_cfg.h"
#include "ketCube_terminal.h"
#include "ketCube_[NAME_LOWERCASE].h"

#ifdef KETCUBE_CFG_INC_MOD_[NAME_UPPERCASE]

ketCube_[NAME_LOWERCASE]_moduleCfg_t ketCube_[NAME_LOWERCASE]_moduleCfg; /*!< Module configuration storage */

/**
 * @brief Initialize the [NAME] sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_[NAME_LOWERCASE]_Init(ketCube_InterModMsg_t *** msg)
{

    // Init Drivers
    
    
    // Custom Init Code

    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Initialize the [NAME] sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_[NAME_LOWERCASE]_UnInit(void)
{
    // UnInit drivers
    
    
    return KETCUBE_CFG_MODULE_OK;
}


/**
  * @brief Read data from [NAME] sensor
  *
  * @param buffer pointer to fuffer for storing the result of measurements
  * @param len data len in bytes
  *
  * @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_[NAME_LOWERCASE]_ReadData(uint8_t * buffer,
                                                uint8_t * len)
{
    uint8_t i = 0;
    uint16_t data;

    // Get sensor data
    
    // Insert measured data to buffer
    buffer[i++] = (data >> 8) & 0xFF;
    buffer[i++] = (data & 0xFF);
    
    *len = i;

    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_[NAME],
                                 "Measured data: %d", data);

    return KETCUBE_CFG_MODULE_OK;
}

#endif                          /* KETCUBE_CFG_INC_MOD_[NAME_UPPERCASE] */
