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

static inline void ketCube_fdc2214_on(void) {
    ketCube_GPIO_Write(FDC2214_SD_PORT, FDC2214_SD_PIN, FALSE);
    HAL_Delay(KETCUBE_FDC2214_I2C_WU);
}

static inline void ketCube_fdc2214_off(void) {
    ketCube_GPIO_Write(FDC2214_SD_PORT, FDC2214_SD_PIN, TRUE);
}

static inline ketCube_cfg_ModError_t ketCube_fdc2214_writeReg( uint8_t regAddr, uint16_t * reg) {
    uint8_t i = 0;
    
    *reg = 0;
    for (i = 0; i < KETCUBE_FDC2214_I2C_TRY; i++) {
       if (ketCube_I2C_TexasWriteReg(KETCUBE_FDC2214_I2C_ADDRESS, regAddr, reg) == KETCUBE_CFG_MODULE_OK) {
           break;
       }   
    }
    if (i == KETCUBE_FDC2214_I2C_TRY) {
        ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Write Register %d! failed", regAddr);
        return KETCUBE_CFG_MODULE_ERROR;
    }
    
    return KETCUBE_CFG_MODULE_OK;
}

static inline ketCube_cfg_ModError_t ketCube_fdc2214_readReg(uint8_t regAddr, uint16_t * reg) {
    uint8_t i = 0;
    
    *reg = 0;
    for (i = 0; i < KETCUBE_FDC2214_I2C_TRY; i++) {
       if ((ketCube_I2C_TexasReadReg(KETCUBE_FDC2214_I2C_ADDRESS, regAddr, reg)) == KETCUBE_CFG_MODULE_OK) {
           break;
       }   
    }
    if (i == KETCUBE_FDC2214_I2C_TRY) {
        ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Read Register %d! failed", regAddr);
        return KETCUBE_CFG_MODULE_ERROR;
    }
    
    return KETCUBE_CFG_MODULE_OK;
}


static inline ketCube_cfg_ModError_t ketCube_fdc2214_reset(void) {
    uint16_t reg;
    
    reg = 0x8000;
    if (ketCube_fdc2214_writeReg(KETCUBE_FDC2214_RESET_DEV, &reg) != KETCUBE_CFG_MODULE_OK) {
        return KETCUBE_CFG_MODULE_ERROR;
    }
    
    HAL_Delay(KETCUBE_FDC2214_I2C_WU);
    
    return KETCUBE_CFG_MODULE_OK;
}

void ketCube_fdc2214_irqStatusUpdated() {
    static uint32_t pastValue;
    uint32_t currentValue;
    uint16_t msb, lsb;
    
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "INT");
    
//     if (ketCube_I2C_TexasReadReg
//         (KETCUBE_FDC2214_I2C_ADDRESS, KETCUBE_FDC2214_STATUS,
//          &lsb)) {
//         ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Unable to read STATUS");
//         return;
//     }
//     
//     if (ketCube_I2C_TexasReadReg
//         (KETCUBE_FDC2214_I2C_ADDRESS, KETCUBE_FDC2214_DATA_CH0, &msb)) {
//         ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Unable to read MSB");
//         return;
//     }
// 
//     if (ketCube_I2C_TexasReadReg
//         (KETCUBE_FDC2214_I2C_ADDRESS, KETCUBE_FDC2214_DATA_LSB_CH0,
//          &lsb)) {
//         ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Unable to read LSB");
//         return;
//     }
//     
//     currentValue = ((((uint32_t ) msb) << 16) | lsb);
//     
//     // TODO redefine threshold
//     if (abs(pastValue - currentValue) > 10000) {
//         ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Signifficant differnece indicated.");
// #if (FDC2214_USE_LED_INDICATION == TRUE)
//         //bool tmp_LED_state = ketCube_GPIO_Read(FDC2214_LED_PORT, FDC2214_LED_PIN);
//         //ketCube_GPIO_Write(FDC2214_LED_PORT, FDC2214_LED_PIN, !tmp_LED_state);
//         if (pastValue > currentValue) {
//             ketCube_GPIO_Write(FDC2214_LED_PORT, FDC2214_LED_PIN, FALSE);
//         } else {
//             ketCube_GPIO_Write(FDC2214_LED_PORT, FDC2214_LED_PIN, TRUE);
//         }
// #endif
//     }
//     
//     pastValue = currentValue;
}
 

/**
 * @brief Initialize the FDC2214 sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_fdc2214_Init(ketCube_InterModMsg_t *** msg)
{
    uint16_t reg;

    GPIO_InitTypeDef GPIO_InitStruct;

    // Init drivers
    if (ketCube_I2C_Init() != KETCUBE_CFG_MODULE_OK) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    
    if (ketCube_GPIO_Init(FDC2214_SD_PORT, FDC2214_SD_PIN, &GPIO_InitStruct) != KETCUBE_CFG_DRV_OK) {
        return KETCUBE_CFG_MODULE_ERROR;
    }
#if (FDC2214_USE_LED_INDICATION == TRUE)
    if (ketCube_GPIO_Init(FDC2214_LED_PORT, FDC2214_LED_PIN, &GPIO_InitStruct) != KETCUBE_CFG_DRV_OK) {
        return KETCUBE_CFG_MODULE_ERROR;
    }
#endif

    // enable interrupt
#if (FDC2214_ENABLE_INT == TRUE)
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Pin = FDC2214_INT_PIN;
    
    ketCube_GPIO_Init(FDC2214_INT_PORT, FDC2214_INT_PIN, &GPIO_InitStruct);

    if (ketCube_GPIO_SetIrq(FDC2214_INT_PORT, FDC2214_INT_PIN, 0, &ketCube_fdc2214_irqStatusUpdated) != KETCUBE_CFG_DRV_OK) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Unable to setUp IRQ!");
        return KETCUBE_CFG_MODULE_ERROR;
    }
#endif

    // Power ON
    ketCube_fdc2214_on();
    
    // Reset
    if (ketCube_fdc2214_reset() == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Unable to reset!");
        return KETCUBE_CFG_MODULE_ERROR;
    }

    /**
     * @todo Make the following code nice-looking after I2C API refactoring
     */
    
    // osc settings include reserved bits configuration
#if (FDC2214_USE_EXTERNAL_OSC == TRUE)
    reg = 0x1e01;
#else
    reg = 0x1c01;
#endif
    
#if (FDC2214_ENABLE_INT == FALSE)
    reg |= 0x0080;
#endif
    
#if (FDC2214_SINGLE_CHAN3 == TRUE)
    reg |= 0xC000;
#elif (FDC2214_SINGLE_CHAN2 == TRUE)
    reg |= 0x8000;
#elif (FDC2214_SINGLE_CHAN1 == TRUE)
    reg |= 0x4000;
#elif (FDC2214_SINGLE_CHAN0 == TRUE)
    reg |= 0x0000;
#endif
    
    if (ketCube_fdc2214_writeReg(KETCUBE_FDC2214_CONFIG, &reg) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Config configuration failed");
        return KETCUBE_CFG_MODULE_ERROR;
    }

    reg = (0x41 << 3); // Reserved bits must be set accordingly!
    
#if (FDC2214_SINGLE_CHAN3 == TRUE)
    // set RR_Sequence ...
    reg |= (0b110 << 13);
#else
    reg |= (0b111 << 13);
#endif
    // set deglitch
    reg |= 0b001;
    if (ketCube_fdc2214_writeReg(KETCUBE_FDC2214_MUX_CONFIG, &reg) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "MuxConfig configuration failed");
        return KETCUBE_CFG_MODULE_ERROR;
    }
    
    reg = 0;
#if (FDC2214_ENABLE_INT == FALSE)
    reg |= 0x0001;
#endif
    
    if (ketCube_fdc2214_writeReg(KETCUBE_FDC2214_STATUS_CONFIG, &reg) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "StatusConfig configuration failed");
        return KETCUBE_CFG_MODULE_ERROR;
    }
    
    // CH0 configuration
    reg = 500;
    //reg = 0x0080;
    if (ketCube_fdc2214_writeReg(KETCUBE_FDC2214_RCOUNT_CH0, &reg) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Setting CH0 failed (E0)!");
        return KETCUBE_CFG_MODULE_ERROR;
    }

    reg = 200;
    //reg = 0x00C8;
    if (ketCube_fdc2214_writeReg(KETCUBE_FDC2214_SETTLECOUNT_CH0, &reg) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Setting CH0 failed (E1)!");
        return KETCUBE_CFG_MODULE_ERROR;
    }

    reg = 0x0001;
    if (ketCube_fdc2214_writeReg(KETCUBE_FDC2214_CLOCK_DIVIDERS_CH0, &reg) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Setting CH0 failed (E2)!");
        return KETCUBE_CFG_MODULE_ERROR;
    }
    
    reg = ((0b10000) << 11);
    if (ketCube_fdc2214_writeReg(KETCUBE_FDC2214_DRIVE_CURRENT_CH0, &reg) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Setting CH0 failed (E3)!");
        return KETCUBE_CFG_MODULE_ERROR;
    }
    
    if (ketCube_fdc2214_readReg(KETCUBE_FDC2214_MANUFACTURER_ID, &reg) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Unable to read ManID");
    } else {
        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "ManID: 0x%04X", reg);
    }
    
    if (ketCube_fdc2214_readReg(KETCUBE_FDC2214_DEVICE_ID, &reg) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Unable to read DevID");
    } else {
        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "DevID: 0x%04X", reg);
    }
    
    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Uninitialize the FDC2214 sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_fdc2214_SleepEnter(void)
{
#if (FDC2214_ENABLE_INT == TRUE)
    return KETCUBE_CFG_MODULE_OK;
#else
    // UnInit drivers
    ketCube_fdc2214_off();
    return ketCube_I2C_UnInit();
#endif
}

/**
 * @brief Initialize the FDC2214 sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_fdc2214_SleepExit(void)
{
#if (FDC2214_ENABLE_INT == TRUE)
    return KETCUBE_CFG_MODULE_OK;
#else
    // UnInit drivers
    ketCube_fdc2214_on();
    return ketCube_I2C_Init();
#endif
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
    uint16_t msb, lsb, status;
    uint32_t raw;
    static uint32_t pastValue;

    if (ketCube_fdc2214_readReg(KETCUBE_FDC2214_STATUS, &status) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Unable to read STATUS");
    } else {
        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "STATUS: 0x%04X", status);
    }

    if (ketCube_fdc2214_readReg(KETCUBE_FDC2214_DATA_CH0, &msb) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Unable to read MSB");
    }
    if (ketCube_fdc2214_readReg(KETCUBE_FDC2214_DATA_LSB_CH0, &lsb) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Unable to read LSB");
    }
    if (ketCube_fdc2214_readReg(KETCUBE_FDC2214_STATUS, &status) == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "Unable to read STATUS");
    } else {
        raw = ((((uint32_t) msb) << 16) | lsb) & 0x0FFFFFFF;
        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "STATUS: 0x%04X; raw: 0x%08X", status, raw);
    }

    raw = ((((uint32_t) msb) << 16) | lsb) & 0x0FFFFFFF;

    buffer[0] = (uint8_t) (raw >> 24);
    buffer[1] = (uint8_t) (raw >> 16);
    buffer[2] = (uint8_t) (raw >> 8);
    buffer[3] = (uint8_t) raw;

    *len = 4;

    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "RawCapacity: %d", raw);

    // TODO redefine threshold
    if (abs(pastValue - raw) > 10000) {
#if (FDC2214_USE_LED_INDICATION == TRUE)
        //bool tmp_LED_state = ketCube_GPIO_Read(FDC2214_LED_PORT, FDC2214_LED_PIN);
        //ketCube_GPIO_Write(FDC2214_LED_PORT, FDC2214_LED_PIN, !tmp_LED_state);
        if (pastValue < raw) {
            ketCube_GPIO_Write(FDC2214_LED_PORT, FDC2214_LED_PIN, FALSE);
            ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "LED ON.");
        } else {
            ketCube_GPIO_Write(FDC2214_LED_PORT, FDC2214_LED_PIN, TRUE);
            ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "LED OFF.");
        }
#endif
    }
    
    pastValue = raw;
    
    return KETCUBE_CFG_MODULE_ERROR;
}

#endif                          /* KETCUBE_CFG_INC_MOD_FDC2214 */
