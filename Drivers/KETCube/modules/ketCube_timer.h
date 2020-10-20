/**
 * @file    ketCube_timer.h
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2018-07-12
 * @brief   This file contains definitions for the ketCube Timer(s) driver
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
#ifndef __KETCUBE_TIMER_H
#define __KETCUBE_TIMER_H

#include "ketCube_cfg.h"

/** @defgroup KETCube_Timer KETCube timer
  * @brief KETCube Timer module
  * @ingroup KETCube_ModuleDrivers
  * @{
  */

#define KETCUBE_TIMER_NAME                      "timer_drv"         ///< TIMER driver name

extern TIM_HandleTypeDef KETCube_Timer_Htim2;

/**
* @brief List of available timers
*/
typedef enum {
    KETCUBE_TIMER_LIST_TIM2,    ///< 16-bit auto-reload up/down counter
    KETCUBE_TIMER_LIST_TIM3,    ///< 16-bit auto-reload up/down counter
    KETCUBE_TIMER_LIST_TIM21,   ///< 16-bit auto-reload up/down counter.
    KETCUBE_TIMER_LIST_TIM22,   ///< 16-bit auto-reload up/down counter.
    KETCUBE_TIMER_LIST_TIM6,    ///< A generic 16-bit timebase
    KETCUBE_TIMER_LIST_TIM7,    ///< A generic 16-bit timebase
    KETCUBE_TIMER_LIST_LPTIM,   ///< The low-power timer

    KETCUBE_TIMER_LIST_CNT      ///< # of timers
} ketCube_Timer_list_t;

/**
* @brief Timer Usage
*/
typedef struct {
    bool tim2:1;
    bool tim3:1;
    bool tim21:1;
    bool tim22:1;
    bool tim6:1;
    bool tim7:1;
    bool lptim:1;
} ketCube_Timer_usage_t;

/** @defgroup KETCube_Timer_fn Public Functions
  * @brief Public functions
  * @{
  */

extern ketCube_cfg_ModError_t ketCube_Timer_Init(ketCube_Timer_list_t tim);

extern bool ketCube_Timer_Timer2_IsICEvent();
extern void ketCube_Timer_Timer2_ResetICEvent();

/**
* @}
*/

/**
* @}
*/

#endif                          /* __KETCUBE_TIMER_H */
