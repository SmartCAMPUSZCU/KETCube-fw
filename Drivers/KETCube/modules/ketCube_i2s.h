/**
 * @file    ketCube_i2s.h
 * @author  Krystof Vanek
 * @version alpha
 * @date    2018-09-18
 * @brief   This file contains definitions for the ketCube I2S driver
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

#ifndef __KETCUBE_I2S_H
#define __KETCUBE_I2S_H

#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_i2s.h"

#include "ketCube_cfg.h"

/** @defgroup KETCube_I2S KETCube I2S
  * @brief KETCube I2S module
  * @ingroup KETCube_ModuleDrivers
  * @{
  */

/** @defgroup KETCube_I2S_defs Public Deffinitions
  * @brief Public defines
  * @{
  */

#define KETCUBE_I2S_HANDLE                     SPI2

#define KETCUBE_I2S_SAMPLE_RATE				   I2S_AUDIOFREQ_32K    /* Available: 8,11,16,22,32,44,48 kHz */


#define KETCUBE_I2S_CLK_ENABLE()               __SPI2_CLK_ENABLE()
#define KETCUBE_I2S_FORCE_RESET()              __SPI2_FORCE_RESET()
#define KETCUBE_I2S_RELEASE_RESET()            __SPI2_RELEASE_RESET()
#define KETCUBE_I2S_WS_CK_SD_GPIO_CLK_ENABLE() __GPIOB_CLK_ENABLE()
#define KETCUBE_I2S_WS_CK_SD_AF                GPIO_AF0_SPI2
#define KETCUBE_I2S_WS_CK_SD_GPIO_PORT         GPIOB
#define KETCUBE_I2S_WS_PIN                     GPIO_PIN_12
#define KETCUBE_I2S_CK_PIN                     GPIO_PIN_13
#define KETCUBE_I2S_SD_PIN                     GPIO_PIN_15
#define KETCUBE_I2S_EV_IRQn                    SPI2_IRQn

/**
* @}
*/

/** @defgroup KETCube_I2S_fn Public Functions
  * @brief Public functions
  * @{
  */

ketCube_cfg_ModError_t ketCube_I2S_Init(void);
ketCube_cfg_ModError_t ketCube_I2S_UnInit(void);

/**
* @}
*/

/**
* @}
*/

#endif                          /* __KETCUBE_I2S_H */
