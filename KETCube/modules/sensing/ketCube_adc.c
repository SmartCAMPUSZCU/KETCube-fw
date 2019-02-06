/**
 * @file    ketCube_adc.c
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-03-02
 * @brief   This file contains definitions for the KETCube PA4 ADC driver
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
#include <math.h>

#include "hw_msp.h"
#include "ketCube_common.h"
#include "ketCube_adc.h"
#include "ketCube_terminal.h"

#ifdef KETCUBE_CFG_INC_MOD_ADC

/**
 * @brief  Configures ADC PIN
 * 
 * @retval KETCUBE_ADC_OK in case of success
 * @retval KETCUBE_ADC_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_ADC_Init(ketCube_InterModMsg_t *** msg)
{
    GPIO_InitTypeDef initStruct;

    __HAL_RCC_GPIOA_CLK_ENABLE();

    initStruct.Mode = GPIO_MODE_ANALOG;
    initStruct.Pull = GPIO_NOPULL;
    initStruct.Speed = GPIO_SPEED_HIGH;
    initStruct.Pin = GPIO_PIN_4;

    HAL_GPIO_Init(GPIOA, &initStruct);

    return KETCUBE_CFG_MODULE_OK;
}

/**
  * @brief Get milivolt value form PA4
  *
  * @param buffer pointer to fuffer for storing the result of milivolt mesurement
  * @param len data len in bytes
  *
  * @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_ADC_ReadData(uint8_t * buffer,
                                            uint8_t * len)
{
    uint32_t vddmv;
    uint16_t vdd;
    uint16_t vin;
    uint16_t mv;

    vdd = HW_AdcReadChannel(ADC_CHANNEL_VREFINT);

    if (vdd == 0) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    vddmv =
        (((uint32_t) KETCUBE_ADC_VDDA_VREFINT_CAL *
          (*KETCUBE_ADC_VREFINT_CAL_ADDR)) / vdd);

    vin = HW_AdcReadChannel(ADC_CHANNEL_4);

    // for 12 bit resolution ...
    mv = (uint16_t) ((vddmv * vin) / KETCUBE_ADC_MAX);

    // write to buffer
    *len = 2;
    buffer[0] = ((uint8_t) ((mv >> 8) & 0xFF));
    buffer[1] = ((uint8_t) (mv & 0xFF));

    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_ADC,
                                 "Voltage@PA4: %d", mv);

    return KETCUBE_CFG_MODULE_OK;
}

#endif                          /* KETCUBE_CFG_INC_MOD_ADC */
