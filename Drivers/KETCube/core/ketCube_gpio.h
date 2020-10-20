/**
 * @file    ketCube_gpio.h
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2018-10-30
 * @brief   This file contains definitions for the ketCube GPIO driver
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
#ifndef __KETCUBE_GPIO_H
#define __KETCUBE_GPIO_H

#include "stm32l0xx_hal_gpio.h"
#include "ketCube_cfg.h"

/** @defgroup KETCube_GPIO KETCube GPIO driver
  * @brief KETCube GPIO driver
  * @ingroup KETCube_ModuleDrivers
  * @{
  */

#define KETCUBE_GPIO_NAME               "gpio_drv"         ///< GPIO driver name

#define KETCUBE_GPIO_LED_PERIOD         500                ///< LED driver blink period in ms

/**
* @brief List of GPIO PINs
*/
typedef enum {
    KETCUBE_GPIO_NOPIN = 0,
    KETCUBE_GPIO_PIN_0 = GPIO_PIN_0,
    KETCUBE_GPIO_PIN_1 = GPIO_PIN_1,
    KETCUBE_GPIO_PIN_2 = GPIO_PIN_2,
    KETCUBE_GPIO_PIN_3 = GPIO_PIN_3,
    KETCUBE_GPIO_PIN_4 = GPIO_PIN_4,
    KETCUBE_GPIO_PIN_5 = GPIO_PIN_5,
    KETCUBE_GPIO_PIN_6 = GPIO_PIN_6,
    KETCUBE_GPIO_PIN_7 = GPIO_PIN_7,
    KETCUBE_GPIO_PIN_8 = GPIO_PIN_8,
    KETCUBE_GPIO_PIN_9 = GPIO_PIN_9,
    KETCUBE_GPIO_PIN_10 = GPIO_PIN_10,
    KETCUBE_GPIO_PIN_11 = GPIO_PIN_11,
    KETCUBE_GPIO_PIN_12 = GPIO_PIN_12,
    KETCUBE_GPIO_PIN_13 = GPIO_PIN_13,
    KETCUBE_GPIO_PIN_14 = GPIO_PIN_14,
    KETCUBE_GPIO_PIN_15 = GPIO_PIN_15
} ketCube_gpio_pin_t;

/**
* @brief GPIO Ports
*/
typedef enum {
    KETCUBE_GPIO_PA = GPIOA_BASE,
    KETCUBE_GPIO_PB = GPIOB_BASE,
    KETCUBE_GPIO_PC = GPIOC_BASE,
    KETCUBE_GPIO_PD = GPIOD_BASE,
    KETCUBE_GPIO_PE = GPIOE_BASE,
    KETCUBE_GPIO_PH = GPIOH_BASE
} ketCube_gpio_port_t;

/**
* @brief LED States
*/
typedef enum {
    KETCUBE_GPIO_FUNCTION_GPIO = 0,    /*!< Function is defined by user */  
    KETCUBE_GPIO_FUNCTION_LED,         /*!< LED function is handled by driver */  
    KETCUBE_GPIO_FUNCTION_BUTTON,      /*!< BUTTON function is handled by driver TODO: not implemented yet */  
} ketCube_gpio_function_t;

/**
* @brief GPIO polarity
*/
typedef enum {
    KETCUBE_GPIO_POLARITY_ON  = TRUE,   /*!< ON polarity (e.g. for LEDs) */  
    KETCUBE_GPIO_POLARITY_OFF = FALSE,  /*!< OFF polarity (e.g. for LEDs) */
} ketCube_gpio_polarity_t;

/**
* @brief LED States
*/
typedef enum {
    KETCUBE_GPIO_LED_OFF = 0,        /*!< LED OFF */  
    KETCUBE_GPIO_LED_ON,             /*!< LED ON */  
    KETCUBE_GPIO_LED_BLINK_SINGLE,   /*!< LED BLINK - single */  
    KETCUBE_GPIO_LED_BLINK_CONT      /*!< LED BLINK - continuous */  
} ketCube_gpio_LEDState_t;


/**
* @brief GPIO PIN descriptor
*/
#ifdef __ARMCC_VERSION
#pragma anon_unions
#endif
typedef struct {
    bool used:1;   /*!< PIN used */
    ketCube_gpio_function_t function:2;  /*!< PIN function */
    
    /* settings */
    union {
        uint8_t value:4;  /*!< value access */
        struct {
            ketCube_gpio_LEDState_t LED_state:3; /*!< LED state if function is KETCUBE_GPIO_FUNCTION_LED */
            ketCube_gpio_polarity_t polarity:1;  /*!< LED ON polarity */
        };
    } settings;
} ketCube_gpio_descriptor_t;


/**
* @brief Pointer to a IRQ Handler Function
*/
typedef void (*ketCube_GPIO_VoidFn_t) (void* context);

/** @defgroup KETCube_GPIO_fn Public Functions
  * @brief Public functions
  * @{
  */

/* Init functions called by KETCube core */
extern void ketCube_GPIO_InitDriver(void);
extern ketCube_cfg_DrvError_t ketCube_GPIO_SleepEnter(void);


/* Init functions called by modules */
extern ketCube_cfg_DrvError_t ketCube_GPIO_Init(ketCube_gpio_port_t port,
                                                uint16_t pin,
                                                GPIO_InitTypeDef *
                                                initStruct);
extern ketCube_cfg_DrvError_t ketCube_GPIO_InitLED(ketCube_gpio_port_t port,
                                                   uint16_t pin,
                                                   ketCube_gpio_polarity_t polarity);
extern ketCube_cfg_DrvError_t ketCube_GPIO_ReInit(ketCube_gpio_port_t port,
                                                  uint16_t pin,
                                                  GPIO_InitTypeDef * initStruct);
extern ketCube_cfg_DrvError_t ketCube_GPIO_Release(ketCube_gpio_port_t port,
                                                   ketCube_gpio_pin_t pin);


/* Interrupt-related functions */
extern ketCube_cfg_DrvError_t ketCube_GPIO_SetIrq(ketCube_gpio_port_t port,
                                                  ketCube_gpio_pin_t pin,
                                                  uint32_t prio,
                                                  ketCube_GPIO_VoidFn_t
                                                  irqHandler);
extern ketCube_cfg_DrvError_t ketCube_GPIO_ClearIrq(ketCube_gpio_port_t port,
                                                    ketCube_gpio_pin_t pin);

/* Read/Write functions */
extern void ketCube_GPIO_Write(ketCube_gpio_port_t port,
                               ketCube_gpio_pin_t pin, bool bit);
extern bool ketCube_GPIO_Read(ketCube_gpio_port_t port,
                              ketCube_gpio_pin_t pin);
extern void ketCube_GPIO_SetLED(ketCube_gpio_port_t port,
                                ketCube_gpio_pin_t pin,
                                ketCube_gpio_LEDState_t state);

/* Handlers */
extern void EXTI0_1_IRQHandler(void);
extern void EXTI2_3_IRQHandler(void);
extern void EXTI4_15_IRQHandler(void);


/**
* @}
*/

/**
* @}
*/

#endif
