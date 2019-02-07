/**
 * @file    ketCube_lora_cmd.c
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-04-27
 * @brief   This file contains the KETCube module commandline deffinition
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

 /* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __KETCUBE_LORA_CMD_H
#define __KETCUBE_LORA_CMD_H

#include "ketCube_cfg.h"
#include "ketCube_common.h"
#include "ketCube_lora.h"

/** @defgroup KETCube_LoRa_CMD KETCube LoRa CMD
  * @brief KETCube LoRa module commandline deffinition
  * @ingroup KETCube_Terminal
  * @{
  */

/**
 * @brief Load current EEPROM Settings
 * 
 * @param data buffer
 * @param offset offset in the configuration struct
 * @param size size of data
 * 
 * @retval err TRUE if no error, else return FALSE
 * 
 */
bool ketCube_LoRa_cmd_loadEEPROM(uint8_t *data, uint8_t offset, uint8_t size)
{
    if (ketCube_cfg_Load((uint8_t *) &(data[0]),
                         KETCUBE_LISTS_MODULEID_LORA,
                         (ketCube_cfg_AllocEEPROM_t) offset,
                         (ketCube_cfg_LenEEPROM_t) size) == KETCUBE_CFG_OK) {
        return TRUE;
    } 
    
    return FALSE;
}

/**
 * @brief Save new EEPROM Settings for LoRa module
 * 
 * @param data buffer
 * @param offset offset in the configuration struct
 * @param size size of data
 * 
 * @retval err TRUE if no error, else return FALSE
 * 
 */
bool ketCube_LoRa_cmd_saveEEPROM(uint8_t *data, uint8_t offset, uint8_t size)
{
    if (ketCube_cfg_Save((uint8_t *) &(data[0]),
                         KETCUBE_LISTS_MODULEID_LORA,
                         (ketCube_cfg_AllocEEPROM_t) offset,
                         (ketCube_cfg_LenEEPROM_t) size) == KETCUBE_CFG_OK) {
        return TRUE;
    } 
    
    return FALSE;
}


/**
 * @brief Is ABP Enabled ? callback
 * 
 */
void ketCube_LoRa_cmd_show_ABP(void)
{
    ketCube_lora_bitCfg_t bitCfg;
    
    if (ketCube_LoRa_cmd_loadEEPROM((uint8_t *) &bitCfg, 
        offsetof(ketCube_lora_moduleCfg_t, bitCfg),
        sizeof(bitCfg))) {
        if (bitCfg.connectionType == KETCUBE_LORA_SELCONNMETHOD_ABP) {
            commandIOParams.as_integer = 1;
        } else {
            commandIOParams.as_integer = 0;
        }
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

/**
 * @brief Is OTAA Enabled ? callback
 * 
 */
void ketCube_LoRa_cmd_show_OTAA(void)
{
    ketCube_lora_bitCfg_t bitCfg;
    
    if (ketCube_LoRa_cmd_loadEEPROM((uint8_t *) &bitCfg, 
        offsetof(ketCube_lora_moduleCfg_t, bitCfg),
        sizeof(bitCfg))){
        if (bitCfg.connectionType == KETCUBE_LORA_SELCONNMETHOD_OTAA) {
            commandIOParams.as_integer = 1;
        } else {
            commandIOParams.as_integer = 0;
        }
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

// Declaring extern function HW_GetUniqueId()
extern void HW_GetUniqueId(uint8_t * id);

/**
 * @brief Show devEUI command callback
 * 
 */
void ketCube_LoRa_cmd_show_devEUI(void)
{
    ketCube_lora_bitCfg_t bitCfg;
    
    if (!ketCube_LoRa_cmd_loadEEPROM((uint8_t *) &bitCfg, 
        offsetof(ketCube_lora_moduleCfg_t, bitCfg),
        sizeof(bitCfg))) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
        return;
    }

    if (bitCfg.devEUIType == KETCUBE_LORA_SELDEVEUI_BOARD) {
        HW_GetUniqueId((uint8_t *) &(commandIOParams.as_byte_array.data[0]));
        commandIOParams.as_byte_array.length = KETCUBE_LORA_CFGLEN_DEVEUI;
    } else {
        if (ketCube_LoRa_cmd_loadEEPROM((uint8_t *) &(commandIOParams.as_byte_array.data[0]), 
        offsetof(ketCube_lora_moduleCfg_t, devEUI),
        KETCUBE_LORA_CFGLEN_DEVEUI)) {
            commandIOParams.as_byte_array.length = KETCUBE_LORA_CFGLEN_DEVEUI;
        } else {
            commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
        }
    }
}


/**
 * @brief Show LoRa devEUI type callback
 * 
 */
void ketCube_LoRa_cmd_show_devEUIType(void)
{
    const char* type = "Unknown";
    ketCube_lora_bitCfg_t bitCfg;
    
    if (!ketCube_LoRa_cmd_loadEEPROM((uint8_t *) &bitCfg, 
        offsetof(ketCube_lora_moduleCfg_t, bitCfg),
        sizeof(bitCfg))) {
        if (bitCfg.devEUIType == KETCUBE_LORA_SELDEVEUI_CUSTOM) {
            type = "Custom";
        } else {
            type = "Board";
        }
        
        snprintf(commandIOParams.as_string, KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH,
                 "%s", type);
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

/**
 * @brief Show appEUI command callback
 * 
 */
void ketCube_LoRa_cmd_show_appEUI(void)
{
    if (ketCube_LoRa_cmd_loadEEPROM((uint8_t *) &(commandIOParams.as_byte_array.data[0]), 
        offsetof(ketCube_lora_moduleCfg_t, appEUI),
        KETCUBE_LORA_CFGLEN_APPEUI)) {
        
        commandIOParams.as_byte_array.length = KETCUBE_LORA_CFGLEN_APPEUI;
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

/**
 * @brief Show appKey command callback
 * 
 */
void ketCube_LoRa_cmd_show_appKey(void)
{
    if (ketCube_LoRa_cmd_loadEEPROM((uint8_t *) &(commandIOParams.as_byte_array.data[0]), 
        offsetof(ketCube_lora_moduleCfg_t, appKey),
        KETCUBE_LORA_CFGLEN_APPKEY)) {
        
        commandIOParams.as_byte_array.length = KETCUBE_LORA_CFGLEN_APPKEY;
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}


/**
 * @brief Show devAddr command callback
 * 
 */
void ketCube_LoRa_cmd_show_devAddr(void)
{
    if (ketCube_LoRa_cmd_loadEEPROM((uint8_t *) &(commandIOParams.as_byte_array.data[0]), 
        offsetof(ketCube_lora_moduleCfg_t, devAddr),
        KETCUBE_LORA_CFGLEN_DEVADDR)) {
        
        commandIOParams.as_byte_array.length = KETCUBE_LORA_CFGLEN_DEVADDR;
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

/**
 * @brief Show appSKey command callback
 * 
 */
void ketCube_LoRa_cmd_show_appSKey(void)
{
    if (ketCube_LoRa_cmd_loadEEPROM((uint8_t *) &(commandIOParams.as_byte_array.data[0]), 
        offsetof(ketCube_lora_moduleCfg_t, appSKey),
        KETCUBE_LORA_CFGLEN_APPSKEY)) {
        
        commandIOParams.as_byte_array.length = KETCUBE_LORA_CFGLEN_APPSKEY;
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

/**
 * @brief Show appSKey command callback
 * 
 */
void ketCube_LoRa_cmd_show_nwkSKey(void)
{
    if (ketCube_LoRa_cmd_loadEEPROM((uint8_t *) &(commandIOParams.as_byte_array.data[0]), 
        offsetof(ketCube_lora_moduleCfg_t, nwkSKey),
        KETCUBE_LORA_CFGLEN_NWKSKEY)) {
        
        commandIOParams.as_byte_array.length = KETCUBE_LORA_CFGLEN_NWKSKEY;
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}


/**
 * @brief Set ABP command callback
 *
 */
void ketCube_LoRa_cmd_set_ABP(void)
{
    ketCube_lora_bitCfg_t bitCfg;
    
    if (!ketCube_LoRa_cmd_loadEEPROM((uint8_t *) &bitCfg, 
        offsetof(ketCube_lora_moduleCfg_t, bitCfg),
        sizeof(bitCfg))) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
        return;
    }
    
    bitCfg.connectionType = KETCUBE_LORA_SELCONNMETHOD_ABP;
    
    if (!ketCube_LoRa_cmd_saveEEPROM((uint8_t *) &bitCfg, 
        offsetof(ketCube_lora_moduleCfg_t, bitCfg),
        sizeof(bitCfg))) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

/**
 * @brief Set OTAA command callback
 *
 */
void ketCube_LoRa_cmd_set_OTAA(void)
{
    ketCube_lora_bitCfg_t bitCfg;
    
    if (!ketCube_LoRa_cmd_loadEEPROM((uint8_t *) &bitCfg, 
        offsetof(ketCube_lora_moduleCfg_t, bitCfg),
        sizeof(bitCfg))) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
        return;
    }
    
    bitCfg.connectionType = KETCUBE_LORA_SELCONNMETHOD_OTAA;
    
    if (!ketCube_LoRa_cmd_saveEEPROM((uint8_t *) &bitCfg, 
        offsetof(ketCube_lora_moduleCfg_t, bitCfg),
        sizeof(bitCfg))) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

/**
 * @brief Set appEUI command callback
 *
 */
void ketCube_LoRa_cmd_set_appEUI(void)
{
    if (!ketCube_LoRa_cmd_saveEEPROM((uint8_t *) &(commandIOParams.as_byte_array.data[0]), 
        offsetof(ketCube_lora_moduleCfg_t, appEUI),
        KETCUBE_LORA_CFGLEN_APPEUI)) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

/**
 * @brief Set appKey command callback
 *
 */
void ketCube_LoRa_cmd_set_appKey(void)
{
    if (!ketCube_LoRa_cmd_saveEEPROM((uint8_t *) &(commandIOParams.as_byte_array.data[0]), 
        offsetof(ketCube_lora_moduleCfg_t, appKey),
        KETCUBE_LORA_CFGLEN_APPKEY)) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

/**
 * @brief Set devEUI command callback
 *
 */
void ketCube_LoRa_cmd_set_devEUI(void)
{
    if (!ketCube_LoRa_cmd_saveEEPROM((uint8_t *) &(commandIOParams.as_byte_array.data[0]), 
        offsetof(ketCube_lora_moduleCfg_t, devEUI),
        KETCUBE_LORA_CFGLEN_DEVEUI)) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

/**
 * @brief Set custom devEUI command callback
 *
 */
void ketCube_LoRa_cmd_set_devEUICustom(void)
{
    ketCube_lora_bitCfg_t bitCfg;
    
    if (!ketCube_LoRa_cmd_loadEEPROM((uint8_t *) &bitCfg, 
        offsetof(ketCube_lora_moduleCfg_t, bitCfg),
        sizeof(bitCfg))) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
        return;
    }
    
    bitCfg.devEUIType = KETCUBE_LORA_SELDEVEUI_CUSTOM;
    
    if (!ketCube_LoRa_cmd_saveEEPROM((uint8_t *) &bitCfg, 
        offsetof(ketCube_lora_moduleCfg_t, bitCfg),
        sizeof(bitCfg))) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

/**
 * @brief Set board devEUI command callback
 *
 */
void ketCube_LoRa_cmd_set_devEUIBoard(void)
{
    ketCube_lora_bitCfg_t bitCfg;
    
    if (!ketCube_LoRa_cmd_loadEEPROM((uint8_t *) &bitCfg, 
        offsetof(ketCube_lora_moduleCfg_t, bitCfg),
        sizeof(bitCfg))) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
        return;
    }
    
    bitCfg.devEUIType = KETCUBE_LORA_SELDEVEUI_BOARD;
    
    if (!ketCube_LoRa_cmd_saveEEPROM((uint8_t *) &bitCfg, 
        offsetof(ketCube_lora_moduleCfg_t, bitCfg),
        sizeof(bitCfg))) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

/**
 * @brief Set devAddr command callback
 *
 */
void ketCube_LoRa_cmd_set_devAddr(void)
{
     if (!ketCube_LoRa_cmd_saveEEPROM((uint8_t *) &(commandIOParams.as_byte_array.data[0]), 
        offsetof(ketCube_lora_moduleCfg_t, devAddr),
        KETCUBE_LORA_CFGLEN_DEVADDR)) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

/**
 * @brief Set appSKey command callback
 *
 */
void ketCube_LoRa_cmd_set_appSKey(void)
{
     if (!ketCube_LoRa_cmd_saveEEPROM((uint8_t *) &(commandIOParams.as_byte_array.data[0]), 
        offsetof(ketCube_lora_moduleCfg_t, appSKey),
        KETCUBE_LORA_CFGLEN_APPSKEY)) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

/**
 * @brief Set nwkSKey command callback
 *
 */
void ketCube_LoRa_cmd_set_nwkSKey(void)
{
     if (!ketCube_LoRa_cmd_saveEEPROM((uint8_t *) &(commandIOParams.as_byte_array.data[0]), 
        offsetof(ketCube_lora_moduleCfg_t, nwkSKey),
        KETCUBE_LORA_CFGLEN_NWKSKEY)) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}


/* Terminal command definitions */

ketCube_terminal_cmd_t ketCube_terminal_commands_show_LoRa[] = {
    DEF_COMMAND("ABP",
                "Is ABP enabled?",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_INTEGER,
                &ketCube_LoRa_cmd_show_ABP),
    DEF_COMMAND("appEUI",
                "Show LoRa application EUI.",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                &ketCube_LoRa_cmd_show_appEUI),
    DEF_COMMAND("appKey",
                "Show LoRa application key.",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                &ketCube_LoRa_cmd_show_appKey),
    DEF_COMMAND("appSKey",
                "Show LoRa app session Key.",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                &ketCube_LoRa_cmd_show_appSKey),
    DEF_COMMAND("devAddr",
                "Show LoRa device address.",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                &ketCube_LoRa_cmd_show_devAddr),
    DEF_COMMAND("devEUI",
                "Show LoRa device EUI.",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                &ketCube_LoRa_cmd_show_devEUI),
    DEF_COMMAND("devEUIType",
                "Show LoRa device EUI type: custom (user-defined) "
                "or deviceID-based.",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_STRING,
                &ketCube_LoRa_cmd_show_devEUIType),
    DEF_COMMAND("OTAA",
                "Is OTAA enabled?",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_INTEGER,
                &ketCube_LoRa_cmd_show_OTAA),
    DEF_COMMAND("nwkSKey",
                "Show LoRa network session Key.",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                &ketCube_LoRa_cmd_show_nwkSKey),
    DEF_TERMINATE()
};

ketCube_terminal_cmd_t ketCube_terminal_commands_set_LoRa[] = {
    DEF_COMMAND("ABP",
                "Enable ABP",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_LoRa_cmd_set_ABP),
    DEF_COMMAND("appEUI",
                "Set LoRa application EUI",
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                &ketCube_LoRa_cmd_set_appEUI),
    DEF_COMMAND("appKey",
                "Set LoRa application key",
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                &ketCube_LoRa_cmd_set_appKey),
    DEF_COMMAND("appSKey",
                "Set LoRa application session key",
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                &ketCube_LoRa_cmd_set_appSKey),
    DEF_COMMAND("devAddr",
                "Set LoRa device address",
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                &ketCube_LoRa_cmd_set_devAddr),
    DEF_COMMAND("devEUI",
                "Set LoRa device EUI",
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                &ketCube_LoRa_cmd_set_devEUI),
    DEF_COMMAND("devEUICustom",
                "Use custom (user-defined) LoRa device EUI",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_LoRa_cmd_set_devEUICustom),
    DEF_COMMAND("devEUIBoard",
                "Use board (boardID-based) LoRa device EUI",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_LoRa_cmd_set_devEUIBoard),
    DEF_COMMAND("OTAA",
                "Enable OTAA",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_LoRa_cmd_set_OTAA),
    DEF_COMMAND("nwkSKey",
                "Set LoRa network session key",
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
                &ketCube_LoRa_cmd_set_nwkSKey),
    DEF_TERMINATE()
};


/**
* @}
*/

#endif                          /* KETCUBE_LORA_CMD_H */
