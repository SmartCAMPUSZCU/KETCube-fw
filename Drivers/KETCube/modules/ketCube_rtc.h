/**
 * @file    ketCube_rtc.c
 * @author  Jan Belohoubek
 * @version alpha
 * @date    2019-12-10
 * @brief   This file contains definitions for the ketCube RTC driver.
 *
 * @note This code is based on Semtech and STM RTC driver implementation. 
 * See the original file Semtech licenses in LICENSE_SEMTECH. The ST Ultimate 
 * Liberty license is dennoted SLA0044. You may not use this file except in
 * compliance with the License. You may obtain a copy of the License at:
 * www.st.com/SLA0044
 * 
 * @attention
 * 
 * <h2><center>&copy; Copyright (c) 2013, SEMTECH S.A.
 * All rights reserved.</center></h2>
 * 
 * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
 * All rights reserved.</center></h2>
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


#ifndef __KETCUBE_RTC_H__
#define __KETCUBE_RTC_H__

#include "utilities.h"


/** @defgroup KETCube_RTC KETCube RTC driver
  * @brief KETCube RTC driver
  * @ingroup KETCube_ModuleDrivers
  * @{
  */
   
/**
 * @brief Temperature coefficient of the clock source
 */
#define RTC_TEMP_COEFFICIENT                            ( -0.035 )

/**
 * @brief Temperature coefficient deviation of the clock source
 */
#define RTC_TEMP_DEV_COEFFICIENT                        ( 0.0035 )

/**
 * @brief Turnover temperature of the clock source
 */
#define RTC_TEMP_TURNOVER                               ( 25.0 )

/**
 * @brief Turnover temperature deviation of the clock source
 */
#define RTC_TEMP_DEV_TURNOVER                           ( 5.0 )


/** @defgroup KETCube_RTC_fn Public Functions
  * @brief Public functions
  * @{
  */

extern ketCube_cfg_DrvError_t ketCube_RTC_Init(void);

extern void ketCube_RTC_StopAlarm();
extern uint32_t ketCube_RTC_GetMinimumTimeout();
extern void ketCube_RTC_SetAlarm(uint32_t timeout);
extern uint32_t ketCube_RTC_GetTimerElapsedTime(void);
extern uint32_t ketCube_RTC_GetTimerValue(void);
extern uint32_t ketCube_RTC_SetTimerContext(void);
extern uint32_t ketCube_RTC_GetTimerContext(void) ;
extern void ketCube_RTC_DelayMs(uint32_t delay);
extern void ketCube_RTC_setMcuWakeUpTime();
extern int16_t ketCube_RTC_getMcuWakeUpTime(void);
extern uint32_t ketCube_RTC_ms2Tick(TimerTime_t timeMicroSec);
extern TimerTime_t ketCube_RTC_Tick2ms(uint32_t tick);
extern TimerTime_t ketCube_RTC_Tick2us(uint32_t tick);
extern uint32_t ketCube_RTC_GetCalendarTime(uint16_t *subSeconds);
uint32_t ketCube_RTC_GetSysTime(void);
extern void ketCube_RTC_BKUPWrite( uint32_t Data0, uint32_t Data1);
extern void ketCube_RTC_BKUPRead( uint32_t *Data0, uint32_t *Data1);

extern uint32_t HAL_GetTick(void);

extern void ketCube_RTC_IrqHandler(void);

void ketCube_RTC_AlarmAEventExec(void);

/**
* @}
*/

/**
* @}
*/



#endif /* __KETCUBE_RTC_H__ */
