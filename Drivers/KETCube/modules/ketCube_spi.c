/**
 * @file    ketCube_spi.c
 * @author  Jan Belohoubek
 * @version alpha
 * @date    2019-12-09
 * @brief   This file contains implementation of the ketCube SPI driver
 *
 * @note This code is based on Semtech and STM SPI driver implementation. 
 * See the original file licenses in LICENSE_SEMTECH and LICENSE_STM respectively.
 * 
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 University of West Bohemia in Pilsen
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

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include "hw_conf.h"

#include "ketCube_gpio.h"
#include "ketCube_spi.h"

#define SPI_CLK_ENABLE()                __HAL_RCC_SPI1_CLK_ENABLE()
#define SPI1_AF                          GPIO_AF0_SPI1  


static volatile bool initialized = FALSE; /* disable concurent execution of the init function body */

static SPI_HandleTypeDef hspi;

static void ketCube_SPI_IoInit(void);
static void ketCube_SPI_IoDeInit(void);

/**
 * @brief Calculates Spi Divisor based on Spi Frequency and Mcu Frequency
 *
 * @param [IN] Spi Frequency
 * @retval Spi divisor
 */
static uint32_t SpiFrequency(uint32_t hz);


/**
 * @brief Initializes the SPI object and MCU peripheral
 *
 * @param [IN] none
 */
ketCube_cfg_DrvError_t ketCube_SPI_Init(void) {
    if (initialized == TRUE) {
        return KETCUBE_CFG_MODULE_ERROR;
    }
  
  /*##-1- Configure the SPI peripheral */
  /* Set the SPI parameters */

  hspi.Instance = SPI1;

  hspi.Init.BaudRatePrescaler = SpiFrequency( 10000000 );
  hspi.Init.Direction      = SPI_DIRECTION_2LINES;
  hspi.Init.Mode           = SPI_MODE_MASTER;
  hspi.Init.CLKPolarity    = SPI_POLARITY_LOW;
  hspi.Init.CLKPhase       = SPI_PHASE_1EDGE;
  hspi.Init.DataSize       = SPI_DATASIZE_8BIT;
  hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;  
  hspi.Init.FirstBit       = SPI_FIRSTBIT_MSB;
  hspi.Init.NSS            = SPI_NSS_SOFT;
  hspi.Init.TIMode         = SPI_TIMODE_DISABLE;


  SPI_CLK_ENABLE(); 

  if(HAL_SPI_Init(&hspi) != HAL_OK)
  {
    /* Initialization Error */
     KETCube_ErrorHandler();
  }

  /*##-2- Configure the SPI GPIOs */
  ketCube_SPI_IoInit();
  
  initialized = TRUE;
  
  return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief De-initializes the SPI object and MCU peripheral
 *
 * @param [IN] none
 */
ketCube_cfg_DrvError_t ketCube_SPI_DeInit(void) {
    if (initialized == TRUE) {
        // UnInit here ...
        HAL_SPI_DeInit( &hspi);
        
          /*##-1- Reset peripherals ####*/
        __HAL_RCC_SPI1_FORCE_RESET();
        __HAL_RCC_SPI1_RELEASE_RESET();
        /*##-2- Configure the SPI GPIOs */
        ketCube_SPI_IoDeInit();
    }
    
    initialized = FALSE;
    
    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Set-UP SPI befere sleep enter
 * 
 * @note This function should be called by KETCube core
 * 
 */
ketCube_cfg_DrvError_t ketCube_SPI_SleepEnter(void) {
    if (initialized == TRUE) {
        ketCube_GPIO_Write(RADIO_MOSI_PORT, RADIO_MOSI_PIN, FALSE);
        ketCube_GPIO_Write(RADIO_MISO_PORT, RADIO_MISO_PIN, FALSE);
        ketCube_GPIO_Write(RADIO_SCLK_PORT, RADIO_SCLK_PIN, FALSE);
        ketCube_GPIO_Write(RADIO_NSS_PORT, RADIO_NSS_PIN , TRUE);
        
        __HAL_SPI_DISABLE(&hspi);
    }
    
    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Set-UP SPI after sleep exit
 * 
 * @note This function should be called by KETCube core
 * 
 */
ketCube_cfg_DrvError_t ketCube_SPI_SleepExit(void) {
    if (initialized == TRUE) {
        ketCube_SPI_IoInit();
        
        __HAL_SPI_ENABLE(&hspi);
    }
    
    return KETCUBE_CFG_MODULE_OK;
}

static void ketCube_SPI_IoInit(void)
{
  GPIO_InitTypeDef initStruct={0};
  
  initStruct.Mode = GPIO_MODE_AF_PP;
  initStruct.Pull = GPIO_NOPULL;
  initStruct.Speed = GPIO_SPEED_HIGH;
  initStruct.Alternate= SPI1_AF;

  ketCube_GPIO_ReInit(RADIO_SCLK_PORT, RADIO_SCLK_PIN, &initStruct);
  ketCube_GPIO_ReInit(RADIO_MISO_PORT, RADIO_MISO_PIN, &initStruct);
  ketCube_GPIO_ReInit(RADIO_MOSI_PORT, RADIO_MOSI_PIN, &initStruct);
  
  initStruct.Mode = GPIO_MODE_OUTPUT_PP;
  initStruct.Pull = GPIO_NOPULL;

  ketCube_GPIO_ReInit(RADIO_NSS_PORT, RADIO_NSS_PIN, &initStruct);
  ketCube_GPIO_Write(RADIO_NSS_PORT, RADIO_NSS_PIN, TRUE);
}

static void ketCube_SPI_IoDeInit(void) {
  GPIO_InitTypeDef initStruct = { 0 };

  initStruct.Mode = GPIO_MODE_OUTPUT_PP;
  
  initStruct.Pull = GPIO_NOPULL;
  ketCube_GPIO_ReInit(RADIO_MOSI_PORT, RADIO_MOSI_PIN, &initStruct); 
  ketCube_GPIO_Write(RADIO_MOSI_PORT, RADIO_MOSI_PIN, FALSE);
  
  initStruct.Pull = GPIO_NOPULL; 
  ketCube_GPIO_ReInit(RADIO_MISO_PORT, RADIO_MISO_PIN, &initStruct); 
  ketCube_GPIO_Write(RADIO_MISO_PORT, RADIO_MISO_PIN, FALSE);
  
  initStruct.Pull = GPIO_NOPULL;
  ketCube_GPIO_ReInit(RADIO_SCLK_PORT, RADIO_SCLK_PIN, &initStruct);
  ketCube_GPIO_Write(RADIO_SCLK_PORT, RADIO_SCLK_PIN, FALSE);

  initStruct.Pull = GPIO_NOPULL;
  ketCube_GPIO_ReInit(RADIO_NSS_PORT, RADIO_NSS_PIN , &initStruct); 
  ketCube_GPIO_Write(RADIO_NSS_PORT, RADIO_NSS_PIN , TRUE);
}

/**
 * @brief Sends outData and receives inData
 *
 * @param [IN] outData Byte to be sent
 * @retval inData      Received byte.
 */
uint16_t ketCube_SPI_InOut(uint16_t txData) {
  uint16_t rxData ;

  HAL_SPI_TransmitReceive( &hspi, ( uint8_t * ) &txData, ( uint8_t* ) &rxData, 1, HAL_MAX_DELAY);	

  return rxData;
}

static uint32_t SpiFrequency(uint32_t hz) {
  uint32_t divisor = 0;
  uint32_t SysClkTmp = SystemCoreClock;
  uint32_t baudRate;
  
  while( SysClkTmp > hz)
  {
    divisor++;
    SysClkTmp= ( SysClkTmp >> 1);
    
    if (divisor >= 7)
      break;
  }
  
  baudRate =((( divisor & 0x4 ) == 0 )? 0x0 : SPI_CR1_BR_2  )| 
            ((( divisor & 0x2 ) == 0 )? 0x0 : SPI_CR1_BR_1  )| 
            ((( divisor & 0x1 ) == 0 )? 0x0 : SPI_CR1_BR_0  );
  
  return baudRate;
}


