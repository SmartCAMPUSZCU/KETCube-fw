/**
 * @file    ketCube_i2c.h
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2018-07-12
 * @brief   This file contains definitions for the ketCube I2C driver
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KETCUBE_AD_H
#define __KETCUBE_AD_H

#include "ketCube_cfg.h"

/** @defgroup KETCube_AD KETCube AD
  * @brief KETCube ADC driver
  * @ingroup KETCube_ModuleDrivers
  * @{
  */

/** @defgroup KETCube_AD_defs Public Deffinitions
  * @brief Public defines
  * @{
  */

#define KETCUBE_AD_NAME                      "ad_drv"         ///< AD driver name

#define KETCUBE_AD_VREFINT_MAX_TIMEOUT_MS     10              ///< Max timeout to stabilize vrefint

/**
* @}
*/

/** @defgroup KETCube_AD_fn Public Functions
  * @brief Public functions
  * @{
  */

extern ketCube_cfg_DrvError_t ketCube_AD_Init(void);
extern ketCube_cfg_DrvError_t ketCube_AD_UnInit(void);

/* General-purpose functions */
extern uint16_t ketCube_AD_ReadChannel(uint32_t channel);
extern uint16_t ketCube_AD_ReadChannelmV(uint32_t channel);

/* ADC channels connected to internal MCU sensors */
extern uint32_t ketCube_AD_GetBatLevelmV(void);
extern uint16_t ketCube_AD_GetTemperature(void);

/**
* @}
*/

/**
* @}
*/

#endif                          /* __KETCUBE_AD_H */
