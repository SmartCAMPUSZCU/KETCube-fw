/**
 * @file    ketCube_buzzer.c
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2019-05-20
 * @brief   This file contains definitions for the KETCube Buzzer driver
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
#include "stm32l0xx_hal_lptim.h"

#include "hw.h"
#include "ketCube_gpio.h"
#include "timeServer.h"
#include "ketCube_common.h"
#include "ketCube_timer.h"
#include "ketCube_buzzer.h"
#include "ketCube_terminal.h"

#ifdef KETCUBE_CFG_INC_DRV_BUZZER

LPTIM_HandleTypeDef hlptim1;

// sound queue
ketCube_buzzer_beep_t queue[KETCUBE_BUZZER_SOUND_QUEUE];
uint8_t head = 0;
uint8_t tail = 0;

static TimerEvent_t beepTimer;

static volatile bool driverInitialized = FALSE;

/**
 * @brief  Init LPTIM
 * 
 * @retval TRUE in case of success
 * @retval FALSE in case of error
 * 
 */
static bool ketCube_buzzer_initLPTIM()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // Init timer
    hlptim1.Instance = LPTIM1;
    hlptim1.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
    hlptim1.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV1;
    hlptim1.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
    hlptim1.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
    hlptim1.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
    hlptim1.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
    if (HAL_LPTIM_Init(&hlptim1) != HAL_OK)
    {
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_BUZZER_NAME, KETCUBE_CFG_SEVERITY_ERROR, "Timer init error!");
        return FALSE;
    }
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();
     /* Peripheral clock enable */
    __HAL_RCC_LPTIM1_CLK_ENABLE();
  
    /**LPTIM1 GPIO Configuration    
    PB2     ------> LPTIM1_OUT 
    */
    GPIO_InitStruct.Pin = KETCUBE_GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_LPTIM1;
    ketCube_GPIO_Init(KETCUBE_GPIO_PB, KETCUBE_GPIO_PIN_2, &GPIO_InitStruct);
    
    return TRUE;
}

/**
 * @brief  Stop LPTIM
 * 
 */
static void ketCube_buzzer_stopLPTIM()
{
    __HAL_RCC_LPTIM1_CLK_DISABLE();
    ketCube_GPIO_Release(KETCUBE_GPIO_PB, KETCUBE_GPIO_PIN_2);
}

/**
 * @brief Init sound generation
 * 
 */
void startNewBeep() {
    // try to init timer
    if (ketCube_buzzer_initLPTIM() == FALSE) {
        return;
    }
    
    ketCube_terminal_DriverSeverityPrintln(KETCUBE_BUZZER_NAME, KETCUBE_CFG_SEVERITY_DEBUG, "head = %d, tail = %d", head, tail);
    ketCube_terminal_DriverSeverityPrintln(KETCUBE_BUZZER_NAME, KETCUBE_CFG_SEVERITY_INFO, "Beep (freq: %d Hz; dur: %d ms; timer: %d) START", queue[tail].frequency, queue[tail].duration, (uint16_t) (queue[tail].frequency * KETCUBE_BUZZER_TICKS_PER_HZ));
    
    queue[tail].active = TRUE;
    
    HAL_LPTIM_PWM_Start(&hlptim1, 
                        ((uint16_t) (queue[tail].frequency * KETCUBE_BUZZER_TICKS_PER_HZ)),  
                        (((uint16_t) (queue[tail].frequency * KETCUBE_BUZZER_TICKS_PER_HZ)) >> 1));
    
    TimerStop(&beepTimer);
    TimerSetValue(&beepTimer, queue[tail].duration);
    TimerStart(&beepTimer);
}

/**
 * @brief  Beep callback for queue events
 * 
 */
void beepCallback(void* context) {
    if (queue[tail].active == TRUE) {
        // stop sound generation
        HAL_LPTIM_PWM_Stop(&hlptim1);
        
        queue[tail].active = FALSE;
        
        if (queue[tail].mute != 0) {
            // start mute period
            TimerStop(&beepTimer);
            TimerSetValue(&beepTimer, queue[tail].mute);
            TimerStart(&beepTimer);
            return;
        }
    }
    
    tail = (tail + 1) % KETCUBE_BUZZER_SOUND_QUEUE;
    
    // end of mute period
    if (head == tail) {
        // finished!
        ketCube_buzzer_stopLPTIM();
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_BUZZER_NAME, KETCUBE_CFG_SEVERITY_DEBUG, "head = %d, tail = %d", head, tail);
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_BUZZER_NAME, KETCUBE_CFG_SEVERITY_INFO, "Beep STOP");
        return;
    } else {
        //start new sound
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_BUZZER_NAME, KETCUBE_CFG_SEVERITY_DEBUG, "Processing next sound in queue head = %d, tail = %d", head, tail);
        startNewBeep();
    }
}

/**
 * @brief  Configures buzzer
 * 
 * @retval KETCUBE_CFG_DRV_OK in case of success
 * @retval KETCUBE_CFG_DRV_ERROR in case of failure
 */
ketCube_cfg_DrvError_t ketCube_buzzer_Init()
{
    uint8_t i;
    
    if (driverInitialized == TRUE) {
        return KETCUBE_CFG_MODULE_OK;
    }
        
    // Request Timer
    if (ketCube_Timer_Init(KETCUBE_BUZZER_TIMER) == KETCUBE_CFG_DRV_OK) {
        //timer not available ...
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_BUZZER_NAME, KETCUBE_CFG_SEVERITY_ERROR, "Timer get error!");
        
        return KETCUBE_CFG_DRV_ERROR;
    }
    
    for (i = 0; i < KETCUBE_BUZZER_SOUND_QUEUE; i++) {
        queue[i].active = FALSE;
    }
    
    head = 0;
    tail = 0;
    
    TimerInit(&beepTimer, beepCallback);
    
    driverInitialized = TRUE;
    
    return KETCUBE_CFG_DRV_OK;
}

/**
 * @brief  Deinitialize buzzer driver
 * 
 * @todo add timer deInit
 * 
 * @retval KETCUBE_CFG_DRV_OK in case of success
 * @retval KETCUBE_CFG_DRV_ERROR in case of failure
 */
ketCube_cfg_DrvError_t ketCube_buzzer_UnInit() {
    
    if (driverInitialized == FALSE) {
        return KETCUBE_CFG_DRV_OK;
    }
    
    
    driverInitialized = FALSE;
    
    return KETCUBE_CFG_DRV_OK;
}

/**
 * @brief  Register new beep or start beep if queue is empty
 * 
 * @param freq frequency in Hz
 * @param dur beep duration in ms
 * @param mute mute duration in ms
 * 
 */
void ketCube_buzzer_Beep(uint16_t freq, uint16_t dur, uint16_t mute)
{
    if (driverInitialized == FALSE) {
        return;
    }
    
    /* if queue is full, exit! */
    if ((head == tail) && (queue[tail].active == TRUE)) {
        return;
    }
    
    queue[head].frequency = freq;
    queue[head].duration = dur;
    queue[head].mute = mute;
    queue[head].active = FALSE;
    
    head = (head + 1) % KETCUBE_BUZZER_SOUND_QUEUE;
    
    // start beep if no beep in progress
    // TODO race condition is possible - but this is not critical (?)
    if (((tail + 1) % KETCUBE_BUZZER_SOUND_QUEUE) != head) {
        // do not start
        return;
    }
    startNewBeep();
}

/**
 * @brief  Check if sound generation is in progress
 * 
 * @retval TRUE if buzzer is in proggress, else return FALSE
 * 
 */
bool ketCube_buzzer_inProgress() {
    if ((head == tail) && (queue[tail].active != TRUE)) {
        return FALSE;
    }
    
    return TRUE;
}


#endif                          /* KETCUBE_CFG_INC_DRV_BUZZER */
