/**
 * @file    hw_rtc.h
 * @author  Jan Belohoubek
 * @version alpha
 * @date    2019-12-10
 * @brief   This is the wrapper for compatibility with the Semtech code
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

#ifndef __HW_RTC_H__
#define __HW_RTC_H__

#include "ketCube_rtc.h"

static inline void HW_RTC_StopAlarm() {
     ketCube_RTC_StopAlarm();
}

static inline uint32_t HW_RTC_GetMinimumTimeout() {
     return ketCube_RTC_GetMinimumTimeout();
}

static inline void HW_RTC_SetAlarm( uint32_t timeout ) {
    ketCube_RTC_SetAlarm(timeout);
}

static inline uint32_t HW_RTC_GetTimerElapsedTime(void) {
    return ketCube_RTC_GetTimerElapsedTime();
}

static inline uint32_t HW_RTC_GetTimerValue(void) {
    return ketCube_RTC_GetTimerValue();
}

static inline uint32_t HW_RTC_SetTimerContext(void) {
    return ketCube_RTC_SetTimerContext();
}
  
static inline uint32_t HW_RTC_GetTimerContext(void) {
    return ketCube_RTC_GetTimerContext();
}

static inline void HW_RTC_setMcuWakeUpTime() {
     ketCube_RTC_setMcuWakeUpTime();
}


static inline int16_t HW_RTC_getMcuWakeUpTime(void) {
    return ketCube_RTC_getMcuWakeUpTime();
}

static inline uint32_t HW_RTC_ms2Tick(TimerTime_t timeMicroSec) {
    return ketCube_RTC_ms2Tick(timeMicroSec);
}

static inline TimerTime_t HW_RTC_Tick2ms(uint32_t tick) {
    return ketCube_RTC_Tick2ms(tick);
}

static inline uint32_t HW_RTC_GetCalendarTime(uint16_t *subSeconds) {
    return ketCube_RTC_GetCalendarTime(subSeconds);
}

static inline void HW_RTC_BKUPWrite(uint32_t Data0, uint32_t Data1) {
    ketCube_RTC_BKUPWrite(Data0, Data1);
}

static inline void HW_RTC_BKUPRead(uint32_t *Data0, uint32_t *Data1) {
    ketCube_RTC_BKUPRead(Data0, Data1);
}

#endif // __HW_RTC_H__
