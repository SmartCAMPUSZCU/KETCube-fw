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
#include "vcom.h"
#include "timeServer.h"

#include "ketCube_mainBoard.h"
#include "ketCube_coreCfg.h"
#include "ketCube_terminal.h"
#include "ketCube_modules.h"
#include "ketCube_common.h"
#include "ketCube_resetMan.h"
#include "ketCube_remote_terminal.h"
#include "ketCube_mcu.h"
#include "ketCube_rtc.h"

static TimerEvent_t KETCube_PeriodTimer;


volatile static bool KETCube_PeriodTimerElapsed = FALSE;
volatile static bool KETCube_Initialized = FALSE;
volatile bool KETCube_eventsProcessed = TRUE;  /* If TRUE, then there are no pending events */

/*!
 * @brief Function executed on TxNextPacket Timeout event
 */
void KETCube_PeriodElapsed(void* context)
{
    TimerStop(&KETCube_PeriodTimer);

    KETCube_PeriodTimerElapsed = TRUE;

    TimerSetValue(&KETCube_PeriodTimer, ketCube_coreCfg.basePeriod);

    TimerStart(&KETCube_PeriodTimer);
}

/*!
 * @brief Repeat base period if error ocured during last basePeriod
 * 
 */
void KETCube_PeriodRepeatIfNeeded(void)
{
    if (ketCube_coreCfg.repeatDelay == 0) {
        return;
    }
    
    if (ketCube_coreCfg.repeatDelay > ketCube_coreCfg.basePeriod) {
        return;
    }
    
    if ((ketCube_coreCfg.volatileData.moduleSendErrorCnt == 0) &&
        (ketCube_coreCfg.volatileData.modulePerErrorCnt == 0)) {
        return;        
    }
        
    ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO, "Module error detected - basePeriod repeat planed after %d ms", ketCube_coreCfg.repeatDelay);
        
    TimerStop(&KETCube_PeriodTimer);

    TimerSetValue(&KETCube_PeriodTimer, ketCube_coreCfg.repeatDelay);

    TimerStart(&KETCube_PeriodTimer);
}

void KETCube_ErrorHandler(void)
{
    KETCUBE_TERMINAL_ENDL();

    KETCUBE_TERMINAL_PRINTF("!!! KETCube ERROR !!!");

    KETCUBE_TERMINAL_ENDL();
    
    // In case of clock-related error, delay may cause problems!
    // HAL_Delay(10000);

//     while (TRUE) {
// 
//     }
    
    ketCube_resetMan_requestReset(KETCUBE_RESETMAN_REASON_ERROR_HANDLER);
}

/**
 * @brief Process custom data 
 * 
 * @param buffer received data
 * 
 * @note This overwrites the weak function defined in ketCube_lora.c
 */
void ketCube_lora_processCustomData(uint8_t * buffer, uint8_t len)
{
    if (len < 1) {
        return;
    }
    // decode commands
    switch (buffer[0]) {
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
    
    /* This variable indicates if pending event is possible */
    KETCube_eventsProcessed = TRUE;
    
    /* STM32 HAL library initialization */
    HAL_Init();

    /* Configure the system clock */
    ketCube_MCU_ClockConfig();
    
    /* Configure the debug mode */
    DBG_Init();

#if defined(USE_BOOTLOADER)
    /* Set the Vector Table base location at 0x3000 */
    NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x3000 );
#endif
    
    /* Configure the hardware */
    ketCube_RTC_Init();
    
    /* Initialize KETCube mainBoard */
    ketCube_mainBoard_Init();
  
    /* Initialize drivers (if needed to do independently on modules) */
    ketCube_Radio_InitDriver();
    ketCube_GPIO_InitDriver();

    /* Init Terminal */
    ketCube_terminal_Init();

    // A hot fix for non-operational RTC after POR - this should be removed in the future
    if (ketCube_coreCfg.volatileData.resetInfo.reason == KETCUBE_RESETMAN_REASON_POR) {
        //perform SW reset
        ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO,
                                             "POR detected - reseting!");

        ketCube_resetMan_requestReset(KETCUBE_RESETMAN_REASON_PORSW);
    }
    ketCube_resetMan_info();
    
    /* Init KETCube modules */
    if (ketCube_modules_Init() != KETCUBE_CFG_OK) {
        KETCube_ErrorHandler();
    }
    
    // Initialize periodic timer
    TimerInit(&KETCube_PeriodTimer, KETCube_PeriodElapsed);
    
    /* Init Watchdog */
    ketCube_MCU_WD_Init();
    
    // KETCube is initialized
    KETCube_Initialized = TRUE;

    /* Configure the periodic timer */
#if (KETCUBE_CORECFG_SKIP_SLEEP_PERIOD != TRUE)
    TimerSetValue(&KETCube_PeriodTimer, ketCube_coreCfg.startDelay);

    TimerStart(&KETCube_PeriodTimer);
#endif                          /*  */
    
    /* main loop */
    while (TRUE) {
        /* process pendig commands */
        ketCube_terminal_ProcessCMD();
        
        /* process pending remote terminal commands */
        ketCube_remoteTerminal_ProcessCMD();

        /* execute periodic function for enabled modules */
#if (KETCUBE_CORECFG_SKIP_SLEEP_PERIOD != TRUE)
        if (KETCube_PeriodTimerElapsed == TRUE) {
#endif

            KETCube_PeriodTimerElapsed = FALSE;

            ketCube_terminal_CoreSeverityPrintln
                (KETCUBE_CFG_SEVERITY_DEBUG,
                 "--- KETCube base period # %d ---", basePeriodCnt++);

            ketCube_modules_ExecutePeriodic();
            
            /* check if we should repeat "last" basePeriod */
            KETCube_PeriodRepeatIfNeeded();

#if (KETCUBE_CORECFG_SKIP_SLEEP_PERIOD != TRUE)
        }
#endif

        // process inter/module messages...
        ketCube_modules_ProcessMsgs();

        // execute module preSleep module functions
        if ((ketCube_modules_SleepEnter() == KETCUBE_CFG_OK) && (KETCube_eventsProcessed == TRUE)) {
#if (KETCUBE_CORECFG_SKIP_SLEEP_PERIOD != TRUE)
            ketCube_MCU_Sleep();
#endif                          /* (KETCUBE_CORECFG_SKIP_SLEEP_PERIOD != TRUE) */
        }
        
        /* If IRQ occurs after this flag setting, sleep is skipped once until no IRQ is generated in run-time */
        KETCube_eventsProcessed = TRUE;
        
        /* execute RTC alarms */
        ketCube_RTC_AlarmAEventExec();
        
        /* reset WD  !!! Must be after RTC Timer !!! */
        ketCube_MCU_WD_Reset();
        
        // execute module wake-up functions
        ketCube_modules_SleepExit();
    }
}

