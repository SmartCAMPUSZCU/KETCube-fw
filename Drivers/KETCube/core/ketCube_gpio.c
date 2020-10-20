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

#include "timeServer.h"
#include "ketCube_cfg.h"
#include "ketCube_gpio.h"
#include "ketCube_terminal.h"

static volatile ketCube_gpio_descriptor_t pinDescr[3][16] = { {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} }; ///< PIN descriptor for ports A, B and C

static void ketCube_gpio_LEDHandler(void* context);
static TimerEvent_t ketCube_gpio_LEDTimer = { 0 }; // Timer for LED support

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


extern IRQn_Type MSP_GetIRQn(uint16_t GPIO_Pin);
static ketCube_GPIO_VoidFn_t ketCube_GPIO_IrqHandlers[16] = { NULL };

/**
 * @brief Get the PIN from index
 * @param  index index
 * 
 * @retval pin PIN
 */
static inline ketCube_gpio_pin_t getPin(uint8_t index) {
    return (ketCube_gpio_pin_t) (0x0001 << index);
}

/**
 * @brief Get the position of the bit
 * @param  pin pin (0 .. 15)
 * 
 * @retval pos the position of the bit
 */
static inline uint8_t getPinIndex(uint16_t pin)
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
 * @brief Get the PORT from index
 * @param  index index
 * 
 * @retval port port
 */
static inline ketCube_gpio_port_t getPort(uint8_t index) {
    switch (index) {
        case 0:
            return KETCUBE_GPIO_PA;
        case 1:
            return KETCUBE_GPIO_PB;
        case 2:
            return KETCUBE_GPIO_PC;
        default:
            return 0xFF;
    }
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
 * @brief Set-UP GPIO befere sleep enter
 * 
 * Set unused GPIO PINs to analog -- this should be set by ketCube_GPIO_Release()
 * 
 * @note This function should be called by KETCube core
 * 
 */
ketCube_cfg_DrvError_t ketCube_GPIO_SleepEnter(void) {
    
    return KETCUBE_CFG_DRV_OK;
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
    uint8_t pinIndex = getPinIndex(pin);
    char portName = getPortName(port);
    
    if ((portIndex == 0xFF) || (pinIndex > 16)) {
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_ERROR, "The declared PIN/Port cannot be used!");
        return KETCUBE_CFG_DRV_ERROR;
    }
    
    if (pinDescr[portIndex][pinIndex].used == TRUE) {
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_ERROR, "P%c%d is already in use!", portName, getPinIndex(pin));
        return KETCUBE_CFG_DRV_ERROR;
    }
    
    RCC_GPIO_CLK_ENABLE((uint32_t) port);
    initStruct->Pin = pin;
    HAL_GPIO_Init((GPIO_TypeDef *) port, initStruct);

    ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_INFO, "P%c%d set-up", portName, getPinIndex(pin));
    
    /* Mark PIn as used */
    pinDescr[portIndex][pinIndex].used = TRUE;
    /* set default function*/
    pinDescr[portIndex][pinIndex].function = KETCUBE_GPIO_FUNCTION_GPIO;
    
    return KETCUBE_CFG_DRV_OK;
}

/**
 * @brief Initializes the GPIO PIN(s) as LEDs
 *
 * @param  port GPIO port 
 * @param  pin GPIO PIN
 * @param  polarity LED PIN ON polarity - set to TRUE if LED is ON when output is 1
 * 
 * @retval KETCUBE_CFG_DRV_OK in case of success
 * @retval KETCUBE_CFG_DRV_ERROR in case of failure
 */
ketCube_cfg_DrvError_t ketCube_GPIO_InitLED(ketCube_gpio_port_t port, uint16_t pin, ketCube_gpio_polarity_t polarity)
{
    GPIO_InitTypeDef initStruct = {0};
    uint8_t portIndex = getPortIndex(port);
    uint8_t pinIndex = getPinIndex(pin);
    
    initStruct.Mode = GPIO_MODE_OUTPUT_PP;
    initStruct.Pull = GPIO_NOPULL;
    
    if (ketCube_GPIO_Init(port, pin, &initStruct) != KETCUBE_CFG_DRV_OK) {
        return KETCUBE_CFG_DRV_ERROR;
    }
    
    /* set LED PIN function*/
    pinDescr[portIndex][pinIndex].function = KETCUBE_GPIO_FUNCTION_LED;
    
    /* set LED ON polarity */
    pinDescr[portIndex][pinIndex].settings.polarity = polarity;
    
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
    uint8_t pinIndex = getPinIndex(pin);
    char portName = getPortName(port);
    
    if ((portIndex == 0xFF) || (pinIndex > 16)) {
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_ERROR, "The declared PIN/Port cannot be used!");
        return KETCUBE_CFG_DRV_ERROR;
    }
    
    RCC_GPIO_CLK_ENABLE((uint32_t) port);
    initStruct->Pin = pin;
    HAL_GPIO_Init((GPIO_TypeDef *) port, initStruct);

    ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_DEBUG, "P%c%d re-init", portName, getPinIndex(pin));
    
    /* mark PIN as used */
    pinDescr[portIndex][pinIndex].used = TRUE;
    
    /* set default function*/
    pinDescr[portIndex][pinIndex].function = KETCUBE_GPIO_FUNCTION_GPIO;
    
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
    uint8_t pinIndex = getPinIndex(pin);
    char portName = getPortName(port);
    
    if ((portIndex == 0xFF) || (pinIndex > 16)) {
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_ERROR, "The declared PIN/Port cannot be used!");
        return KETCUBE_CFG_DRV_ERROR;
    }
    
    if (pinDescr[portIndex][pinIndex].used == FALSE) {
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_DEBUG, "P%c%d already released", portName, getPinIndex(pin));
    }
    
    ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_INFO, "P%c%d released", portName, getPinIndex(pin));
    
    /* Mark PIN as unused */
    pinDescr[portIndex][pinIndex].used = FALSE;

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
    uint8_t index = getPinIndex(pin);

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
 * @brief Clear IRQ for the GPIO  object
 *
 * @param  port GPIO port 
 * @param  pin GPIO PIN
 * 
 * @retval KETCUBE_CFG_DRV_OK in case of success
 * @retval KETCUBE_CFG_DRV_ERROR in case of failure
 */
ketCube_cfg_DrvError_t ketCube_GPIO_ClearIrq(ketCube_gpio_port_t port,
                                             ketCube_gpio_pin_t pin)
{
    IRQn_Type IRQnb;
    uint8_t index = getPinIndex(pin);

    if (ketCube_GPIO_IrqHandlers[index] == NULL) {
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_ERROR, "EXTI line %d already cleared!", index);
        return KETCUBE_CFG_DRV_ERROR;
    } else {
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_INFO, "Clearing EXTI line %d", index);
    }

    ketCube_GPIO_IrqHandlers[index] = NULL;
    
    IRQnb = MSP_GetIRQn(pin);
    HAL_NVIC_DisableIRQ(IRQnb);

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
 * @brief Sets the LED state
 * 
 * @note pin must be initialized as LED pin (@see ketCube_GPIO_InitLED) prior calling this function - otherwise this function has no effect
 *
 * @param port GPIO port 
 * @param pin GPIO PIN
 * @param state set LED state @see ketCube_gpio_LEDState_t
 * 
 */
void ketCube_GPIO_SetLED(ketCube_gpio_port_t port, ketCube_gpio_pin_t pin, ketCube_gpio_LEDState_t state)
{
    uint8_t portIndex = getPortIndex(port);
    uint8_t pinIndex  = getPinIndex(pin);
    
    if ((portIndex == 0xFF) || (pinIndex > 16)) {
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_GPIO_NAME, KETCUBE_CFG_SEVERITY_ERROR, "The declared PIN/Port cannot be used!");
        return;
    }
    
    pinDescr[portIndex][pinIndex].settings.LED_state = state;
    
    switch(state) {
        case KETCUBE_GPIO_LED_BLINK_SINGLE:
        case KETCUBE_GPIO_LED_BLINK_CONT:
            /* start timer if not running */
            if (!(TimerIsStarted(&ketCube_gpio_LEDTimer))) {
                TimerInit(&ketCube_gpio_LEDTimer, ketCube_gpio_LEDHandler);
                TimerSetValue(&ketCube_gpio_LEDTimer, KETCUBE_GPIO_LED_PERIOD);
                TimerStart(&ketCube_gpio_LEDTimer);
            }
            break;
        case KETCUBE_GPIO_LED_ON:
            /* set inmediatelly */
            ketCube_GPIO_Write(port, pin, pinDescr[portIndex][pinIndex].settings.polarity);
            break;
        case KETCUBE_GPIO_LED_OFF:
        default:
            /* set inmediatelly */
            ketCube_GPIO_Write(port, pin, !(pinDescr[portIndex][pinIndex].settings.polarity));
            break;
    }
}

/**
 * @brief LED Periodic Handler
 * 
 * @note this function is executed out of the IRQ context
 * 
 */
void ketCube_gpio_LEDHandler(void* context) {
    static bool blinkValue = TRUE;
    bool blink = FALSE; /* is there any reason to "blink" next time ? */
    uint8_t port, pin;
    
    // use to synchronize blink
    blinkValue ^= TRUE;
    
    TimerStop(&ketCube_gpio_LEDTimer);
    
    for (port = 0; port < 3; port++) {
        for (pin = 0; pin < 16; pin++) {
            if ((pinDescr[port][pin].used == TRUE) && (pinDescr[port][pin].function == KETCUBE_GPIO_FUNCTION_LED)) {
                switch(pinDescr[port][pin].settings.LED_state) {
                    case KETCUBE_GPIO_LED_BLINK_SINGLE:
                        ketCube_GPIO_Write(getPort(port), getPin(pin), pinDescr[port][pin].settings.polarity);
                        pinDescr[port][pin].settings.LED_state = KETCUBE_GPIO_LED_OFF;
                        blink = TRUE; /* timer required ... */
                        break;
                    case KETCUBE_GPIO_LED_BLINK_CONT:
                        ketCube_GPIO_Write(getPort(port), getPin(pin), blinkValue);
                        blink = TRUE; /* timer required ... */
                        break;
                    case KETCUBE_GPIO_LED_ON:
                        /* this is written by ketCube_GPIO_SetLED */
                        break;
                    case KETCUBE_GPIO_LED_OFF:
                    default:
                        /* this is required, as KETCUBE_GPIO_LED_BLINK_SINGLE will turn LED off in the next round */
                        ketCube_GPIO_Write(getPort(port), getPin(pin), !(pinDescr[port][pin].settings.polarity));
                        break;
                }
            }
        }
    }
    
    if (blink != FALSE) {
        TimerSetValue(&ketCube_gpio_LEDTimer, KETCUBE_GPIO_LED_PERIOD);
        TimerStart(&ketCube_gpio_LEDTimer);
    }
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
    uint8_t index = getPinIndex(pin);

    if (ketCube_GPIO_IrqHandlers[index] != NULL) {
        ketCube_GPIO_IrqHandlers[index] (NULL);
    }
}

void EXTI0_1_IRQHandler(void)
{
    KETCube_eventsProcessed = FALSE; /* Possible pending events */
    
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI2_3_IRQHandler(void)
{
    KETCube_eventsProcessed = FALSE; /* Possible pending events */
    
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}


void EXTI4_15_IRQHandler(void)
{
    KETCube_eventsProcessed = FALSE; /* Possible pending events */
    
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
