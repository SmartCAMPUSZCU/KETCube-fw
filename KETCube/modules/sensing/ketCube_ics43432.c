/**
 * @file    ketCube_ics43432.c
 * @author  Krystof Vanek
 * @version pre-alpha
 * @date    2018-11-18
 * @brief   This file contains the ICS43432 microphone module
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
#include "ketCube_i2s.h"
#include "ketCube_ics43432.h"
#include "ketCube_gpio.h"

#ifdef KETCUBE_CFG_INC_MOD_ICS43432

ketCube_ics43432_moduleCfg_t ketCube_ics43432_moduleCfg; /*!< Module configuration storage */

static volatile uint32_t noiseCnt = 0;

ketCube_cfg_ModError_t ketCube_ics43432_Init(ketCube_InterModMsg_t *** msg)
{
    /* Initialise I2S bus and start synchronization */
    if (ketCube_I2S_Init() != KETCUBE_CFG_MODULE_OK) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_ICS43432,
                                      "I2S bus initialisation failed!");
        return KETCUBE_CFG_MODULE_ERROR;
    }

    /* Initialise LED pin */
    GPIO_InitTypeDef initStruct;
    initStruct.Mode = GPIO_MODE_OUTPUT_PP;
    initStruct.Pull = GPIO_NOPULL;
    initStruct.Speed = GPIO_SPEED_LOW;
    initStruct.Pin = KETCUBE_ICS43432_NOISE_LED_PIN;

    ketCube_GPIO_ReInit(KETCUBE_ICS43432_NOISE_LED_PORT,
                        KETCUBE_ICS43432_NOISE_LED_PIN, &initStruct);

    /* Enable and set I2S Interrupt to lower priority */
    HAL_NVIC_SetPriority(KETCUBE_I2S_EV_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(KETCUBE_I2S_EV_IRQn);

    return KETCUBE_CFG_MODULE_OK;
}

ketCube_cfg_ModError_t ketCube_ics43432_UnInit(void)
{
    return KETCUBE_CFG_MODULE_OK;
}

ketCube_cfg_ModError_t ketCube_ics43432_ReadData(uint8_t * buffer,
                                                 uint8_t * len)
{
    uint8_t i = 0;

    // Fraction of samples over threshold scaled to byte
    buffer[i++] =
        (uint8_t) (255 * KETCUBE_ICS43432_MAX_SAMPLE_COUNT / noiseCnt);
    *len = i;

    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_ICS43432,
                                 "Noise counter: %5d", noiseCnt);

    noiseCnt = 0;

    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief I2S interrupt handler
 */
void SPI2_IRQHandler()
{
    //Read data register to clear interrupt flag
    uint16_t temp = SPI2->DR;
    static uint16_t tempH;
    static uint32_t U32;
    static int32_t I32;
    static bool sync = false;

    /**
      * Synchronization flag is set false in right
      * side words and true after first word of
      * left channel.
      * Thus it is used to determine order of recieved
      * words.
      */

    if (SPI2->SR & SPI_SR_CHSIDE) {
        /* CHSIDE == 1 => Right Channel */
        sync = false;
    } else {
        /* CHSIDE == 0 => Left Channel */
        if (SPI2->SR & SPI_SR_OVR) {
            sync = false;
        } else {
            if (sync == false) {
                tempH = temp;
                sync = true;
            } else {
                U32 = (((uint32_t) tempH) << 16) | temp;
                I32 = (*((int32_t *) (&U32))) >> 8;
                if (abs(I32) > 30000) {
                    if (noiseCnt < UINT32_MAX) {
                        noiseCnt += 1;
                    }
                    ketCube_GPIO_Write(KETCUBE_ICS43432_NOISE_LED_PORT,
                                       KETCUBE_ICS43432_NOISE_LED_PIN,
                                       FALSE);
                } else {
                    ketCube_GPIO_Write(KETCUBE_ICS43432_NOISE_LED_PORT,
                                       KETCUBE_ICS43432_NOISE_LED_PIN,
                                       TRUE);
                }
            }
        }
    }
}

#endif                          // KETCUBE_CFG_INC_MOD_ICS43432
