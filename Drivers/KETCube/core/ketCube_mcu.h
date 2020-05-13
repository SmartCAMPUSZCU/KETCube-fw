/**
 * @file    ketCube_mcu.h
 * @author  Jan Belohoubek
 * @version alpha
 * @date    2019-12-10
 * @brief   This file contains the KETCube MCU defs
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 University of West Bohemia in Pilsen
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

#ifndef __KETCUBE_MCU_H
#define __KETCUBE_MCU_H

#include "ketCube_cfg.h"

/** @defgroup KETCube_MCU KETCube MCU
  * @brief KETCube MCU
  * @ingroup KETCube_Core
  * @{
  */

/* Low Power modes selection */
#define KETCUBE_MCU_LPMODE_STOP   0
#define KETCUBE_MCU_LPMODE_SLEEP  1

#define KETCUBE_MCU_LPMODE   KETCUBE_MCU_LPMODE_STOP

#define KETCUBE_MCU_WDT      30                         /*<! Watchdog reset period in (pseudo)seconds (seconds are approximated by the closest power of 2); use 30 as the maxValue ! */


/** @defgroup KETCube_MCU_fn Public Functions
* @{
*/

extern uint32_t ketCube_MCU_GetRandomSeed(void);
extern void ketCube_MCU_GetUniqueId(uint8_t *id);

extern void ketCube_MCU_Sleep(void);
extern void ketCube_MCU_EnableSleep(void);
extern void ketCube_MCU_DisableSleep(void);
extern bool ketCube_MCU_IsSleepEnabled(void);

extern void ketCube_MCU_WD_Init(void);
extern void ketCube_MCU_WD_Reset(void);

extern void ketCube_MCU_ClockConfig(void);

/**
* @}
*/



/**
* @}
*/

#endif                          /* __KETCUBE_MCU_H */