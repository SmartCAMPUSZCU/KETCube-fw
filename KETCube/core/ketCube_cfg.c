/**
 * @file    ketCube_cfg.c
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-01-05
 * @brief   This file contains the KETCube configuration module
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

#include "ketCube_cfg.h"
#include "ketCube_eeprom.h"
#include "ketCube_common.h"
#include "ketCube_modules.h"

const char * ketCube_severity_strAlias[4] = {"NONE", "ERROR", "INFO", "DEBUG"}; ///< String representatzion of KETCube severity levels

/**
 * @brief Load configuration data from EEPROM
 * @param data pointer to an array of bytes
 * @param id module ID
 * @param addr EEPROM address
 * @param len EEPROM data len
 *
 * @retval KETCUBE_CFG_OK in case of success
 * @retval ketCube_cfg_Load_ERROR in case of failure
 */
ketCube_cfg_Error_t ketCube_cfg_Load(uint8_t * data,
                                     ketCube_cfg_moduleIDs_t id,
                                     ketCube_cfg_AllocEEPROM_t addr,
                                     ketCube_cfg_LenEEPROM_t len)
{
    if (ketCube_EEPROM_ReadBuffer
        (ketCube_modules_List[id].EEpromBase + addr, &(data[0]),
         len) == KETCUBE_EEPROM_OK) {
        return KETCUBE_CFG_OK;
    } else {
        return ketCube_cfg_Load_ERROR;
    }
}

/**
 * @brief Load configuration data from EEPROM to string
 * @param data pointer to string string
 * @param buffLen buffer len
 * @param id module ID
 * @param addr EEPROM address
 * @param len EEPROM data len
 *
 * @retval KETCUBE_CFG_OK in case of success
 * @retval ketCube_cfg_Load_ERROR in case of failure
 */
ketCube_cfg_Error_t ketCube_cfg_LoadStr(char *data, uint8_t buffLen,
                                        ketCube_cfg_moduleIDs_t id,
                                        ketCube_cfg_AllocEEPROM_t addr,
                                        ketCube_cfg_LenEEPROM_t len)
{
    uint8_t i;

    if (buffLen < (3 * len)) {
        return KETCUBE_CFG_BUFF_SMALL;
    }

    if (ketCube_cfg_Load
        ((uint8_t *) & (data[buffLen - len]), id, addr,
         len) == KETCUBE_CFG_OK) {
        for (i = 0; i < len; i++) {
            ketCube_common_Byte2hex(data[buffLen - len + i],
                                    &(data[3 * i]));
            data[3 * i + 2] = '-';
        }
        data[3 * (i - 1) + 2] = 0x00;
    } else {
        return KETCUBE_CFG_ERROR;
    }

    return KETCUBE_CFG_OK;
}

/**
 * @brief Save configuration data to EEPROM from string
 * @param data pointer to a string
 * @param id module ID
 * @param addr EEPROM address
 * @param len EEPROM data length
 *
 * @retval KETCUBE_CFG_OK in case of success
 * @retval ketCube_cfg_Load_ERROR in case of failure
 */
ketCube_cfg_Error_t ketCube_cfg_SaveStr(char *data,
                                        ketCube_cfg_moduleIDs_t id,
                                        ketCube_cfg_AllocEEPROM_t addr,
                                        ketCube_cfg_LenEEPROM_t len)
{
    if (ketCube_common_IsHexString(&(data[0]), 2 * len) == FALSE) {
        return KETCUBE_CFG_INV_HEX;
    }

    ketCube_common_Hex2Bytes((uint8_t *) & (data[0]), &(data[0]), 2 * len);

    if (ketCube_EEPROM_WriteBuffer
        (ketCube_modules_List[id].EEpromBase + addr, (uint8_t *) & (data[0]),
         len) == KETCUBE_EEPROM_OK) {
        return KETCUBE_CFG_OK;
    } else {
        return ketCube_cfg_Save_ERROR;
    }
}

/**
 * @brief Save configuration data to EEPROM
 * @param data pointer to an array of bytes
 * @param id  module ID
 * @param addr EEPROM address
 * @param len EEPROM data length
 *
 * @retval KETCUBE_CFG_OK in case of success
 * @retval ketCube_cfg_Load_ERROR in case of failure
 */
ketCube_cfg_Error_t ketCube_cfg_Save(uint8_t * data,
                                     ketCube_cfg_moduleIDs_t id,
                                     ketCube_cfg_AllocEEPROM_t addr,
                                     ketCube_cfg_LenEEPROM_t len)
{
    if (ketCube_EEPROM_WriteBuffer
        (ketCube_modules_List[id].EEpromBase + addr, &(data[0]),
         len) == KETCUBE_EEPROM_OK) {
        return KETCUBE_CFG_OK;
    } else {
        return ketCube_cfg_Save_ERROR;
    }
}

/**
 * @brief Erase configuration from EEPROM
 * 
 * @param id  module ID
 * @param addr EEPROM address
 * @param len EEPROM data length
 *
 * @retval KETCUBE_CFG_OK in case of success
 * @retval ketCube_cfg_Load_ERROR in case of failure
 */
ketCube_cfg_Error_t ketCube_cfg_SetDefaults(ketCube_cfg_moduleIDs_t id,
                                            ketCube_cfg_AllocEEPROM_t addr,
                                            ketCube_cfg_LenEEPROM_t len)
{
    if (ketCube_EEPROM_Erase
        (ketCube_modules_List[id].EEpromBase + addr,
         len) == KETCUBE_EEPROM_OK) {
        return KETCUBE_CFG_OK;
    } else {
        return ketCube_cfg_Save_ERROR;
    }
}
