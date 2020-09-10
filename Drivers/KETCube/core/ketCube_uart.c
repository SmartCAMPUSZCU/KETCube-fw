/**
 * @file    ketCube_uart.c
 * @author  Martin Ubl
 * @version 0.1
 * @date    2018-03-07
 * @brief   This file contains definitions for the UART manager
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

#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_uart.h"

#include "ketCube_uart.h"
#include "ketCube_gpio.h"

/**
* @brief Array of registered descriptors
*/
static ketCube_UART_descriptor_t
    * ketCube_UART_descriptors[KETCUBE_UART_CHANNEL_COUNT] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

/**
 * @brief Register UART channel for exclusive access
 * @param channel		UART channel to be registered
 * @param descriptor	descriptor with filled information
 * @retval KETCUBE_CFG_DRV_OK in case of success
 * @retval KETCUBE_CFG_DRV_ERROR in case of failure
 */
ketCube_cfg_DrvError_t ketCube_UART_RegisterHandle(ketCube_UART_ChannelNo_t
                                                   channel,
                                                   ketCube_UART_descriptor_t
                                                   * descriptor)
{
    if (ketCube_UART_descriptors[channel] != NULL) {
        return KETCUBE_CFG_DRV_ERROR;
    }
    // the descriptor needs to be set in order to call appropriate callbacks
    ketCube_UART_descriptors[channel] = descriptor;

    if (HAL_UART_Init(descriptor->handle) != HAL_OK) {
        // clear descriptor in case of failure
        ketCube_UART_descriptors[channel] = NULL;
        return KETCUBE_CFG_DRV_ERROR;
    }
    // enable IRQ and set priority
    HAL_NVIC_SetPriority(descriptor->irqNumber, descriptor->irqPriority,
                         descriptor->irqSubPriority);
    HAL_NVIC_EnableIRQ(descriptor->irqNumber);

    return KETCUBE_CFG_DRV_OK;
}

/**
 * @brief Unregister UART channel
 * @param channel		UART channel to be unregistered
 * @retval KETCUBE_CFGDRV_OK in case of success
 * @retval KETCUBE_CFG_DRV_ERROR in case of failure
 */
ketCube_cfg_DrvError_t
ketCube_UART_UnRegisterHandle(ketCube_UART_ChannelNo_t channel)
{
    if (ketCube_UART_descriptors[channel] == NULL)
        return KETCUBE_CFG_DRV_ERROR;

    // deinitialize IO pins
    ketCube_UART_IoDeInitCallback(channel);

    ketCube_UART_descriptors[channel] = NULL;

    return KETCUBE_CFG_DRV_OK;
}

/**
 * @brief Retrieve UART handle for given channel
 * @param channel		UART channel
 * @retval valid handle or NULL if the channel hasn't been registered
 */
UART_HandleTypeDef *ketCube_UART_GetHandle(ketCube_UART_ChannelNo_t
                                           channel)
{
    return ketCube_UART_descriptors[channel] ==
        NULL ? NULL : ketCube_UART_descriptors[channel]->handle;
}

// these are just plain callbacks; their body repeats, so we could easily define macro for that

#define DEFINE_CALLBACK_FNC(cbname, cbfnc) void ketCube_UART_##cbname(ketCube_UART_ChannelNo_t channel)\
{\
	if (ketCube_UART_descriptors[channel] == NULL || ketCube_UART_descriptors[channel]->cbfnc == NULL){\
		return;\
	}\
	(ketCube_UART_descriptors[channel]->cbfnc )();\
}

/**
 * @brief IRQ callback for given channel
 */
DEFINE_CALLBACK_FNC(IRQCallback, fnIRQCallback);

/**
 * @brief IRQ callback for given channel
 */
DEFINE_CALLBACK_FNC(ReceiveCallback, fnReceiveCallback);

/**
 * @brief TX complete callback for given channel
 * @param channel		UART channel
 */
DEFINE_CALLBACK_FNC(TransmitCallback, fnTransmitCallback);

/**
 * @brief Error callback for given channel
 */
DEFINE_CALLBACK_FNC(ErrorCallback, fnErrorCallback);

/**
 * @brief Wakeup callback for given channel
 */
DEFINE_CALLBACK_FNC(WakeupCallback, fnWakeupCallback);

/**
 * @brief IO ports init callback for given channel
 */
DEFINE_CALLBACK_FNC(IoInitCallback, fnIoInit);

/**
 * @brief IO ports deinit callback for given channel
 */
DEFINE_CALLBACK_FNC(IoDeInitCallback, fnIoDeInit);

/**
 * @brief Initialize all registered descriptors (e.g. when going back from sleep)
 */
void ketCube_UART_IoInitAll(void)
{
    int i;
    for (i = 0; i < KETCUBE_UART_CHANNEL_COUNT; i++) {
        if (ketCube_UART_descriptors[i] != NULL
            && ketCube_UART_descriptors[i]->fnIoInit != NULL)
            (ketCube_UART_descriptors[i]->fnIoInit) ();
    }
}

/**
 * @brief Denitialize all registered descriptors (e.g. when going to sleep)
 */
void ketCube_UART_IoDeInitAll(void)
{
    int i;
    for (i = 0; i < KETCUBE_UART_CHANNEL_COUNT; i++) {
        if (ketCube_UART_descriptors[i] != NULL
            && ketCube_UART_descriptors[i]->fnIoDeInit != NULL)
            (ketCube_UART_descriptors[i]->fnIoDeInit) ();
    }
}

/**
 * @brief Setup UART PIN(s)
 * 
 * @param pin KETCube PIN
 * @param port KETCube PORT
 * @param af alternate function for selected UART peripheral and PIN
 * 
 * @retval KETCUBE_CFG_DRV_OK in case of success
 * @retval KETCUBE_CFG_DRV_ERROR in case of failure
 */
ketCube_cfg_DrvError_t ketCube_UART_SetupPin(ketCube_gpio_port_t port,
                                             ketCube_gpio_pin_t pin,
                                             uint8_t af)
{
    static GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = af;

    return ketCube_GPIO_Init(port, pin, &GPIO_InitStruct);
}

/**
 * @brief Disable All UART Instances
 * 
 */
void ketCube_UART_DisableAll(void) {
    
    int i;
    for (i = 0; i < KETCUBE_UART_CHANNEL_COUNT; i++) {
        if (ketCube_UART_descriptors[i] != NULL){
            __HAL_UART_DISABLE(ketCube_UART_descriptors[i]->handle);
        }
    }
}

/**
 * @brief Enable All Registered UART Instances
 * 
 */
void ketCube_UART_EnableAll(void) {
    
    int i;
    for (i = 0; i < KETCUBE_UART_CHANNEL_COUNT; i++) {
        if (ketCube_UART_descriptors[i] != NULL){
            __HAL_UART_ENABLE(ketCube_UART_descriptors[i]->handle);
        }
    }
}
