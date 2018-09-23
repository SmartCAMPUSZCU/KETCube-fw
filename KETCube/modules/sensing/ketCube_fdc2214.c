/**
 * @file    ketCube_fdc2214.c
 * @author  Jan Belohoubek
 * @version pre-alpha
 * @date    2018-03-18
 * @brief   This file contains the FDC2214 driver
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
#include "stm32l0xx_hal_i2c.h"
#include <math.h>

#include "ketCube_cfg.h"
#include "ketCube_terminal.h"
#include "ketCube_i2c.h"
#include "ketCube_fdc2214.h"

#ifdef KETCUBE_CFG_INC_MOD_FDC2214

#define FDC2214_GPIO_PORT                   GPIOB
#define FDC2214_SD_PIN                      GPIO_PIN_2
#define FDC2214_INT_PIN                     GPIO_PIN_7
#define FDC2214_SD_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE()
#define FDC2214_SD_CLK_DISABLE()            __HAL_RCC_GPIOB_CLK_DISABLE()
#define FDC2214_ON()                        HAL_GPIO_WritePin(FDC2214_GPIO_PORT, FDC2214_SD_PIN, GPIO_PIN_RESET)
#define FDC2214_OFF()                       HAL_GPIO_WritePin(FDC2214_GPIO_PORT, FDC2214_SD_PIN, GPIO_PIN_SET)

/**
 * @brief Initialize the FDC2214 sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_fdc2214_Init(ketCube_InterModMsg_t *** msg)
{
    uint16_t reg;
    uint8_t i;

    GPIO_InitTypeDef GPIO_InitStruct;

    // Init drivers
    if (ketCube_I2C_Init() != KETCUBE_CFG_MODULE_OK) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    /* Init GPIO */
    FDC2214_SD_CLK_ENABLE();

    GPIO_InitStruct.Pin = FDC2214_SD_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(FDC2214_GPIO_PORT, &GPIO_InitStruct);

    /* Power ON */
    FDC2214_ON();

    reg = 0x8000;
    i = 0;
    while (ketCube_I2C_TexasWriteReg
           (KETCUBE_FDC2214_I2C_ADDRESS, KETCUBE_FDC2214_RESET_DEV,
            &reg) == KETCUBE_CFG_MODULE_ERROR) {
        i++;
        if (i == 200) {
            return KETCUBE_CFG_MODULE_ERROR;
        }
        ketCube_terminal_DebugPrintln("FDC2214 :: E0");
    }

    reg = 0xffff;
    while (ketCube_I2C_TexasWriteReg
           (KETCUBE_FDC2214_I2C_ADDRESS, KETCUBE_FDC2214_RCOUNT_CH0,
            &reg) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_DebugPrintln("FDC2214 :: E1");
    }

    reg = 0x0400;
    while (ketCube_I2C_TexasWriteReg
           (KETCUBE_FDC2214_I2C_ADDRESS, KETCUBE_FDC2214_SETTLECOUNT_CH0,
            &reg) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_DebugPrintln("FDC2214 :: E2");
    }

    reg = 0x1001;
    while (ketCube_I2C_TexasWriteReg
           (KETCUBE_FDC2214_I2C_ADDRESS,
            KETCUBE_FDC2214_CLOCK_DIVIDERS_CH0,
            &reg) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_DebugPrintln("FDC2214 :: E3");
    }


    // external OSC
    // reg = 0x0e01;

    // internal OSC
    reg = 0x0c01;

    while (ketCube_I2C_TexasWriteReg
           (KETCUBE_FDC2214_I2C_ADDRESS, KETCUBE_FDC2214_CONFIG,
            &reg) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_DebugPrintln("FDC2214 :: E5");
    }

    reg = 0x420f;
    while (ketCube_I2C_TexasWriteReg
           (KETCUBE_FDC2214_I2C_ADDRESS, KETCUBE_FDC2214_MUX_CONFIG,
            &reg) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_DebugPrintln("FDC2214 :: E6");
    }

    reg = 0x8800;
    while (ketCube_I2C_TexasWriteReg
           (KETCUBE_FDC2214_I2C_ADDRESS, KETCUBE_FDC2214_DRIVE_CURRENT_CH0,
            &reg) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_DebugPrintln("FDC2214 :: E8");
    }

    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Initialize the FDC2214 sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_fdc2214_UnInit(void)
{
    // UnInit drivers
    return ketCube_I2C_UnInit();
}

/**
  * @brief Read data from FDC2214 sensor
  *
  * @param buffer pointer to fuffer for storing the result of milivolt mesurement
  * @param len data len in bytes
  *
  * @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_fdc2214_ReadData(uint8_t * buffer,
                                                uint8_t * len)
{
    uint16_t msb, lsb;
    uint32_t raw;

    if (ketCube_I2C_TexasReadReg
        (KETCUBE_FDC2214_I2C_ADDRESS, KETCUBE_FDC2214_DATA_CH0, &msb)) {
        ketCube_terminal_DebugPrintln("FDC2214 :: E0");
        return KETCUBE_CFG_MODULE_ERROR;
    }

    if (ketCube_I2C_TexasReadReg
        (KETCUBE_FDC2214_I2C_ADDRESS, KETCUBE_FDC2214_DATA_LSB_CH0,
         &lsb)) {
        ketCube_terminal_DebugPrintln("FDC2214 :: E1");
        return KETCUBE_CFG_MODULE_ERROR;
    }

    raw = ((((uint32_t) msb) << 16) | lsb) & 0x0FFFFFFF;

    buffer[0] = (uint8_t) (raw >> 24);
    buffer[1] = (uint8_t) (raw >> 16);
    buffer[2] = (uint8_t) (raw >> 8);
    buffer[3] = (uint8_t) raw;

    *len = 4;

    ketCube_terminal_Println("FDC2214 :: RawCapacity: %d", raw);

    return KETCUBE_CFG_MODULE_OK;
}

#endif                          /* KETCUBE_CFG_INC_MOD_FDC2214 */
