 /******************************************************************************
  * @file    vcom.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    27-February-2017
  * @brief   manages virtual com port
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	*
  ******************************************************************************
	*
	* @author  Jan Belohoubek
	* @author  Martin Ubl
  * @version 0.1
  * @date    2018-05-07
	* @brief   This file has been modified to fit into the KETCube platform
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
	*
  ******************************************************************************
  */
  
#include "hw.h"
#include "vcom.h"
#include <stdarg.h>

#include "ketCube_uart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUFSIZE 1024
#define USARTX_IRQn USART1_IRQn
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* buffer */
static char buff[BUFSIZE];
/* buffer write index*/
__IO uint16_t iw=0;
/* buffer read index*/
static uint16_t ir=0;

static char rxBuff[BUFSIZE];
/* Rx buffer write indexes */
__IO uint16_t rxBuffW=0;
__IO uint16_t rxBuffR=0;

/* Uart Handle */
UART_HandleTypeDef UartHandle;

static ketCube_UART_descriptor_t ketCube_vcom_descriptor;

void vcom_Print(void);
void vcom_Receive(void);
void vcom_ErrorCallback(void);
void vcom_WakeupCallback(void);
void vcom_IoInit(void);
void vcom_IoDeInit(void);

/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

void vcom_Init(void)
{
  /*## Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  UartHandle.Instance        = USARTX;
  
  UartHandle.Init.BaudRate   = 9600;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;
	
	//USARTx_RCC_CONFIG(RCC_USARTxCLKSOURCE_HSI);
	__HAL_RCC_USART1_CONFIG(RCC_USART1CLKSOURCE_HSI);
  
	/* register callbacks in generic UART manager */
	ketCube_vcom_descriptor.handle = &UartHandle;
	ketCube_vcom_descriptor.irqNumber = USARTX_IRQn;
	ketCube_vcom_descriptor.irqPriority = 0x1;
	ketCube_vcom_descriptor.irqSubPriority = 0;
	ketCube_vcom_descriptor.fnIoInit = &vcom_IoInit;
	ketCube_vcom_descriptor.fnIoDeInit = &vcom_IoDeInit;
	ketCube_vcom_descriptor.fnIRQCallback = &vcom_Print;
	ketCube_vcom_descriptor.fnReceiveCallback = &vcom_Receive;
	ketCube_vcom_descriptor.fnErrorCallback = &vcom_ErrorCallback;
	ketCube_vcom_descriptor.fnWakeupCallback = &vcom_WakeupCallback;
	
	if (ketCube_UART_RegisterHandle(KETCUBE_UART_CHANNEL_1, &ketCube_vcom_descriptor) != KETCUBE_CFG_MODULE_OK)
	{
		Error_Handler();
	}
	
	HAL_UART_Receive_IT(&UartHandle, (uint8_t *)&rxBuff[rxBuffW], 1);
}

void vcom_ErrorCallback(void)
{
	HAL_UART_Receive_IT(&UartHandle, (uint8_t *)&rxBuff[rxBuffW], 1);
}

void vcom_WakeupCallback(void)
{
	HAL_UART_Receive_IT(&UartHandle, (uint8_t *)&rxBuff[rxBuffW], 1);
}

void vcom_DeInit(void)
{
#if 1
  HAL_UART_DeInit(&UartHandle);
#endif
}

void vcom_Send( char *format, ... )
{
  va_list args;
  va_start(args, format);
  uint8_t len;
  uint8_t lenTop;
  char tempBuff[128];
  
  BACKUP_PRIMASK();
  DISABLE_IRQ();
  
  /*convert into string at buff[0] of length iw*/
  len = vsprintf(&tempBuff[0], format, args);
  
  if (iw+len<BUFSIZE)
  {
    memcpy( &buff[iw], &tempBuff[0], len);
    iw+=len;
  }
  else
  {
    lenTop=BUFSIZE-iw;
    memcpy( &buff[iw], &tempBuff[0], lenTop);
    len-=lenTop;
    memcpy( &buff[0], &tempBuff[lenTop], len);
    iw = len;
  }
  RESTORE_PRIMASK();
  
  HAL_NVIC_SetPendingIRQ(USARTX_IRQn);
    
  va_end(args);
}

/* modifes only ir*/
void vcom_Print( void)
{
  char* CurChar;
  while( ( (iw+BUFSIZE-ir)%BUFSIZE) >0 )
  {
    BACKUP_PRIMASK();
    DISABLE_IRQ();
    
    CurChar = &buff[ir];
    ir= (ir+1) %BUFSIZE;
    
    RESTORE_PRIMASK();
    
    HAL_UART_Transmit(&UartHandle,(uint8_t *) CurChar, 1, 300);    
  }
  HAL_NVIC_ClearPendingIRQ(USARTX_IRQn);
}

void vcom_Receive( void)
{
	PRINTF("Recv char: %c\n\r", rxBuff[rxBuffW]);
	
	rxBuffW = (rxBuffW + 1) % BUFSIZE;
	HAL_UART_Receive_IT(&UartHandle, (uint8_t *)&rxBuff[rxBuffW], 1);
}

bool IsNewCharReceived ( void)
{
	if (rxBuffW != rxBuffR) {
	  return TRUE;	
  }
	return FALSE;
}

char GetNewChar ( void)
{
	char byte;
	if (rxBuffW != rxBuffR) {
		byte = rxBuff[rxBuffR];	
		rxBuffR = (rxBuffR + 1) % BUFSIZE;
	  return byte;
  }
	return '\0';
}

void vcom_Send_Lp( char *format, ... )
{
  va_list args;
  va_start(args, format);
  uint8_t len;
  uint8_t lenTop;
  char tempBuff[128];
  
  BACKUP_PRIMASK();
  DISABLE_IRQ();
  
  /*convert into string at buff[0] of length iw*/
  len = vsprintf(&tempBuff[0], format, args);
  
  if (iw+len<BUFSIZE)
  {
    memcpy( &buff[iw], &tempBuff[0], len);
    iw+=len;
  }
  else
  {
    lenTop=BUFSIZE-iw;
    memcpy( &buff[iw], &tempBuff[0], lenTop);
    len-=lenTop;
    memcpy( &buff[0], &tempBuff[lenTop], len);
    iw = len;
  }
  RESTORE_PRIMASK();  
  
  va_end(args);
}

void vcom_IoInit(void)
{
	/* Enable USART1 clock */
  USARTX_CLK_ENABLE(); 

	/* Enable RX/TX port clocks */
  USARTX_TX_GPIO_CLK_ENABLE();
  USARTX_RX_GPIO_CLK_ENABLE();

	/* UART TX GPIO pin configuration  */
	
	ketCube_UART_SetupPin(USARTX_TX_PIN, USARTX_TX_AF, USARTX_TX_GPIO_PORT);
	ketCube_UART_SetupPin(USARTX_RX_PIN, USARTX_RX_AF, USARTX_RX_GPIO_PORT);
}

void vcom_IoDeInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure={0};
  
  USARTX_TX_GPIO_CLK_ENABLE();
  USARTX_RX_GPIO_CLK_ENABLE();

  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  
  GPIO_InitStructure.Pin =  USARTX_TX_PIN ;
  HAL_GPIO_Init(  USARTX_TX_GPIO_PORT, &GPIO_InitStructure );
  
	/*
  GPIO_InitStructure.Pin =  USARTX_RX_PIN ;
  HAL_GPIO_Init(  USARTX_RX_GPIO_PORT, &GPIO_InitStructure ); */
	
	__HAL_UART_ENABLE_IT(&UartHandle, UART_IT_WUF);
	HAL_UARTEx_EnableStopMode(&UartHandle); 
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
