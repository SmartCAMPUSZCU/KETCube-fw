/**
 * @file    main.c
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-03-03
 * @brief   This file is the KETCube MAIN
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

#include "hw.h"
#include "low_power.h"
#include "bsp.h"
#include "vcom.h"
#include "timeServer.h"

#include "ketCube_mainBoard.h"
#include "ketCube_coreCfg.h"
#include "ketCube_terminal.h"
#include "ketCube_modules.h"
#include "ketCube_common.h"

static TimerEvent_t KETCube_PeriodTimer;


static bool KETCube_PeriodTimerElapsed = FALSE;


bool KETCube_wasResetPOR;
void KETCube_getResetFlags(void) {  
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) == TRUE) {
        KETCube_wasResetPOR = TRUE;
    } else {
        KETCube_wasResetPOR = FALSE;
    }
    __HAL_RCC_CLEAR_RESET_FLAGS();
}

/*!
 * @brief Function executed on TxNextPacket Timeout event
 */
static void KETCube_PeriodElapsed(void)
{
    TimerStop(&KETCube_PeriodTimer);

    KETCube_PeriodTimerElapsed = TRUE;

    TimerSetValue(&KETCube_PeriodTimer, ketCube_coreCfg_BasePeriod);

    TimerStart(&KETCube_PeriodTimer);
}

void KETCube_ErrorHandler(void)
{
    KETCUBE_TERMINAL_ENDL();

    KETCUBE_TERMINAL_PRINTF("!!! KETCube ERROR !!!");

    KETCUBE_TERMINAL_ENDL();

    while (TRUE) {

    }
}

/**
 * @brief Process custom data 
 * 
 * @param buffer received data
 * 
 * @note This overwrites the weak function defined in ketCube_lora.c
 */
void ketCube_lora_processCustomData(uint8_t * buffer, uint8_t len) {
    if (len < 1) {
        return;
    }
    
    // decode commands
    switch(buffer[0]) {
        case 0x01:
            // do something ...
            break;
        case 0x02:
            // do something else ...
            break;
        default:
            // command not found
            return;
    }
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    uint32_t basePeriodCnt = 0;
    
    /* STM32 HAL library initialization */
    HAL_Init();
            
    /* Configure the system clock */
    SystemClock_Config();
    
    /* Configure the debug mode */
    DBG_Init();
    
    /* Configure the hardware */
    HW_Init();
    
    /* Init Terminal */
    ketCube_terminal_Init();

    // A hot fix for non-operational RTC after POR - this should be removed in the future
    if (KETCube_wasResetPOR == TRUE) {
        //perform SW reset
        ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO, "POR detected - reseting!");
        
        NVIC_SystemReset();
    }
    
    /* Init KETCube core config */
    if (ketCube_coreCfg_Init() != KETCUBE_CFG_OK) {
        KETCube_ErrorHandler();
    }

    /* Init KETCube modules */
    if (ketCube_modules_Init() != KETCUBE_CFG_OK) {
        KETCube_ErrorHandler();
    }

    /* Configure the periodic timer */
#if (KETCUBE_CORECFG_SKIP_SLEEP_PERIOD != TRUE)
    TimerInit(&KETCube_PeriodTimer, KETCube_PeriodElapsed);

    TimerSetValue(&KETCube_PeriodTimer, ketCube_coreCfg_StartDelay);

    TimerStart(&KETCube_PeriodTimer);
#endif                          /*  */

    /* main loop */
    while (TRUE)
    {
        /* process pendig commands */
        ketCube_terminal_ProcessCMD();

        /* execute periodic function for enabled modules */
#if (KETCUBE_CORECFG_SKIP_SLEEP_PERIOD != TRUE)
        if (KETCube_PeriodTimerElapsed == TRUE) {
#endif

            KETCube_PeriodTimerElapsed = FALSE;

            ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_DEBUG, "--- KETCube base period # %d ---", basePeriodCnt++);

            ketCube_modules_ExecutePeriodic();

#if (KETCUBE_CORECFG_SKIP_SLEEP_PERIOD != TRUE)
        }
#endif

        // process inter/module messages...
        ketCube_modules_ProcessMsgs();

        // execute module preSleep module functions
        if (ketCube_modules_SleepEnter() == KETCUBE_CFG_OK) {

#if (KETCUBE_CORECFG_SKIP_SLEEP_PERIOD != TRUE)
            DISABLE_IRQ();

#ifndef LOW_POWER_DISABLE
            LowPower_Handler();

#endif                          /* LOW_POWER_DISABLE */
            ENABLE_IRQ();
#endif                          /* (KETCUBE_CORECFG_SKIP_SLEEP_PERIOD != TRUE) */

            // execute module wake-up module functions
            ketCube_modules_SleepExit();
        }
    }
}
