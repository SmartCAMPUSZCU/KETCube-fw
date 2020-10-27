/**
 *
 * @file    ketCube_uart2WAN.c
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

#include "ketCube_cfg.h"
#include "ketCube_modules.h"

#include "ketCube_gpio.h"
#include "ketCube_uart2WAN.h"
#include "ketCube_terminal.h"

#include "ketCube_common.h"

#include "hw.h"
#include "timeServer.h"

#include <string.h>

#ifdef KETCUBE_CFG_INC_MOD_UART2WAN


#define KETCUBE_UART2WAN_RX_BUFFER_SIZE   20

ketCube_uart2WAN_moduleCfg_t ketCube_uart2WAN_moduleCfg; /*!< Module configuration storage */

/* recv buffer */
static uint8_t rxBuffer[KETCUBE_UART2WAN_RX_BUFFER_SIZE];
static volatile bool rxBufferTransmitted = TRUE; /* was the buffer content transmitted through WAN? */
static volatile bool rxInProgress = FALSE; /* Rx is in progress */
/* position in recv buffer */
static volatile int rxPos = 1;

/* stored UART handle */
static UART_HandleTypeDef thisUARTHandle;
/* UART descriptor */
static ketCube_UART_descriptor_t thisDescriptor;
/* temporary byte buffer */
static uint8_t rxByte;

static void ketCube_uart2WAN_IoInit(void);
static void ketCube_uart2WAN_IoDeInit(void);
static void ketCube_uart2WAN_RXCompleteCallback(void);
static void ketCube_uart2WAN_TXCompleteCallback(void);
static void ketCube_uart2WAN_ErrorCallback(void);
static void ketCube_uart2WAN_WakeupCallback(void);

static void ketCube_uart2WAN_Send(uint8_t * buffer, int count);
static ketCube_cfg_ModError_t ketCube_uart2WAN_AwaitFrame(uint32_t timeout);

/**
 * @brief  Configures interface.
 * @param  None
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
ketCube_cfg_ModError_t ketCube_uart2WAN_Init(ketCube_InterModMsg_t *** msg)
{    
    /* USART2 instance */
    thisUARTHandle.Instance = KETCUBE_UART2WAN_USART_INSTANCE;

    thisUARTHandle.Init.BaudRate = KETCUBE_UART2WAN_USART_BAUDRATE;
    thisUARTHandle.Init.WordLength = KETCUBE_UART2WAN_USART_DATA_BITS;
    thisUARTHandle.Init.StopBits = KETCUBE_UART2WAN_USART_STOP_BITS;
    thisUARTHandle.Init.Parity = KETCUBE_UART2WAN_USART_PARITY;
    thisUARTHandle.Init.HwFlowCtl = KETCUBE_UART2WAN_USART_HW_FLOW_CONTROL;
    thisUARTHandle.Init.Mode = KETCUBE_UART2WAN_USART_INIT_MODE;

    KETCUBE_UART2WAN_USART_SET_CLK_SRC();

    /* register callbacks in generic UART manager */
    thisDescriptor.handle = &thisUARTHandle;
    thisDescriptor.irqNumber = KETCUBE_UART2WAN_USART_IRQ_NUMBER;
    thisDescriptor.irqPriority = KETCUBE_UART2WAN_USART_IRQ_PRIORITY;
    thisDescriptor.irqSubPriority = KETCUBE_UART2WAN_USART_IRQ_SUBPRIORITY;
    thisDescriptor.fnIoInit = &ketCube_uart2WAN_IoInit;
    thisDescriptor.fnIoDeInit = &ketCube_uart2WAN_IoDeInit;
    thisDescriptor.fnIRQCallback = NULL;
    thisDescriptor.fnReceiveCallback = &ketCube_uart2WAN_RXCompleteCallback;
    thisDescriptor.fnTransmitCallback = &ketCube_uart2WAN_TXCompleteCallback;
    thisDescriptor.fnErrorCallback = &ketCube_uart2WAN_ErrorCallback;
    thisDescriptor.fnWakeupCallback = &ketCube_uart2WAN_WakeupCallback;

    if (ketCube_UART_RegisterHandle
        (KETCUBE_UART2WAN_USART_CHANNEL,
         &thisDescriptor) != KETCUBE_CFG_DRV_OK) {
        Error_Handler();
    }

    return KETCUBE_CFG_MODULE_OK;
}

/**
  * @brief Process data -- display on serial line
  *
  * @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_uart2WAN_ProcessData(ketCube_InterModMsg_t * msg)
{
    /* msg->msgLen-2 to remove terminating zeroes and the first byte */
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_UART2WAN, "WAN Rx (%d)=%s", msg->msgLen-2,
    ketCube_common_bytes2Str(&(msg->msg[1]), msg->msgLen-2));

    // remove first byte and terminating zero byte
    ketCube_uart2WAN_Send(&(msg->msg[1]), msg->msgLen-2);

    // confirm msg reception 
    msg->msgLen = 0;
    
    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Initialize IO PINs
 */
void ketCube_uart2WAN_IoInit(void)
{
    /* enable USART and RX/TX port clocks */
    KETCUBE_UART2WAN_USART_CLK_ENABLE();

    /* enable RX/TX pins */
    ketCube_UART_SetupPin(KETCUBE_UART2WAN_USART_TX_PIN_PORT,
                          KETCUBE_UART2WAN_USART_TX_PIN,
                          KETCUBE_UART2WAN_USART_TX_PIN_AF
                         );
    ketCube_UART_SetupPin(KETCUBE_UART2WAN_USART_RX_PIN_PORT,
                          KETCUBE_UART2WAN_USART_RX_PIN,
                          KETCUBE_UART2WAN_USART_RX_PIN_AF
                         );
}

/**
 * @brief Deinitialize
 */
void ketCube_uart2WAN_IoDeInit(void)
{
    /* disable RX/TX pins */
    ketCube_GPIO_Release(KETCUBE_UART2WAN_USART_RX_PIN_PORT,
                         KETCUBE_UART2WAN_USART_RX_PIN);
    ketCube_GPIO_Release(KETCUBE_UART2WAN_USART_TX_PIN_PORT,
                         KETCUBE_UART2WAN_USART_TX_PIN);
    
    KETCUBE_UART2WAN_USART_CLK_DISABLE();
}

/**
 * @brief UART error callback
 */
void ketCube_uart2WAN_ErrorCallback(void)
{
    HAL_UART_Receive_IT(&thisUARTHandle, &rxByte, 1);
}

/**
 * @brief UART wakeup callback
 */
void ketCube_uart2WAN_WakeupCallback(void)
{
    HAL_UART_Receive_IT(&thisUARTHandle, &rxByte, 1);
}

/**
 * @brief UART TX complete callback
 */
void ketCube_uart2WAN_TXCompleteCallback(void)
{

}

/**
 * @brief UART RX complete callback
 */
void ketCube_uart2WAN_RXCompleteCallback(void)
{
    uint8_t byte = rxByte;
    
    if (rxInProgress == TRUE) {
        HAL_UART_Receive_IT(&thisUARTHandle, &rxByte, 1);
    }
    
    if (rxPos < KETCUBE_UART2WAN_RX_BUFFER_SIZE) {
        rxBuffer[rxPos++] = byte;
    } else {
        rxInProgress = FALSE; // terminate now!
    }
}

/**
 * @brief Function for sending formatted data through UART,
 *        Switches node to driver mode
 * @param buffer	data to be sent
 * @param count		length of data
 */
static void ketCube_uart2WAN_Send(uint8_t * buffer, int count)
{
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_UART2WAN, "UART Tx (%d)=%s", count,
    ketCube_common_bytes2Str(&(buffer[0]), count));

    HAL_UART_Transmit(&thisUARTHandle, &(buffer[0]), count, KETCUBE_UART2WAN_USART_TIMEOUT);

    if (ketCube_uart2WAN_AwaitFrame(KETCUBE_UART2WAN_USART_TIMEOUT) == KETCUBE_CFG_MODULE_OK) {
        
        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_UART2WAN, "UART Rx (%d)=%s", rxPos,
        ketCube_common_bytes2Str(&(rxBuffer[0]), rxPos));
    } else {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_UART2WAN, "UART Rx timeout!");
        rxBuffer[0] = 0; /* indicate error */
    }
    
    rxBufferTransmitted = FALSE;
}

/**
 * @brief Await frame on input for <timeout> milliseconds
 * @param timeout		how many milliseconds should we wait for frame?
 * 
 * @return KETCUBE_CFG_MODULE_OK if frame has been received
 * @return KETCUBE_CFG_MODULE_ERROR otherwise (timeout)
 */
static ketCube_cfg_ModError_t ketCube_uart2WAN_AwaitFrame(uint32_t timeout)
{
    TimerTime_t delayValue = 0;
    TimerTime_t tickstart = 0;
    
    // init Rx
    rxPos = 0;
    rxInProgress = TRUE;
    HAL_UART_Receive_IT(&thisUARTHandle, &rxByte, 1);
    
    delayValue = HW_RTC_ms2Tick(timeout);

    tickstart = HW_RTC_GetTimerValue();
    while (((HW_RTC_GetTimerValue() - tickstart)) < delayValue) {
        __NOP();
    }
    
    rxInProgress = FALSE;
    
    if (rxPos > 0) {
        return KETCUBE_CFG_MODULE_OK;
    } else {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    return KETCUBE_CFG_MODULE_ERROR;
}

/**
 * @brief Callback function for KETCube system - read sensor data
 * @param buffer	buffer for incoming data which are cut by set rules
 * @param len		length we actually filled
 */
ketCube_cfg_ModError_t ketCube_uart2WAN_ReadData(uint8_t * buffer,
                                                 uint8_t * len)
{
    // transmit response data if any
    if (rxBufferTransmitted == FALSE) {
        *len = rxPos;
        
        /* check buffer overflow*/
        if (*len > KETCUBE_UART2WAN_RX_BUFFER_SIZE) {
            *len = KETCUBE_UART2WAN_RX_BUFFER_SIZE;
        }
        
        /* check for timeout error */
        if (*len == 0) {
            *len = 1;
            buffer[0] = 0x00;
            ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_UART2WAN, "Transmitting response: timeout");
        } else {
            buffer[0] = *len;
            memcpy(&(buffer[1]), rxBuffer, *len);
            ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_UART2WAN, "Transmitting response: %d bytes", *len);
            (*len)++;
        }
        rxBufferTransmitted = TRUE;
    } else {
        buffer[0] = 0xFF;
        *len = 1;
    }
    
    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Sleep exit
 *
 * @retval KETCUBE_CFG_MODULE_OK go sleep
 * @retval KETCUBE_CFG_MODULE_ERROR do not go sleep
 *
 */
ketCube_cfg_ModError_t ketCube_uart2WAN_SleepExit(void)
{
    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Prepare sleep mode
 *
 * @retval KETCUBE_CFG_MODULE_OK go sleep
 * @retval KETCUBE_CFG_MODULE_ERROR do not go sleep
 *
 */
ketCube_cfg_ModError_t ketCube_uart2WAN_SleepEnter(void)
{
    return KETCUBE_CFG_MODULE_OK;
}

#endif                          /* KETCUBE_CFG_INC_MOD_UART2WAN */
