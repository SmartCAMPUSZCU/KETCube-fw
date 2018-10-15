/**
 * @file    ketCube_mainBoard.h
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-01-04
 * @brief   Definitions for KETCube main board
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
#ifndef __KETCUBE_MAIN_BOARD_H
#define __KETCUBE_MAIN_BOARD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_gpio.h"
#include "stdlib.h"


/** @defgroup  KETCube_mainBoard KETCube Main Board
  * @brief KETCube Configuration Manager
  *
  * This KETCube module incorporates the static in-code and NVM (EEPROM) configuration management
  *
  * @ingroup KETCube_ModuleDrivers
  * @{
  */

#define KETCUBE_MAIN_BOARD_LED1_PIN                           GPIO_PIN_7
#define KETCUBE_MAIN_BOARD_LED1_GPIO_PORT                     GPIOB
#define KETCUBE_MAIN_BOARD_LED1_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE()
#define KETCUBE_MAIN_BOARD_LED1_GPIO_CLK_DISABLE()            __HAL_RCC_GPIOB_CLK_DISABLE()
#define KETCUBE_MAIN_BOARD_LED1_On()                          HAL_GPIO_WritePin(KETCUBE_MAIN_BOARD_LED1_GPIO_PORT, KETCUBE_MAIN_BOARD_LED1_PIN, GPIO_PIN_SET)
#define KETCUBE_MAIN_BOARD_LED1_Off()                         HAL_GPIO_WritePin(KETCUBE_MAIN_BOARD_LED1_GPIO_PORT, KETCUBE_MAIN_BOARD_LED1_PIN, GPIO_PIN_RESET)
#define KETCUBE_MAIN_BOARD_LED1_Toggle()                      HAL_GPIO_TogglePin(KETCUBE_MAIN_BOARD_LED1_GPIO_PORT, KETCUBE_MAIN_BOARD_LED1_PIN)

#define KETCUBE_MAIN_BOARD_LED2_PIN                           GPIO_PIN_6
#define KETCUBE_MAIN_BOARD_LED2_GPIO_PORT                     GPIOB
#define KETCUBE_MAIN_BOARD_LED2_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE()
#define KETCUBE_MAIN_BOARD_LED2_GPIO_CLK_DISABLE()            __HAL_RCC_GPIOB_CLK_DISABLE()
#define KETCUBE_MAIN_BOARD_LED2_On()                          HAL_GPIO_WritePin(KETCUBE_MAIN_BOARD_LED2_GPIO_PORT, KETCUBE_MAIN_BOARD_LED2_PIN, GPIO_PIN_SET)
#define KETCUBE_MAIN_BOARD_LED2_Off()                         HAL_GPIO_WritePin(KETCUBE_MAIN_BOARD_LED2_GPIO_PORT, KETCUBE_MAIN_BOARD_LED2_PIN, GPIO_PIN_RESET)
#define KETCUBE_MAIN_BOARD_LED2_Toggle()                      HAL_GPIO_TogglePin(KETCUBE_MAIN_BOARD_LED2_GPIO_PORT, KETCUBE_MAIN_BOARD_LED2_PIN)


static inline void ketCube_MainBoard_LED1_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* Enable the GPIO_LED Clock */
    KETCUBE_MAIN_BOARD_LED1_GPIO_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    GPIO_InitStruct.Pin = KETCUBE_MAIN_BOARD_LED1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    HAL_GPIO_Init(KETCUBE_MAIN_BOARD_LED1_GPIO_PORT, &GPIO_InitStruct);

    KETCUBE_MAIN_BOARD_LED1_Off();
}

static inline void ketCube_MainBoard_LED2_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* Enable the GPIO_LED Clock */
    KETCUBE_MAIN_BOARD_LED2_GPIO_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    GPIO_InitStruct.Pin = KETCUBE_MAIN_BOARD_LED2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    HAL_GPIO_Init(KETCUBE_MAIN_BOARD_LED2_GPIO_PORT, &GPIO_InitStruct);

    KETCUBE_MAIN_BOARD_LED2_Off();
}

/**
  * @}
  */

#endif                          /* __KETCUBE_MAIN_BOARD_H */
