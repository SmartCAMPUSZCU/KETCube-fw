/**
 * @file    ketCube_i2c.c
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-01-04
 * @brief   This file contains the ketCube I2C driver
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
#include "stm32l0xx_hal_i2c.h"

#include "ketCube_cfg.h"
#include "ketCube_i2c.h"
#include "ketCube_terminal.h"

// local fn declarations
I2C_HandleTypeDef KETCUBE_I2C_Handle;
static void ketCube_I2C_Error();

static uint8_t initRuns = 0;    //< This driver can be initialized in number of modules. If 0 == not initialized, else initialized

/**
 * @brief  Configures I2C interface.
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_I2C_Init(void)
{
    initRuns += 1;

    if (initRuns > 1) {
        return KETCUBE_CFG_MODULE_OK;
    }

    if (HAL_I2C_GetState(&KETCUBE_I2C_Handle) == HAL_I2C_STATE_RESET) {

        /* I2C peripheral configuration */
        KETCUBE_I2C_Handle.Init.Timing = KETCUBE_I2C_SPEED_100KHZ;      /* 100KHz */

        KETCUBE_I2C_Handle.Init.OwnAddress1 = KETCUBE_I2C_ADDRESS;
        KETCUBE_I2C_Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
        KETCUBE_I2C_Handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
        KETCUBE_I2C_Handle.Instance = KETCUBE_I2C_HANDLE;

        /* Init the I2C */
        GPIO_InitTypeDef GPIO_InitStruct;
        /* Enable I2C GPIO clocks */
        KETCUBE_I2C_SCL_SDA_GPIO_CLK_ENABLE();

        /* I2C_EXPBD SCL and SDA pins configuration ------------------------------------- */
        GPIO_InitStruct.Pin = KETCUBE_I2C_SCL_PIN | KETCUBE_I2C_SDA_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Alternate = KETCUBE_I2C_SCL_SDA_AF;

        HAL_GPIO_Init(KETCUBE_I2C_SCL_SDA_GPIO_PORT, &GPIO_InitStruct);

        /* Enable the I2C_EXPBD peripheral clock */
        KETCUBE_I2C_CLK_ENABLE();
        /* Force the I2C peripheral clock reset */
        KETCUBE_I2C_FORCE_RESET();

        /* Release the I2C peripheral clock reset */
        KETCUBE_I2C_RELEASE_RESET();

        /* Enable and set I2C_EXPBD Interrupt to the highest priority */
        HAL_NVIC_SetPriority(KETCUBE_I2C_EV_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(KETCUBE_I2C_EV_IRQn);

        HAL_I2C_Init(&KETCUBE_I2C_Handle);
    }

    if (HAL_I2C_GetState(&KETCUBE_I2C_Handle) == HAL_I2C_STATE_READY) {
        return KETCUBE_CFG_MODULE_OK;
    } else {
        return KETCUBE_CFG_MODULE_ERROR;
    }
}

/**
 * @brief  Configures I2C interface.
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_I2C_UnInit(void)
{
    if (initRuns > 1) {
        initRuns -= 1;
        return KETCUBE_CFG_MODULE_OK;
    } else if (initRuns == 0) {
        // UnInit here ...
        HAL_I2C_DeInit(&KETCUBE_I2C_Handle);
        return KETCUBE_CFG_MODULE_OK;
    }
    // Run UnInit body once only: (initRuns == 1)
    initRuns -= 1;

    return KETCUBE_CFG_MODULE_OK;
}

uint8_t ketCube_I2C_ReadData(uint8_t Addr, uint8_t Reg, uint8_t * pBuffer,
                             uint16_t Size)
{

    HAL_StatusTypeDef status = HAL_OK;

    status =
        HAL_I2C_Mem_Read(&KETCUBE_I2C_Handle, Addr, (uint16_t) Reg,
                         I2C_MEMADD_SIZE_8BIT, pBuffer, Size,
                         KETCUBE_I2C_TIMEOUT);

    /* Check the communication status */
    if (status == HAL_OK) {
        return 0;
    } else if (status == HAL_ERROR) {
        ketCube_I2C_Error();
        return 1;
    } else {
        return 1;
    }
}

uint8_t ketCube_I2C_WriteData(uint8_t Addr, uint8_t Reg, uint8_t * pBuffer,
                              uint16_t Size)
{

    HAL_StatusTypeDef status = HAL_OK;

    status =
        HAL_I2C_Mem_Write(&KETCUBE_I2C_Handle, Addr, (uint16_t) Reg,
                          I2C_MEMADD_SIZE_8BIT, pBuffer, Size,
                          KETCUBE_I2C_TIMEOUT);

    /* Check the communication status */
    if (status == HAL_OK) {
        return 0;
    } else if (status == HAL_ERROR) {
        ketCube_I2C_Error();
        return 1;
    } else {
        return 1;
    }
}


uint8_t ketCube_I2C_WriteRawData(uint8_t Addr, uint8_t * pBuffer, uint16_t Size)
{
    HAL_StatusTypeDef status = HAL_OK;
    
    status =
        HAL_I2C_Master_Transmit(&KETCUBE_I2C_Handle, Addr, pBuffer, Size,
                                KETCUBE_I2C_TIMEOUT);

    /* Check the communication status */
    if (status == HAL_OK) {
        return 0;
    } else if (status == HAL_ERROR) {
        ketCube_I2C_Error();
        return 1;
    } else {
        return 1;
    }
}

uint8_t ketCube_I2C_ReadRawData(uint8_t Addr, uint8_t * pBuffer, uint16_t Size)
{
    HAL_StatusTypeDef status = HAL_OK;
    
    status =
        HAL_I2C_Master_Receive(&KETCUBE_I2C_Handle, Addr, pBuffer, Size,
                               KETCUBE_I2C_TIMEOUT);

    /* Check the communication status */
    if (status == HAL_OK) {
        return 0;
    } else if (status == HAL_ERROR) {
        ketCube_I2C_Error();
        return 1;
    } else {
        return 1;
    }
}

/**
 * @brief  Manages error callback by re-initializing I2C
 * @retval None
 */
static void ketCube_I2C_Error()
{
    uint8_t i;
    uint8_t tmpInitRuns = initRuns;
    
    ketCube_terminal_DebugPrintln("I2C :: Re-Initialize()");
    
    /* De-initialize the I2C comunication bus */
    for (i = tmpInitRuns; i > 0; i--) {
        ketCube_I2C_UnInit(); 
    }
    /* De-initialize the I2C comunication bus */
    // HAL_I2C_DeInit(&KETCUBE_I2C_Handle);

    /* Re-Initiaize the I2C comunication bus */
    for (i = tmpInitRuns; i > 0; i--) {
        ketCube_I2C_Init(); 
    }
}

/**
 * @brief  Read TexasInstruments I2C periph 16-bit register
 * @param  devAddr I2C Address
 * @param  regAddr register address
 * @param  data pointer to 16-bit value
 * 
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_I2C_TexasReadReg(uint8_t devAddr,
                                                uint8_t regAddr,
                                                uint16_t * data)
{
    uint8_t buffer[2];

    HAL_StatusTypeDef status =
        HAL_I2C_Master_Transmit(&KETCUBE_I2C_Handle, devAddr, &(regAddr),
                                1, KETCUBE_I2C_TIMEOUT);
    if (status != HAL_OK) {
        return KETCUBE_CFG_MODULE_ERROR;
    }
    HAL_Delay(50);
    status =
        HAL_I2C_Master_Receive(&KETCUBE_I2C_Handle, devAddr, &(buffer[0]),
                               2, KETCUBE_I2C_TIMEOUT);
    if (status != HAL_OK) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    *data = (((uint16_t) buffer[0]) << 8) | buffer[1];

    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief  Write TexasInstruments I2C periph 16-bit register
 * @param  devAddr I2C Address
 * @param  regAddr register address
 * @param  data pointer to 16-bit value
 * 
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_I2C_TexasWriteReg(uint8_t devAddr,
                                                 uint8_t regAddr,
                                                 uint16_t * data)
{
    uint8_t buffer[2];
    buffer[0] = (uint8_t) ((*data >> 8) & 0x00FF);
    buffer[1] = (uint8_t) (*data & 0x00FF);

    if (ketCube_I2C_WriteData(devAddr, regAddr, &(buffer[0]), 2)) {
        return KETCUBE_CFG_MODULE_ERROR;
    } else {
        return KETCUBE_CFG_MODULE_OK;
    }
}

/**
 * @brief  Read STM I2C Single 8-bit register
 * @param  devAddr I2C Address
 * @param  regAddr register address
 * @param  data pointer to 8-bit value
 * @param  try # of tries when I2C failed
 * 
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_I2C_STMReadSingle(uint8_t devAddr,
                                                 uint8_t regAddr,
                                                 uint8_t * data,
                                                 uint8_t try)
{
    regAddr = regAddr & (~0x80);

    while (try > 0) {
        HAL_StatusTypeDef status =
            HAL_I2C_Master_Transmit(&KETCUBE_I2C_Handle, devAddr,
                                    &(regAddr),
                                    1, KETCUBE_I2C_TIMEOUT);
        if (status != HAL_OK) {
            try--;
            continue;
        }
        //HAL_Delay(1);
        status =
            HAL_I2C_Master_Receive(&KETCUBE_I2C_Handle, devAddr, data,
                                   1, KETCUBE_I2C_TIMEOUT);
        if (status == HAL_OK) {
            return KETCUBE_CFG_MODULE_OK;
        }
        try--;
    }

    return KETCUBE_CFG_MODULE_ERROR;
}

/**
 * @brief  Write AnalogDevices I2C periph 8-bit register
 * @param  devAddr I2C Address
 * @param  regAddr register address
 * @param  data 8-bit register value
 * 
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_I2C_AnalogWriteReg(uint8_t devAddr,
                                                  uint8_t regAddr,
                                                  uint8_t data)
{
    if (ketCube_I2C_WriteData(devAddr, regAddr, &(data), 1)) {
        return KETCUBE_CFG_MODULE_ERROR;
    } else {
        return KETCUBE_CFG_MODULE_OK;
    }
}

/**
 * @brief  Read STM I2C Single 8-bit register
 * @param  devAddr I2C Address
 * @param  regAddr register address
 * @param  data pointer to 8-bit value
 * @param  len data length to read
 * @param  try # of tries when I2C failed
 * 
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_I2C_STMReadBlock(uint8_t devAddr,
                                                uint8_t regAddr,
                                                uint8_t * data,
                                                uint8_t len, uint8_t try)
{
    regAddr = regAddr & (~0x80);

    while (try > 0) {
        HAL_StatusTypeDef status =
            HAL_I2C_Master_Transmit(&KETCUBE_I2C_Handle, devAddr,
                                    &(regAddr),
                                    1, KETCUBE_I2C_TIMEOUT);
        if (status != HAL_OK) {
            try--;
            continue;
        }
        //HAL_Delay(1);
        status =
            HAL_I2C_Master_Receive(&KETCUBE_I2C_Handle, devAddr, data,
                                   len, KETCUBE_I2C_TIMEOUT);
        if (status == HAL_OK) {
            return KETCUBE_CFG_MODULE_OK;
        }
        try--;
    }

    return KETCUBE_CFG_MODULE_ERROR;
}

/**
 * @brief  Write STM I2C Single 8-bit register
 * @param  devAddr I2C Address
 * @param  regAddr register address
 * @param  data pointer to 8-bit value
 * @param  try # of tries when I2C failed
 * 
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_I2C_STMWriteSingle(uint8_t devAddr,
                                                  uint8_t regAddr,
                                                  uint8_t * data,
                                                  uint8_t try)
{
    while (try > 0) {
        if (ketCube_I2C_WriteData(devAddr, (regAddr & (~0x80)), data, 1)) {
            try--;
        } else {
            return KETCUBE_CFG_MODULE_OK;
        }
    }
    return KETCUBE_CFG_MODULE_ERROR;
}
