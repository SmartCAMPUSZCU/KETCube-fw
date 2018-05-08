/**
 * @file    ketCube_eeprom.c
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-01-05
 * @brief   This file contains the ketCube EEPROM driver
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

#include "stdint.h"
#include "ketCube_eeprom.h"
#include "stm32l0xx_hal.h"
#include "ketCube_terminal.h"
#include "vcom.h"

/**
  * @brief Unlock the EEPROM
  *
  * @retval error code, KETCUBE_EEPROM_OK if success
  *
  */
ketCube_EEPROM_Error_t EEPROM_Unlock(void)
{
    uint32_t tickstart = 0U;

    tickstart = HAL_GetTick();
    /* Wait for FLASH to be free */
    while ((FLASH->SR & FLASH_SR_BSY) != 0) {
        if ((tickstart == 0U)
            || ((HAL_GetTick() - tickstart) > KETCUBE_EEPROM_TIMEOUT)) {
            return ketCube_EEPROM_Error_tIMEOUT;
        }
    }

    /* If PELOCK is locked */
    if ((FLASH->PECR & FLASH_PECR_PELOCK) != 0) {
        FLASH->PEKEYR = FLASH_PEKEY1;
        FLASH->PEKEYR = FLASH_PEKEY2;
    }

    /* enable flash interrupts */
    FLASH->PECR = FLASH->PECR | (FLASH_PECR_ERRIE | FLASH_PECR_EOPIE);

    return KETCUBE_EEPROM_OK;
}


/**
  * @brief Lock the EEPROM
  *
  * @retval error code, KETCUBE_EEPROM_OK if success
  *
  */
ketCube_EEPROM_Error_t EEPROM_Lock(void)
{
    uint32_t tickstart = 0U;

    tickstart = HAL_GetTick();
    /* Wait for FLASH to be free */
    while ((FLASH->SR & FLASH_SR_BSY) != 0) {
        if ((tickstart == 0U)
            || ((HAL_GetTick() - tickstart) > KETCUBE_EEPROM_TIMEOUT)) {
            return ketCube_EEPROM_Error_tIMEOUT;
        }
    }
    FLASH->PECR = FLASH->PECR & ~(FLASH_PECR_ERRIE | FLASH_PECR_EOPIE); /* disable flash interrupts */
    FLASH->PECR = FLASH->PECR | FLASH_PECR_PELOCK;      /* Lock memory with PELOCK */

    return KETCUBE_EEPROM_OK;
}

/**
  * @brief Write the EEPROM word
  *
  * @param addr EEPROM offset (from the base address)
  * @param data buffer to be written
  *
  * @retval error code, KETCUBE_EEPROM_OK if success
  *
  */
ketCube_EEPROM_Error_t EEPROM_Write(uint32_t addr, uint8_t data)
{
    uint32_t tickstart = 0U;

    if ((KETCUBE_EEPROM_BASE_ADDR + addr) > KETCUBE_EEPROM_END_ADDR) {
        return KETCUBE_EEPROM_ERROR_MEMOVER;
    }

    if (*((uint8_t *) (KETCUBE_EEPROM_BASE_ADDR + addr)) == data) {
        return KETCUBE_EEPROM_OK;
    }

    tickstart = HAL_GetTick();
    *((uint8_t *) (addr + KETCUBE_EEPROM_BASE_ADDR)) = data;    /* write data to EEPROM */
    //__WFI();
    while (*((uint8_t *) (addr + KETCUBE_EEPROM_BASE_ADDR)) != data) {
        if ((tickstart == 0U)
            || ((HAL_GetTick() - tickstart) > KETCUBE_EEPROM_TIMEOUT)) {
            return KETCUBE_EEPROM_ERROR;
        }
    }

    return KETCUBE_EEPROM_OK;
}

/**
  * @brief Read the EEPROM word
  *
  * @param addr EEPROM offset (from the base address)
  * @param data to data to read
  *
  * @retval KETCUBE_EEPROM_OK if success
  * @retval KETCUBE_EEPROM_ERROR_MEMOVER if address out of memory
  *
  */
ketCube_EEPROM_Error_t EEPROM_Read(uint32_t addr, uint8_t * data)
{
    if ((KETCUBE_EEPROM_BASE_ADDR + addr) > KETCUBE_EEPROM_END_ADDR) {
        return KETCUBE_EEPROM_ERROR_MEMOVER;
    }

    *data = *((uint8_t *) (addr + KETCUBE_EEPROM_BASE_ADDR));

    return KETCUBE_EEPROM_OK;
}

/**
  * @brief Write Buffer to the EEPROM
  *
  * @param addr EEPROM offset (from the base address)
  * @param data buffer to be written
  * @param len data buffer length
  *
  * @retval error code, KETCUBE_EEPROM_OK if success
  *
  */
ketCube_EEPROM_Error_t ketCube_EEPROM_WriteBuffer(uint32_t addr,
                                                  uint8_t * data,
                                                  uint8_t len)
{
    ketCube_EEPROM_Error_t ret;
    uint8_t i;

    if ((KETCUBE_EEPROM_BASE_ADDR + addr) > KETCUBE_EEPROM_END_ADDR) {
        return KETCUBE_EEPROM_ERROR_MEMOVER;
    }

    for (i = 0; i < len; i++) {
        EEPROM_Unlock();

        /* Reset the ERASE and DATA bits in the FLASH_PECR register to disable any residual erase */
        FLASH->PECR = FLASH->PECR & ~(FLASH_PECR_ERASE | FLASH_PECR_DATA);

        ret = EEPROM_Write(addr + i, *(data + i));
        if (ret != KETCUBE_EEPROM_OK) {
            return ret;
        }

        /* Lock the EEPROM */
        EEPROM_Lock();
    }

    return KETCUBE_EEPROM_OK;
}

/**
  * @brief Read Buffer to the EEPROM
  *
  * @param addr EEPROM offset (from the base address)
  * @param data buffer to be written
  * @param len data buffer length
  *
  * @retval error code, KETCUBE_EEPROM_OK if success
  *
  */
ketCube_EEPROM_Error_t ketCube_EEPROM_ReadBuffer(uint32_t addr,
                                                 uint8_t * data,
                                                 uint8_t len)
{
    ketCube_EEPROM_Error_t ret;
    uint8_t i;

    if ((KETCUBE_EEPROM_BASE_ADDR + addr) > KETCUBE_EEPROM_END_ADDR) {
        return KETCUBE_EEPROM_ERROR_MEMOVER;
    }

    for (i = 0; i < len; i++) {
        ret = EEPROM_Read(addr + i, (uint8_t *) (data + i));
        if (ret != KETCUBE_EEPROM_OK) {
            return ret;
        }
    }

    return KETCUBE_EEPROM_OK;
}
