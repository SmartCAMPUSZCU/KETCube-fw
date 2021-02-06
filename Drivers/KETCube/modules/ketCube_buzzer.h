/**
 * @file    ketCube_buzzer.h
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2018-07-12
 * @brief   This file contains definitions for the KETCube Buzzer module
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
#ifndef __KETCUBE_BUZZER_H
#define __KETCUBE_BUZZER_H

#include "ketCube_cfg.h"

/** @defgroup KETCube_buzzer KETCube Buzzer
  * @brief KETCube Buzzer driver
  * @ingroup KETCube_ActMods
  * @{
  */

#define KETCUBE_BUZZER_NAME              "buzzer_drv"         /*!<  Buzzer driver name */

#define KETCUBE_BUZZER_TIMER             KETCUBE_TIMER_LIST_LPTIM

#define KETCUBE_BUZZER_TICKS_PER_HZ     4.521                 /*!< Timer ticks per 1 Hz */

#define KETCUBE_BUZZER_SOUND_QUEUE      8                     /*!< Sound queue depth */

/**
* @brief  KETCube module configuration
*/
typedef struct ketCube_buzzer_beep_t {
    uint16_t frequency;   /*!< Buzzer frequency [Hz] */
    uint16_t duration;    /*!< Beep duration [ms] */
    uint16_t mute;        /*!< Mute duration [ms] */
    bool active;          /*!< Is active now or not */
} ketCube_buzzer_beep_t;

/** @defgroup KETCube_buzzer_fn Public Functions
* @{
*/

extern ketCube_cfg_DrvError_t ketCube_buzzer_Init();
extern ketCube_cfg_DrvError_t ketCube_buzzer_UnInit();
extern void ketCube_buzzer_Beep(uint16_t freq, uint16_t dur, uint16_t mute);
extern bool ketCube_buzzer_inProgress();

/**
* @}
*/


/**
* @}
*/

#endif                          /* __KETCUBE_BUZZER_H */
