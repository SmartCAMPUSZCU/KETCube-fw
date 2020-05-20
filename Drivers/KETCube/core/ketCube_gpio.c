/**
 * @file    ketCube_gpio.c
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2018-10-30
 * @brief   This file contains the ketCube GPIO driver
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

#include "ketCube_cfg.h"
#include "ketCube_gpio.h"
#include "ketCube_terminal.h"

static ketCube_gpio_pin_t pinUsage[3] = {KETCUBE_GPIO_NOPIN, KETCUBE_GPIO_NOPIN, KETCUBE_GPIO_NOPIN,};  ///< PIN usage for ports A, B and C

#define RCC_GPIO_CLK_ENABLE( __GPIO_PORT__ )              \
do {                                                    \
    switch( __GPIO_PORT__)                                \
    {                                                     \
      case GPIOA_BASE: __HAL_RCC_GPIOA_CLK_ENABLE(); break;    \
      case GPIOB_BASE: __HAL_RCC_GPIOB_CLK_ENABLE(); break;    \
      case GPIOC_BASE: __HAL_RCC_GPIOC_CLK_ENABLE(); break;    \
      case GPIOD_BASE: __HAL_RCC_GPIOD_CLK_ENABLE(); break;    \
      case GPIOH_BASE: default:  __HAL_RCC_GPIOH_CLK_ENABLE(); \
    }                                                    \
  } while(0)

#define RCC_GPIO_CLK_DISABLE( __GPIO_PORT__ )              \
do {                                                    \
    switch( __GPIO_PORT__)                                \
    {                                                     \
      case GPIOA_BASE: __HAL_RCC_GPIOA_CLK_DISABLE(); break;    \
      case GPIOB_BASE: __HAL_RCC_GPIOB_CLK_DISABLE(); break;    \
      case GPIOC_BASE: __HAL_RCC_GPIOC_CLK_DISABLE(); break;    \
      case GPIOD_BASE: __HAL_RCC_GPIOD_CLK_DISABLE(); break;    \
      case GPIOH_BASE: default:  __HAL_RCC_GPIOH_CLK_ENABLE(); \
    }                                                    \
  } while(0)


static ketCube_GPIO_VoidFn_t ketCube_GPIO_IrqHandlers[16] = { NULL };

/**
 * @brief Get the position of the bit
 * @param  pin pin (0 .. 15)
 * 
 * @retval pos the position of the bit
 */
static inline uint8_t ketCube_GPIO_GetBitPos(uint16_t pin)
{
    uint8_t pos = 0;

    if ((pin & 0xFF00) != 0) {
        pos |= 0x8;
    }
    if ((pin & 0xF0F0) != 0) {
        pos |= 0x4;
    }
    if ((pin & 0xCCCC) != 0) {
        pos |= 0x2;
    }
    if ((pin & 0xAAAA) != 0) {
        pos |= 0x1;
    }

    return pos;
}

/**
 * @brief Get the PORT index
 * @param  port @see ketCube_gpio_port_t
 * 
 * @retval index port index; 0xFF in case of invalid port
 */
static inline uint8_t getPortIndex(ketCube_gpio_port_t port) {
    switch (port) {
        case KETCUBE_GPIO_PA:
            return 0;
        case KETCUBE_GPIO_PB:
            return 1;
        case KETCUBE_GPIO_PC:
            return 2;
        default:
            return 0xFF;
    }
}

/**
 * @brief Get the PORT index
 * @param  port @see ketCube_gpio_port_t
 * 
 * @retval name port name (A, B or C; 0xFF in case of invalid port
 */
static inline char getPortName(ketCube_gpio_port_t port) {
    switch (port) {
        case KETCUBE_GPIO_PA:
            return 'A';
        case KETCUBE_GPIO_PB:
            return 'B';
        case KETCUBE_GPIO_PC:
            return 'C';
        default:
            return 0xFF;
    }
}

/**
 * @brief Dummy irq handler
 */
void ketCube_GPIO_noneIrqHandler()
{
    // this function exist
    // just to have valid function pointer ... 
}

/**
 * @brief Initializes the GPIO driver
 * 
 * @note this sets all PINs into initial state
 * 
 */
void ketCube_GPIO_InitDriver(void) {
    GPIO_InitTypeDef GPIO_InitStruct={0};
    
    /* Configure all GPIO as analog to reduce current consumption on non used IOs */
    /* Enable GPIOs clock */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    /* All GPIOs except debug pins (SWCLK and SWD) */
    GPIO_InitStruct.Pin = GPIO_PIN_All & (~( GPIO_PIN_13 | GPIO_PIN_14) );
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* All GPIOs */
    GPIO_InitStruct.Pin = GPIO_PIN_All;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
    
    /* Disable GPIOs clock */
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOH_CLK_DISABLE();
}

/**
 * @brief Initializes the GPIO PIN(s)
 *
 * @param  port GPIO port 
 * @param  pin GPIO PIN
 * @param  initStruct  GPIO_InitTypeDef intit structure
 * 
 * @retval KETCUBE_CFG_DRV_OK in case of success
 * @retval KETCUBE_CFG_DRV_ERROR in case of failure
 */
ketCube_cfg_DrvError_t ketCube_GPIO_Init(ketCube_gpio_port_t port,
                                         uint16_t pin,
                                         GPIO_InitTypeDef * initStruct)
{
    uint8_t portIndex = getPortIndex(port);
    char portName = getPortName(port);
    
    if (portIndex == 0xFF) {
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_ERROR, "The declared port cannot be used!");
        return KETCUBE_CFG_DRV_ERROR;
    }
    
    if ((pinUsage[portIndex] & pin) != 0) {
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_ERROR, "P%c%d is already in use!", portName, ketCube_GPIO_GetBitPos(pin));
        return KETCUBE_CFG_DRV_ERROR;
    }
    
    RCC_GPIO_CLK_ENABLE((uint32_t) port);
    initStruct->Pin = pin;
    HAL_GPIO_Init((GPIO_TypeDef *) port, initStruct);

    ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_INFO, "P%c%d set-up", portName, ketCube_GPIO_GetBitPos(pin));
    pinUsage[portIndex] |= pin;
    
    return KETCUBE_CFG_DRV_OK;
}

/**
 * @brief Initializes the GPIO PIN(s) without usage checking
 *
 * @param  port GPIO port 
 * @param  pin GPIO PIN
 * @param  initStruct  GPIO_InitTypeDef intit structure
 * 
 * @retval KETCUBE_CFG_DRV_OK in case of success
 * @retval KETCUBE_CFG_DRV_ERROR in case of failure
 */
ketCube_cfg_DrvError_t ketCube_GPIO_ReInit(ketCube_gpio_port_t port,
                                           uint16_t pin,
                                           GPIO_InitTypeDef * initStruct)
{
    uint8_t portIndex = getPortIndex(port);
    char portName = getPortName(port);
    
    if (portIndex == 0xFF) {
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_ERROR, "The declared port cannot be used!");
        return KETCUBE_CFG_DRV_ERROR;
    }
    
    RCC_GPIO_CLK_ENABLE((uint32_t) port);
    initStruct->Pin = pin;
    HAL_GPIO_Init((GPIO_TypeDef *) port, initStruct);

    ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_DEBUG, "P%c%d re-init", portName, ketCube_GPIO_GetBitPos(pin));
    pinUsage[portIndex] |= pin;
    
    return KETCUBE_CFG_DRV_OK;
}

/**
 * @brief Release KETCube PIN - setup as input pin
 * 
 * This can help in low-power modes to avoid leakage when uart pin(s) drive(s) something.
 * 
 * @param port KETCube port
 * @param pin KETCube pin
 * 
 * @retval KETCUBE_CFG_DRV_OK in case of success
 * @retval KETCUBE_CFG_DRV_ERROR in case of failure
 */
ketCube_cfg_DrvError_t ketCube_GPIO_Release(ketCube_gpio_port_t port, ketCube_gpio_pin_t pin)
{
    static GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    uint8_t portIndex = getPortIndex(port);
    char portName = getPortName(port);
    
    if (portIndex == 0xFF) {
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_ERROR, "The declared port cannot be used!");
        return KETCUBE_CFG_DRV_ERROR;
    }
    
    if ((pinUsage[portIndex] & pin) == 0) {
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_DEBUG, "P%c%d already released", portName, ketCube_GPIO_GetBitPos(pin));
    }
    
    ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_INFO, "P%c%d released", portName, ketCube_GPIO_GetBitPos(pin));
    
    pinUsage[portIndex] &= ~(pin);

    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;

    HAL_GPIO_Init((GPIO_TypeDef *) port, &GPIO_InitStruct);

    return KETCUBE_CFG_DRV_OK;
}

/**
 * @brief Records the interrupt handler for the GPIO  object
 *
 * @param  port GPIO port 
 * @param  pin GPIO PIN
 * @param  prio NVIC priority (0 is highest)
 * @param  irqHandler pointer to handling function
 * 
 * @retval KETCUBE_CFG_DRV_OK in case of success
 * @retval KETCUBE_CFG_DRV_ERROR in case of failure
 */
ketCube_cfg_DrvError_t ketCube_GPIO_SetIrq(ketCube_gpio_port_t port,
                                           ketCube_gpio_pin_t pin,
                                           uint32_t prio,
                                           ketCube_GPIO_VoidFn_t
                                           irqHandler)
{
    IRQn_Type IRQnb;
    uint8_t index = ketCube_GPIO_GetBitPos(pin);

    if (ketCube_GPIO_IrqHandlers[index] != NULL) {
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_ERROR, "EXTI line %d already used!", index);
        return KETCUBE_CFG_DRV_ERROR;
    } else {
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_INFO, "Registering EXTI line %d", index);
    }

    if (irqHandler != NULL) {
        ketCube_GPIO_IrqHandlers[index] =
            (ketCube_GPIO_VoidFn_t) irqHandler;
    } else {
        // this means interrupt is used, but has no specific handler
        // this can be useful for wake-up ...  
        ketCube_GPIO_IrqHandlers[index] =
            (ketCube_GPIO_VoidFn_t) & ketCube_GPIO_noneIrqHandler;
    }

    IRQnb = MSP_GetIRQn(pin);
    HAL_NVIC_SetPriority(IRQnb, prio, 0);
    HAL_NVIC_EnableIRQ(IRQnb);

    return KETCUBE_CFG_DRV_OK;
}

/**
 * @brief Writes the bit value to the GPIO PIN
 *
 * @param port GPIO port 
 * @param pin GPIO PIN
 * @param bit TRUE if PIN value is '1', else return FALSE
 * 
 */
void ketCube_GPIO_Write(ketCube_gpio_port_t port, ketCube_gpio_pin_t pin,
                        bool bit)
{
    HAL_GPIO_WritePin((GPIO_TypeDef *) port, pin, (GPIO_PinState) bit);
}

/**
 * @brief Reads the current PIN value
 *
 * @param  port GPIO port 
 * @param  pin GPIO PIN
 * 
 * @retval bit TRUE if PIN value is '1', else return FALSE
 */
bool ketCube_GPIO_Read(ketCube_gpio_port_t port, ketCube_gpio_pin_t pin)
{
    return (bool) HAL_GPIO_ReadPin((GPIO_TypeDef *) port, pin);
}

/**
 * @brief Set-UP GPIO befere sleep enter
 * 
 * Set unused GPIO PINs to analog
 * 
 * @note This function should be called by KETCube core
 * 
 */
ketCube_cfg_DrvError_t ketCube_GPIO_SleepEnter(void) {
    GPIO_InitTypeDef GPIO_InitStruct={0};
    uint8_t portIndex;
    
    // process portA
    portIndex = getPortIndex(KETCUBE_GPIO_PA);
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    /* All GPIOs except used PINs */
    GPIO_InitStruct.Pin = GPIO_PIN_All & (~(pinUsage[portIndex]) );
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    if (pinUsage[portIndex] == 0) {
        // no PINs used!
        __HAL_RCC_GPIOA_CLK_DISABLE();
    }
    
    // process portB
    portIndex = getPortIndex(KETCUBE_GPIO_PB);
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    /* All GPIOs except used PINs */
    GPIO_InitStruct.Pin = GPIO_PIN_All & (~(pinUsage[portIndex]) );
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    if (pinUsage[portIndex] == 0) {
        // no PINs used!
        __HAL_RCC_GPIOB_CLK_DISABLE();
    }
    
    // process portC
    portIndex = getPortIndex(KETCUBE_GPIO_PC);
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    /* All GPIOs except used PINs */
    GPIO_InitStruct.Pin = GPIO_PIN_All & (~(pinUsage[portIndex]) );
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    if (pinUsage[portIndex] == 0) {
        // no PINs used!
        __HAL_RCC_GPIOC_CLK_DISABLE();
    }
    
    return KETCUBE_CFG_DRV_OK;
}

/**
 * @brief Execute the irqHandler
 * This function redefines a weak deffinition from stm32l0xx_hal_gpio.c
 *
 * @param pin GPIO PIN
 * 
 */
void HAL_GPIO_EXTI_Callback(ketCube_gpio_pin_t pin)
{
    uint8_t index = ketCube_GPIO_GetBitPos(pin);

    if (ketCube_GPIO_IrqHandlers[index] != NULL) {
        ketCube_GPIO_IrqHandlers[index] (NULL);
    }
}

void EXTI0_1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI2_3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}


void EXTI4_15_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

/**
  * @brief  Gets IRQ number as a function of the GPIO_Pin.
  * @param  GPIO_Pin: Specifies the pins connected to the EXTI line.
  * @retval IRQ number
  */
IRQn_Type MSP_GetIRQn( uint16_t GPIO_Pin)
{
  switch( GPIO_Pin )
  {
    case GPIO_PIN_0:  
    case GPIO_PIN_1:  return EXTI0_1_IRQn;
    case GPIO_PIN_2: 
    case GPIO_PIN_3:  return EXTI2_3_IRQn;
    case GPIO_PIN_4:  
    case GPIO_PIN_5:  
    case GPIO_PIN_6:
    case GPIO_PIN_7:
    case GPIO_PIN_8:
    case GPIO_PIN_9:  
    case GPIO_PIN_10:
    case GPIO_PIN_11:
    case GPIO_PIN_12:
    case GPIO_PIN_13:
    case GPIO_PIN_14:
    case GPIO_PIN_15: 
    default: return EXTI4_15_IRQn;
  }
}
