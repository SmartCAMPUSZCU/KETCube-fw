/**
 * @file    ketCube_i2s.c
 * @author  Krystof Vanek
 * @version alpha
 * @date    2018-09-18
 * @brief   This file contains the ketCube I2S driver
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
#include "stm32l0xx_hal_i2s.h"

#include "ketCube_cfg.h"
#include "ketCube_i2s.h"

// local fn declarations
I2S_HandleTypeDef KETCUBE_I2S_Handle;

static uint8_t initRuns = 0;    //< This driver can be initialised only once. If 0 == not initialised, else initialised

/**
 * @brief  Configures I2S interface.
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_I2S_Init(void)
{
    initRuns += 1;

    if (initRuns > 1) {
        return KETCUBE_CFG_MODULE_OK;
    }

    if (HAL_I2S_GetState(&KETCUBE_I2S_Handle) == HAL_I2S_STATE_RESET) {

        /* I2S peripheral configuration */
        KETCUBE_I2S_Handle.Instance = KETCUBE_I2S_HANDLE;
        KETCUBE_I2S_Handle.Init.Mode = I2S_MODE_MASTER_RX;
        KETCUBE_I2S_Handle.Init.Standard = I2S_STANDARD_PHILIPS;
        KETCUBE_I2S_Handle.Init.DataFormat = I2S_DATAFORMAT_24B;
        KETCUBE_I2S_Handle.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
        KETCUBE_I2S_Handle.Init.AudioFreq = KETCUBE_I2S_SAMPLE_RATE;
        KETCUBE_I2S_Handle.Init.CPOL = I2S_CPOL_HIGH;

        /* Init the I2S GPIO */
        GPIO_InitTypeDef GPIO_InitStruct;
        /* Enable I2S GPIO clocks */
        KETCUBE_I2S_WS_CK_SD_GPIO_CLK_ENABLE();

        /* I2S WS,CK and SD pins configuration ------------------------------------- */
        GPIO_InitStruct.Pin = KETCUBE_I2S_WS_PIN | KETCUBE_I2S_CK_PIN
            | KETCUBE_I2S_SD_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_LOW; /*  For yet unknown reasons the I2S bus works well with LOW
                                                   speed setting, despite being quite fast (0.5~3 MHz).
                                                   Setting speed to high introduces frame alignment errors. */
        GPIO_InitStruct.Alternate = KETCUBE_I2S_WS_CK_SD_AF;
        HAL_GPIO_Init(KETCUBE_I2S_WS_CK_SD_GPIO_PORT, &GPIO_InitStruct);

        /* Enable the I2S peripheral clock */
        KETCUBE_I2S_CLK_ENABLE();
        /* Force the I2S peripheral clock reset */
        KETCUBE_I2S_FORCE_RESET();

        /* Release the I2S peripheral clock reset */
        KETCUBE_I2S_RELEASE_RESET();

        /* Initialise I2S peripheral */
        HAL_I2S_Init(&KETCUBE_I2S_Handle);
        uint16_t packet;

        /* Start I2S bus clock generation */
        HAL_I2S_Receive(&KETCUBE_I2S_Handle, &packet, 1, 1000);
        __HAL_I2S_ENABLE_IT(&KETCUBE_I2S_Handle, I2S_IT_RXNE);
    }

    if (HAL_I2S_GetState(&KETCUBE_I2S_Handle) == HAL_I2S_STATE_READY) {
        return KETCUBE_CFG_MODULE_OK;
    } else {
        return KETCUBE_CFG_MODULE_ERROR;
    }
}
