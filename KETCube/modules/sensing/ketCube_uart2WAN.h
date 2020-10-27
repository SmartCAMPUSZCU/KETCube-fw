/**
 *
 * @file    ketCube_uart2WANe.h
 * @author  Jan Belohoubek 
 * @version alpha
 * @date    2020-10-27
 * 
 * @brief   This file contains the UART to WAN (e.g. LoRa) bridge/gateway
 * 
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2018 - 2020 University of West Bohemia in Pilsen
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
#ifndef __KETCUBE_UART2WAN_H
#define __KETCUBE_UART2WAN_H

#include "ketCube_uart.h"

/** @defgroup KETCube_uart2WAN
* @{
*/

/**
* @brief  KETCube module configuration
*/
typedef struct ketCube_uart2WAN_moduleCfg_t {
    ketCube_cfg_ModuleCfgByte_t coreCfg;      /*!< KETCube core cfg byte */
} ketCube_uart2WAN_moduleCfg_t;

extern ketCube_uart2WAN_moduleCfg_t ketCube_uart2WAN_moduleCfg;

/**
 * @brief enumerator of M-BUS configuration options addresses
 */
typedef enum {
    KETCUBE_UART2WAN_ERR_NOERR              = 0x00,        /*<! No error */
    KETCUBE_UART2WAN_ERR_TIMEOUT            = 0x01         /*<! Timeout */
} ketCube_uart2WAN_err_t;


#define KETCUBE_UART2WAN_USART_INSTANCE          USART2
#define KETCUBE_UART2WAN_USART_CHANNEL           KETCUBE_UART_CHANNEL_2
#define KETCUBE_UART2WAN_USART_SET_CLK_SRC()     __HAL_RCC_USART2_CONFIG(RCC_USART2CLKSOURCE_HSI)
#define KETCUBE_UART2WAN_USART_IRQ_NUMBER        USART2_IRQn
#define KETCUBE_UART2WAN_USART_IRQ_PRIORITY      0x1
#define KETCUBE_UART2WAN_USART_IRQ_SUBPRIORITY   1
#define KETCUBE_UART2WAN_USART_CLK_ENABLE()      __USART2_CLK_ENABLE()
#define KETCUBE_UART2WAN_USART_CLK_DISABLE()     __USART2_CLK_DISABLE()
#define KETCUBE_UART2WAN_USART_RX_PIN            KETCUBE_GPIO_PIN_2
#define KETCUBE_UART2WAN_USART_RX_PIN_AF         GPIO_AF4_USART2
#define KETCUBE_UART2WAN_USART_RX_PIN_PORT       KETCUBE_GPIO_PA
#define KETCUBE_UART2WAN_USART_TX_PIN            KETCUBE_GPIO_PIN_3
#define KETCUBE_UART2WAN_USART_TX_PIN_AF         GPIO_AF4_USART2
#define KETCUBE_UART2WAN_USART_TX_PIN_PORT       KETCUBE_GPIO_PA

#define KETCUBE_UART2WAN_USART_BAUDRATE          9600               /*<! default baudrate */
#define KETCUBE_UART2WAN_USART_DATA_BITS         UART_WORDLENGTH_8B /*<! default is 8. When parity is used, use 9 dta bits -- Oh, how uggly UART implementation */
#define KETCUBE_UART2WAN_USART_STOP_BITS         UART_STOPBITS_1    /*<! default is one stop bit */
#define KETCUBE_UART2WAN_USART_PARITY            UART_PARITY_NONE   /*<! default is no parity */
#define KETCUBE_UART2WAN_USART_HW_FLOW_CONTROL   UART_HWCONTROL_NONE        /*<! default M-BUS HW control */
#define KETCUBE_UART2WAN_USART_INIT_MODE         UART_MODE_TX_RX    /*<! default USART startup mode for M-BUS        */

#define KETCUBE_UART2WAN_USART_TIMEOUT           5000                /*<! UART Timeout in ms  */


/* module interface */

extern ketCube_cfg_ModError_t ketCube_uart2WAN_Init(ketCube_InterModMsg_t ***
                                                msg);
extern ketCube_cfg_ModError_t ketCube_uart2WAN_ReadData(uint8_t * buffer,
                                                    uint8_t * len);
extern ketCube_cfg_ModError_t ketCube_uart2WAN_ProcessData(ketCube_InterModMsg_t * msg);
extern ketCube_cfg_ModError_t ketCube_uart2WAN_SleepEnter(void);
extern ketCube_cfg_ModError_t ketCube_uart2WAN_SleepExit(void);

/**
* @}
*/

#endif                          /* __KETCUBE_UART2WAN_H */
