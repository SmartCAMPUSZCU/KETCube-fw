/**
 * @file    ketCube_mainBoard.h
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2018-12-14
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

#ifndef __KETCUBE_MAIN_BOARD_H
#define __KETCUBE_MAIN_BOARD_H


#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_gpio.h"
#include "stdlib.h"

#include "ketCube_gpio.h"


/** @defgroup  KETCube_mainBoard KETCube Main Board
  * @brief KETCube Configuration Manager
  *
  * This KETCube module incorporates the static in-code and NVM (EEPROM) configuration management
  *
  * @ingroup KETCube_ModuleDrivers
  * @{
  */

/** 
  * @defgroup KETCube_mainBoard_Revisions
  * @brief Main Board revisions
  * @{
  */
#define KETCUBE_MAIN_REV_A      0
#define KETCUBE_MAIN_REV_B      1
#define KETCUBE_MAIN_REV_C      2
#define KETCUBE_MAIN_REV_D      3
#define KETCUBE_MAIN_REV_E      4
#define KETCUBE_MAIN_REV_F      5
 
#define KETCUBE_MAIN_REV_DEV    (KETCUBE_MAIN_REV_F + 1)  ///< The recent "public" board revision is (KETCUBE_MAIN_REV_DEV - 1)
/**
  * @}
  */


/** 
  * @defgroup KETCube_mainBoard_SJOptions
  * @brief Solder Jumper configuration options
  * @{
  */
#define KETCUBE_MAIN_BOARD_SJ_OPEN  0     ///< Solder jumper is OPEN
#define KETCUBE_MAIN_BOARD_SJ_12    1     ///< Solder jumper PADs 1 and 2 are CLOSED (this holds for all solder jumpers)
#define KETCUBE_MAIN_BOARD_SJ_23    2     ///< Solder jumper PADs 2 and 3 are CLOSED (this holds for 3-PAD solder jumpers)
/**
  * @}
  */

/** @defgroup KETCube_mainBoard_Options KETCube options
  * @brief KETCube mainBoard Solder Jumpers and optional parts
  * @{
  */

#define KETCUBE_MAIN_BOARD_REV           (KETCUBE_MAIN_REV_DEV - 1)     ///< The current board revision, @see main.c for deffinition

#define KETCUBE_MAIN_BOARD_OPTION_SJ1     KETCUBE_MAIN_BOARD_SJ_12      ///< Connect PA8 to IO3
#define KETCUBE_MAIN_BOARD_OPTION_SJ7     KETCUBE_MAIN_BOARD_SJ_OPEN    ///< Connect Vref to IO3

#define KETCUBE_MAIN_BOARD_OPTION_SJ2     KETCUBE_MAIN_BOARD_SJ_12      ///< MuRaTa radio powered

#define KETCUBE_MAIN_BOARD_OPTION_SJ3     KETCUBE_MAIN_BOARD_SJ_12      ///< MuRaTa sleep Mode control
#define KETCUBE_MAIN_BOARD_OPTION_SJ8     KETCUBE_MAIN_BOARD_SJ_OPEN    ///< MuRaTa sleep Mode control

#define KETCUBE_MAIN_BOARD_OPTION_SJ5     KETCUBE_MAIN_BOARD_SJ_12      ///< HDC1080 connected
#define KETCUBE_MAIN_BOARD_OPTION_SJ6     KETCUBE_MAIN_BOARD_SJ_12      ///< HDC1080 connected

#define KETCUBE_MAIN_BOARD_OPTION_SJ9     KETCUBE_MAIN_BOARD_SJ_23      ///< 12 == NRST to IO4; 23 == PA5 to IO4
#define KETCUBE_MAIN_BOARD_OPTION_SJ10    KETCUBE_MAIN_BOARD_SJ_12      ///< Vref to 3V3


/**
  * @}
  */

/** @defgroup KETCube_mainBoard_Socket Socket PINs
  * @brief KETCube mainBoard SOCKET PINs
  * @{
  */

// Configurable Socket part

#define KETCUBE_MAIN_BOARD_PIN_IO1_PIN       KETCUBE_GPIO_PIN_10
#define KETCUBE_MAIN_BOARD_PIN_IO1_PORT      KETCUBE_GPIO_PA

#define KETCUBE_MAIN_BOARD_PIN_IO2_PIN       KETCUBE_GPIO_PIN_9
#define KETCUBE_MAIN_BOARD_PIN_IO2_PORT      KETCUBE_GPIO_PA

#if (KETCUBE_MAIN_BOARD_OPTION_SJ1 == KETCUBE_MAIN_BOARD_SJ_12)
#define KETCUBE_MAIN_BOARD_PIN_IO3_PIN     KETCUBE_GPIO_PIN_8
#define KETCUBE_MAIN_BOARD_PIN_IO3_PORT    KETCUBE_GPIO_PA
#else
#define KETCUBE_MAIN_BOARD_PIN_IO3_PIN     KETCUBE_GPIO_PIN_0
#define KETCUBE_MAIN_BOARD_PIN_IO3_PORT    NULL
#endif

#if (KETCUBE_MAIN_BOARD_OPTION_SJ9 == KETCUBE_MAIN_BOARD_SJ_23)
#define KETCUBE_MAIN_BOARD_PIN_IO4_PIN     KETCUBE_GPIO_PIN_5
#define KETCUBE_MAIN_BOARD_PIN_IO4_PORT    KETCUBE_GPIO_PA
#else
#define KETCUBE_MAIN_BOARD_PIN_IO4_PIN     KETCUBE_GPIO_PIN_0
#define KETCUBE_MAIN_BOARD_PIN_IO4_PORT    NULL
#endif

// MicroBUS Socket part

#define KETCUBE_MAIN_BOARD_PIN_AN_PIN        KETCUBE_GPIO_PIN_4
#define KETCUBE_MAIN_BOARD_PIN_AN_PORT       KETCUBE_GPIO_PA

#define KETCUBE_MAIN_BOARD_PIN_RST_PIN       KETCUBE_GPIO_PIN_0
#define KETCUBE_MAIN_BOARD_PIN_RST_PORT      KETCUBE_GPIO_PA

#define KETCUBE_MAIN_BOARD_PIN_CS_PIN        KETCUBE_GPIO_PIN_12
#define KETCUBE_MAIN_BOARD_PIN_CS_PORT       KETCUBE_GPIO_PB

#define KETCUBE_MAIN_BOARD_PIN_SCK_PIN       KETCUBE_GPIO_PIN_13
#define KETCUBE_MAIN_BOARD_PIN_SCK_PORT      KETCUBE_GPIO_PB

#define KETCUBE_MAIN_BOARD_PIN_MISO_PIN      KETCUBE_GPIO_PIN_14
#define KETCUBE_MAIN_BOARD_PIN_MISO_PORT     KETCUBE_GPIO_PB

#define KETCUBE_MAIN_BOARD_PIN_MOSI_PIN      KETCUBE_GPIO_PIN_15
#define KETCUBE_MAIN_BOARD_PIN_MOSI_PORT     KETCUBE_GPIO_PB

#define KETCUBE_MAIN_BOARD_PIN_PWM_PIN       KETCUBE_GPIO_PIN_2
#define KETCUBE_MAIN_BOARD_PIN_PWM_PORT      KETCUBE_GPIO_PB

#if (KETCUBE_MAIN_BOARD_REV < KETCUBE_MAIN_BOARD_REV_E)
#define KETCUBE_MAIN_BOARD_PIN_INT_PIN     KETCUBE_GPIO_PIN_7
#define KETCUBE_MAIN_BOARD_PIN_INT_PORT    KETCUBE_GPIO_PB
#else
#define KETCUBE_MAIN_BOARD_PIN_INT_PIN     KETCUBE_GPIO_PIN_5
#define KETCUBE_MAIN_BOARD_PIN_INT_PORT    KETCUBE_GPIO_PB
#endif

#define KETCUBE_MAIN_BOARD_PIN_RX_PIN        KETCUBE_GPIO_PIN_3
#define KETCUBE_MAIN_BOARD_PIN_RX_PORT       KETCUBE_GPIO_PA

#define KETCUBE_MAIN_BOARD_PIN_TX_PIN        KETCUBE_GPIO_PIN_2
#define KETCUBE_MAIN_BOARD_PIN_TX_PORT       KETCUBE_GPIO_PA

#define KETCUBE_MAIN_BOARD_PIN_SCL_PIN       KETCUBE_GPIO_PIN_8
#define KETCUBE_MAIN_BOARD_PIN_SCL_PORT      KETCUBE_GPIO_PB

#define KETCUBE_MAIN_BOARD_PIN_SDA_PIN       KETCUBE_GPIO_PIN_9
#define KETCUBE_MAIN_BOARD_PIN_SDA_PORT      KETCUBE_GPIO_PB

/**
  * @}
  */


/** @defgroup KETCube_mainBoard_LEDs Debug LEDs
  * @brief KETCube mainBoard Debug LEDs
  * @{
  */

#if (KETCUBE_MAIN_BOARD_REV < KETCUBE_MAIN_BOARD_REV_E)
#define KETCUBE_MAIN_BOARD_LED1_PIN                           KETCUBE_GPIO_PIN_5
#define KETCUBE_MAIN_BOARD_LED1_GPIO_PORT                     KETCUBE_GPIO_PB
#else
#define KETCUBE_MAIN_BOARD_LED1_PIN                           KETCUBE_GPIO_PIN_7
#define KETCUBE_MAIN_BOARD_LED1_GPIO_PORT                     KETCUBE_GPIO_PB
#endif

#define KETCUBE_MAIN_BOARD_LED2_PIN                             KETCUBE_GPIO_PIN_6
#define KETCUBE_MAIN_BOARD_LED2_GPIO_PORT                       KETCUBE_GPIO_PB

/**
  * @}
  */


/**
  * @}
  */

#endif                          /* __KETCUBE_MAIN_BOARD_H */
