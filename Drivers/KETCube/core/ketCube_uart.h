/**
 * @file    ketCube_uart.h
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
#ifndef __KETCUBE_UART_H
#define __KETCUBE_UART_H

#include "ketCube_cfg.h"
#include "ketCube_gpio.h"

/** @defgroup KETCube_UART KETCube UART driver
  * @brief KETCube UART driver for STM32L0
  *
  * @ingroup KETCube_CoreDrivers
  * @{
  */

#define KETCUBE_UART_NAME               "uart_drv"        ///< UART driver name

/**
* @brief UART supported channels
* 
* @todo rename to respect channel names: USART 1,2,4,5, LPUART
*/
typedef enum {
    KETCUBE_UART_CHANNEL_1 = 0,
    KETCUBE_UART_CHANNEL_2 = 1,
    KETCUBE_UART_CHANNEL_3 = 2,
    KETCUBE_UART_CHANNEL_4 = 3,
    KETCUBE_UART_CHANNEL_5 = 4,

    KETCUBE_UART_CHANNEL_COUNT
} ketCube_UART_ChannelNo_t;

typedef void (*ketCube_UART_SimpleCbFn_t) (void);

/**
* @brief UART descriptor structure
*/
typedef struct {
    UART_HandleTypeDef *handle;
    IRQn_Type irqNumber;
    int irqPriority;
    int irqSubPriority;
    ketCube_UART_SimpleCbFn_t fnIoInit;
    ketCube_UART_SimpleCbFn_t fnIoDeInit;
    ketCube_UART_SimpleCbFn_t fnIRQCallback;
    ketCube_UART_SimpleCbFn_t fnReceiveCallback;
    ketCube_UART_SimpleCbFn_t fnTransmitCallback;
    ketCube_UART_SimpleCbFn_t fnErrorCallback;
    ketCube_UART_SimpleCbFn_t fnWakeupCallback;
} ketCube_UART_descriptor_t;

extern ketCube_cfg_DrvError_t
ketCube_UART_RegisterHandle(ketCube_UART_ChannelNo_t channel,
                            ketCube_UART_descriptor_t * descriptor);
extern ketCube_cfg_DrvError_t
ketCube_UART_UnRegisterHandle(ketCube_UART_ChannelNo_t channel);
extern ketCube_cfg_DrvError_t ketCube_UART_SetupPin(ketCube_gpio_pin_t pin,
                                             ketCube_gpio_port_t port,
                                             uint8_t af
                                            );

extern UART_HandleTypeDef *ketCube_UART_GetHandle(ketCube_UART_ChannelNo_t
                                                  channel);

extern void ketCube_UART_IRQCallback(ketCube_UART_ChannelNo_t channel);
extern void ketCube_UART_ReceiveCallback(ketCube_UART_ChannelNo_t channel);
extern void ketCube_UART_TransmitCallback(ketCube_UART_ChannelNo_t
                                          channel);
extern void ketCube_UART_ErrorCallback(ketCube_UART_ChannelNo_t channel);
extern void ketCube_UART_WakeupCallback(ketCube_UART_ChannelNo_t channel);

extern void ketCube_UART_IoInitCallback(ketCube_UART_ChannelNo_t channel);
extern void ketCube_UART_IoDeInitCallback(ketCube_UART_ChannelNo_t
                                          channel);

extern void ketCube_UART_IoInitAll(void);
extern void ketCube_UART_IoDeInitAll(void);

/**
* @}
*/

#endif                          /* __KETCUBE_UART_H */
