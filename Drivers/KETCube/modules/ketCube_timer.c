/**
 * @file    ketCube_timer.c
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2018-07-12
 * @brief   This file contains the ketCube Timer(s) driver
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
#include "stm32l0xx_hal.h"

#include "ketCube_cfg.h"
#include "ketCube_timer.h"

ketCube_Timer_usage_t timerUsed = { 0, 0, 0, 0, 0, 0, 0 };

TIM_HandleTypeDef KETCube_Timer_Htim2;
static volatile bool KETCube_Timer_Timer2_IC = FALSE;

/**
 * @brief  Configures Timer(s)
 *
 * @param tim requested timer
 * 
 * @retval KETCUBE_CFG_DRV_OK in case of success
 * @retval KETCUBE_CFG_DRV_ERROR in case of failure -- requested timer already used; resource not available
 */
ketCube_cfg_DrvError_t ketCube_Timer_Init(ketCube_Timer_list_t tim)
{
    switch (tim) {
    case KETCUBE_TIMER_LIST_TIM2:
        if (timerUsed.tim2 == TRUE) {
            return KETCUBE_CFG_DRV_ERROR;
        }
        break;
    case KETCUBE_TIMER_LIST_TIM3:
        if (timerUsed.tim2 == TRUE) {
            return KETCUBE_CFG_DRV_ERROR;
        }
        break;
    case KETCUBE_TIMER_LIST_TIM21:
        if (timerUsed.tim21 == TRUE) {
            return KETCUBE_CFG_DRV_ERROR;
        }
        break;
    case KETCUBE_TIMER_LIST_TIM22:
        if (timerUsed.tim22 == TRUE) {
            return KETCUBE_CFG_DRV_ERROR;
        }
        break;
    case KETCUBE_TIMER_LIST_TIM6:
        if (timerUsed.tim6 == TRUE) {
            return KETCUBE_CFG_DRV_ERROR;
        }
        break;
    case KETCUBE_TIMER_LIST_TIM7:
        if (timerUsed.tim7 == TRUE) {
            return KETCUBE_CFG_DRV_ERROR;
        }
        break;
    case KETCUBE_TIMER_LIST_LPTIM:
        if (timerUsed.lptim == TRUE) {
            return KETCUBE_CFG_DRV_ERROR;
        }
        break;
    default:
        return KETCUBE_CFG_DRV_ERROR;
    }

    return KETCUBE_CFG_DRV_OK;
}

/**
  * @brief  Mark IC event as processed @ Timer 2
  * 
  */
void ketCube_Timer_Timer2_ResetICEvent(void) {
    KETCube_Timer_Timer2_IC = FALSE;
}


/**
  * @brief  Return IC Event state @ Timer 2
  * 
  */
bool ketCube_Timer_Timer2_IsICEvent(void) {
    return KETCube_Timer_Timer2_IC;
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&KETCube_Timer_Htim2);
}

/**
  * @brief  Input Capture callback in non-blocking mode
  * 
  * @brief Only Timer2 is currently supported
  * 
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    // Timer checking
    if (htim->Instance != TIM2) {
        return;
    }
    
    KETCube_eventsProcessed = FALSE; /* Possible pending events */
    
    KETCube_Timer_Timer2_IC = TRUE;
}
