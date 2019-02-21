/**
 * @file    ketCube_fdc2214.c
 * @author  Jan Belohoubek
 * @version alpha
 * @date    2019-01-01
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

ketCube_fdc2214_moduleCfg_t ketCube_fdc2214_moduleCfg; /*<! Module configuration storage */

/**
 * @brief Exit the FDC2214 shut-down mode
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
static inline void ketCube_fdc2214_on(void)
{
    ketCube_GPIO_Write(FDC2214_SD_PORT, FDC2214_SD_PIN, FALSE);
    HAL_Delay(KETCUBE_FDC2214_I2C_WU);
}

/**
 * @brief Enter the FDC2214 shut-down mode
 *
 * @note Entering shut-down mode will reset all registers to their default states
 * 
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
static inline void ketCube_fdc2214_off(void)
{
    ketCube_GPIO_Write(FDC2214_SD_PORT, FDC2214_SD_PIN, TRUE);
}

/**
 * @brief Generic function to write the FDC2214 register
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
static ketCube_cfg_ModError_t ketCube_fdc2214_writeReg(uint8_t regAddr,
                                                       uint16_t * reg,
                                                       const char *str)
{
    uint8_t i = 0;

    for (i = 0; i < KETCUBE_FDC2214_I2C_TRY; i++) {
        if (ketCube_I2C_TexasWriteReg
            (KETCUBE_FDC2214_I2C_ADDRESS, regAddr,
             reg) == KETCUBE_CFG_MODULE_OK) {
            break;
        }
    }
    if (i == KETCUBE_FDC2214_I2C_TRY) {
        if (str != NULL) {
            ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214,
                                          "Unable to write %s (RegAddr: 0x%02X)",
                                          str, regAddr);
        }
        return KETCUBE_CFG_MODULE_ERROR;
    }

    if (str != NULL) {
        ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_FDC2214,
                                         "New %s value: 0x%04X", str,
                                         *reg);
    }

    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Generic function to read the FDC2214 register
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
static ketCube_cfg_ModError_t ketCube_fdc2214_readReg(uint8_t regAddr,
                                                      uint16_t * reg,
                                                      const char *str)
{
    uint8_t i = 0;

    *reg = 0;
    for (i = 0; i < KETCUBE_FDC2214_I2C_TRY; i++) {
        if ((ketCube_I2C_TexasReadReg
             (KETCUBE_FDC2214_I2C_ADDRESS, regAddr,
              reg)) == KETCUBE_CFG_MODULE_OK) {
            break;
        }
    }
    if (i == KETCUBE_FDC2214_I2C_TRY) {
        if (str != NULL) {
            ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214,
                                          "Unable to read %s (RegAddr: 0x%02X)",
                                          str, regAddr);
        }
        return KETCUBE_CFG_MODULE_ERROR;
    }

    if (str != NULL) {
        ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_FDC2214,
                                         "%s: 0x%04X", str, *reg);
    }

    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Exit the FDC2214 sleep-down mode
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
static ketCube_cfg_ModError_t ketCube_fdc2214_wakeUp(void)
{
    uint16_t reg;

    if (ketCube_fdc2214_readReg
        (KETCUBE_FDC2214_CONFIG, &reg,
         (char *) &"ConfigReg") == KETCUBE_CFG_MODULE_ERROR) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    reg &= ~(0x1 << 13);

    if (ketCube_fdc2214_writeReg
        (KETCUBE_FDC2214_CONFIG, &reg,
         (char *) &"ConfigReg") == KETCUBE_CFG_MODULE_ERROR) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    HAL_Delay(KETCUBE_FDC2214_I2C_WU);

    return KETCUBE_CFG_MODULE_OK;
}


/**
 * @brief Enter the FDC2214 sleep-down mode
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
static ketCube_cfg_ModError_t ketCube_fdc2214_sleep(void)
{
    uint16_t reg;

    if (ketCube_fdc2214_readReg
        (KETCUBE_FDC2214_CONFIG, &reg,
         (char *) &"ConfigReg") == KETCUBE_CFG_MODULE_ERROR) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    reg |= (0x1 << 13);

    if (ketCube_fdc2214_writeReg
        (KETCUBE_FDC2214_CONFIG, &reg,
         (char *) &"ConfigReg") == KETCUBE_CFG_MODULE_ERROR) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Soft-reset the FDC2214 sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
static inline ketCube_cfg_ModError_t ketCube_fdc2214_reset(void)
{
    uint16_t reg;

    reg = 0x8000;
    if (ketCube_fdc2214_writeReg
        (KETCUBE_FDC2214_RESET_DEV, &reg,
         (char *) &"ResetReg") != KETCUBE_CFG_MODULE_OK) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    HAL_Delay(KETCUBE_FDC2214_I2C_WU);

    return KETCUBE_CFG_MODULE_OK;
}

static uint32_t pastValue;
static uint32_t values[FDC2214_SAMPLE_CNT];
static uint32_t valuePtr = 0;
static uint32_t mean = 0;
static uint8_t wait = 0;

/**
 * @brief LED indication
 *
 * @param raw fdc2214 current raw value produced by fdc2214 sensor
 * 
 */
static void ketCube_fdc2214_LEDIndication(uint32_t raw)
{
#if (FDC2214_LED_INDICATION == TRUE)
        //ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214,"%d; %d", (mean - raw), (pastValue - raw));
        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214,"%d", (pastValue - raw));
        
        wait = (wait + 1) % FDC2214_LED_HYST;
        
//         if ( (mean < raw) && (abs(mean - raw) > FDC2214_LED_THRESHOLD_LOW) ) {
//             ketCube_GPIO_Write(FDC2214_LED_PORT, FDC2214_LED_PIN, FALSE);
//             ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "LED ON.");
//             pastValue = raw;
//             wait = 0;
//         } else if ((wait == 0) && (pastValue > raw) && (abs(pastValue - raw) > FDC2214_LED_THRESHOLD_HIGH)) {
//             ketCube_GPIO_Write(FDC2214_LED_PORT, FDC2214_LED_PIN, TRUE);
//             ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "LED OFF.");
//             pastValue = raw;
//         }
        
        if ( (pastValue > raw) && (abs(pastValue - raw) > FDC2214_LED_THRESHOLD_HIGH) ) {
            ketCube_GPIO_Write(FDC2214_LED_PORT, FDC2214_LED_PIN, FALSE);
            ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "LED ON.");
        } else if ((pastValue < raw) && (abs(pastValue - raw) > FDC2214_LED_THRESHOLD_LOW)) {
            ketCube_GPIO_Write(FDC2214_LED_PORT, FDC2214_LED_PIN, TRUE);
            ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "LED OFF.");
        }
        pastValue = raw;
        
        //values[valuePtr] = raw;
        //valuePtr = (valuePtr + 1) % FDC2214_SAMPLE_CNT;
        //mean = ketCube_common_Med32(&(values[0]), FDC2214_SAMPLE_CNT);
#endif
}


void ketCube_fdc2214_irqStatusUpdated()
{
    uint16_t msb, lsb;
    uint32_t raw;
    uint8_t i;

    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214, "INT");

    // Loop through data registers
    // note, that the trick for the loop condition can be found in the ketCube_fdc2214_chanSeq_t deffinition
    for (i = (FDC2214_CHAN_SEQ & 0x0F);
         i <= ((FDC2214_CHAN_SEQ & 0x0F) | (FDC2214_CHAN_SEQ >> 4)); i++) {

        ketCube_fdc2214_readReg(KETCUBE_FDC2214_DATA_CH0, &msb,
                                (char *) NULL);
        ketCube_fdc2214_readReg(KETCUBE_FDC2214_DATA_LSB_CH0, &lsb,
                                (char *) NULL);

        raw = ((((uint32_t) msb) << 16) | lsb) & 0x0FFFFFFF;

        if (FDC2214_LED_CHAN == i) {
            // execute LED indication function
            ketCube_fdc2214_LEDIndication(raw);
        }
    }
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
    uint8_t i;

    GPIO_InitTypeDef GPIO_InitStruct;

    // Init drivers
    if (ketCube_I2C_Init() != KETCUBE_CFG_MODULE_OK) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    if (ketCube_GPIO_Init
        (FDC2214_SD_PORT, FDC2214_SD_PIN,
         &GPIO_InitStruct) != KETCUBE_CFG_DRV_OK) {
        return KETCUBE_CFG_MODULE_ERROR;
    }
#if (FDC2214_LED_INDICATION == TRUE)
    if (ketCube_GPIO_Init
        (FDC2214_LED_PORT, FDC2214_LED_PIN,
         &GPIO_InitStruct) != KETCUBE_CFG_DRV_OK) {
        return KETCUBE_CFG_MODULE_ERROR;
    }
#endif

    // enable interrupt
#if (FDC2214_ENABLE_INT == TRUE)
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Pin = FDC2214_INT_PIN;

    ketCube_GPIO_Init(FDC2214_INT_PORT, FDC2214_INT_PIN, &GPIO_InitStruct);

    if (ketCube_GPIO_SetIrq
        (FDC2214_INT_PORT, FDC2214_INT_PIN, 0,
         &ketCube_fdc2214_irqStatusUpdated) != KETCUBE_CFG_DRV_OK) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214,
                                      "Unable to setUp IRQ!");
        return KETCUBE_CFG_MODULE_ERROR;
    }
#endif

    // Power ON
    ketCube_fdc2214_off();
    ketCube_fdc2214_on();

    // Reset
    if (ketCube_fdc2214_reset() == KETCUBE_CFG_MODULE_ERROR) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_FDC2214,
                                      "Unable to reset!");
        return KETCUBE_CFG_MODULE_ERROR;
    }
    // get DevID/ManID
    ketCube_fdc2214_readReg(KETCUBE_FDC2214_DEVICE_ID, &reg,
                            (char *) &"DevID");
    ketCube_fdc2214_readReg(KETCUBE_FDC2214_MANUFACTURER_ID, &reg,
                            (char *) &"ManID");

    // reserved bits configuration
    reg = 0x1c01;
#if (FDC2214_USE_EXTERNAL_OSC == TRUE)
    reg |= (0x1 << 9);
#endif

#if (FDC2214_ENABLE_INT == FALSE)
    reg |= 0x0080;
#endif

#if (FDC2214_CHAN_SEQ <= FDC2214_SINGLE_CH3)
    reg |= (FDC2214_CHAN_SEQ << 14);
#endif

    if (ketCube_fdc2214_writeReg
        (KETCUBE_FDC2214_CONFIG, &reg,
         (char *) &"ConfigReg") == KETCUBE_CFG_MODULE_ERROR) {
        return KETCUBE_CFG_MODULE_ERROR;
    }
    // Reserved bits must be set accordingly!
    reg = (0x41 << 3);

#if (FDC2214_CHAN_SEQ > FDC2214_SINGLE_CH3)
    reg |= (0x1 << 15);
    reg |= (((FDC2214_CHAN_SEQ >> 4) - 1) << 13);       // set RR_Sequence
#else
    reg |= (0x2 << 13);         // set RR_Sequence
#endif
    // set deglitch filter
    reg |= FDC2214_DGL_FILTER;

    if (ketCube_fdc2214_writeReg
        (KETCUBE_FDC2214_MUX_CONFIG, &reg,
         (char *) &"MuxConfigReg") == KETCUBE_CFG_MODULE_ERROR) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    reg = 0;
#if (FDC2214_ENABLE_INT == TRUE)
    reg |= 0x0001;
#endif

    if (ketCube_fdc2214_writeReg
        (KETCUBE_FDC2214_STATUS_CONFIG, &reg,
         (char *) &"StatusConfigReg") == KETCUBE_CFG_MODULE_ERROR) {
        return KETCUBE_CFG_MODULE_ERROR;
    }
    // Init channels you need to initialize ...
    // note, that the trick for the loop condition can be found in the ketCube_fdc2214_chanSeq_t deffinition
    for (i = (FDC2214_CHAN_SEQ & 0x0F);
         i <= ((FDC2214_CHAN_SEQ & 0x0F) | (FDC2214_CHAN_SEQ >> 4)); i++) {

        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214,
                                     "Setting CH%d", i);

        reg = FDC2214_RCOUNT;
        if (ketCube_fdc2214_writeReg
            (KETCUBE_FDC2214_RCOUNT_CH0 + i, &reg,
             (char *) &"RCountReg") == KETCUBE_CFG_MODULE_ERROR) {
            return KETCUBE_CFG_MODULE_ERROR;
        }

        reg = FDC2214_SETTLECOUNT;
        if (ketCube_fdc2214_writeReg
            (KETCUBE_FDC2214_SETTLECOUNT_CH0 + i, &reg,
             (char *) &"SettleCountReg") == KETCUBE_CFG_MODULE_ERROR) {
            return KETCUBE_CFG_MODULE_ERROR;
        }
        // set default value for clock divider; It MUST != 0
        reg = 0x0001;
#if (FDC2214_SENSTYPE == FDC2214_SENSTYPE_SINGLE_ENDED)
        reg |= 0x2 << 12;
#else
        reg |= 0x1 << 12;       // choose 10 for differential sensor with higher frequency
#endif
        if (ketCube_fdc2214_writeReg
            (KETCUBE_FDC2214_CLOCK_DIVIDERS_CH0 + i, &reg,
             (char *) &"ClkDivReg") == KETCUBE_CFG_MODULE_ERROR) {
            return KETCUBE_CFG_MODULE_ERROR;
        }

        reg = ((0x11) << 11);
        if (ketCube_fdc2214_writeReg
            (KETCUBE_FDC2214_DRIVE_CURRENT_CH0 + i, &reg,
             (char *) &"CurrentReg") == KETCUBE_CFG_MODULE_ERROR) {
            return KETCUBE_CFG_MODULE_ERROR;
        }

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
    ketCube_cfg_ModError_t ret = KETCUBE_CFG_MODULE_OK;

#if (FDC2214_ENABLE_INT == FALSE) && (FDC2214_ENABLE_SLEEP == TRUE)
    ret = ketCube_fdc2214_sleep();
#elif (FDC2214_ENABLE_INT == FALSE) && (FDC2214_ENABLE_SLEEP == FALSE)
    // UnInit drivers
    ketCube_fdc2214_off();
    ret = ketCube_I2C_UnInit();
#endif

    return ret;
}

/**
 * @brief Initialize the FDC2214 sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_fdc2214_SleepExit(void)
{
    ketCube_cfg_ModError_t ret = KETCUBE_CFG_MODULE_OK;

#if (FDC2214_ENABLE_INT == FALSE) && (FDC2214_ENABLE_SLEEP == TRUE)
    ret = ketCube_fdc2214_wakeUp();
#elif (FDC2214_ENABLE_INT == FALSE) && (FDC2214_ENABLE_SLEEP == FALSE)
    ret = ketCube_fdc2214_Init();
#endif
    return ret;

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
    uint8_t i;

    // check status reg
    if (ketCube_fdc2214_readReg
        (KETCUBE_FDC2214_STATUS, &status,
         (char *) &"StatusReg") == KETCUBE_CFG_MODULE_ERROR) {
        // try re-init ... 
        ketCube_fdc2214_SleepEnter();
        ketCube_fdc2214_off();
        if (ketCube_fdc2214_Init(NULL) == KETCUBE_CFG_MODULE_ERROR) {
            while(1) {
                ketCube_GPIO_Write(FDC2214_LED_PORT, FDC2214_LED_PIN, FALSE);
                HAL_Delay(500);
                ketCube_GPIO_Write(FDC2214_LED_PORT, FDC2214_LED_PIN, TRUE);
                HAL_Delay(500);
            }
            //NVIC_SystemReset();
            return KETCUBE_CFG_MODULE_ERROR;
        }
    }
    // Loop through data registers
    // note, that the trick for the loop condition can be found in the ketCube_fdc2214_chanSeq_t deffinition
    for (i = (FDC2214_CHAN_SEQ & 0x0F);
         i <= ((FDC2214_CHAN_SEQ & 0x0F) | (FDC2214_CHAN_SEQ >> 4)); i++) {

        ketCube_fdc2214_readReg(KETCUBE_FDC2214_DATA_CH0, &msb,
                                (char *) &"MSB");
        ketCube_fdc2214_readReg(KETCUBE_FDC2214_DATA_LSB_CH0, &lsb,
                                (char *) &"LSB");

        raw = ((((uint32_t) msb) << 16) | lsb) & 0x0FFFFFFF;

        buffer[4 * i + 0] = (uint8_t) (raw >> 24);
        buffer[4 * i + 1] = (uint8_t) (raw >> 16);
        buffer[4 * i + 2] = (uint8_t) (raw >> 8);
        buffer[4 * i + 3] = (uint8_t) raw;

        *len += 4;

        /*ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_FDC2214,
                                     "CH%d RawCapacity: 0x%08X (%d: %d; %d)",
                                     i, raw, raw, (pastValue - raw), (meanValue - raw));*/

        if (FDC2214_LED_CHAN == i) {
            // execute LED indication function
            ketCube_fdc2214_LEDIndication(raw);
        }
    }

    // check status reg
    ketCube_fdc2214_readReg(KETCUBE_FDC2214_STATUS, &status,
                            (char *) &"StatusReg");

    return KETCUBE_CFG_MODULE_OK;
}

#endif                          /* KETCUBE_CFG_INC_MOD_FDC2214 */
