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
 * @brief Is ABP Enabled ? callback
 * 
 */
void ketCube_LoRa_cmd_show_ABP(void)
{
    ketCube_lora_moduleCfg_t data;

    if (ketCube_cfg_Load
        ((uint8_t *) & data, KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) ==
        KETCUBE_CFG_OK) {
        if (data.connectionType == KETCUBE_LORA_SELCONNMETHOD_ABP) {
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
    ketCube_lora_moduleCfg_t data;

    if (ketCube_cfg_Load
        ((uint8_t *) & data, KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) ==
        KETCUBE_CFG_OK) {
        if (data.connectionType == KETCUBE_LORA_SELCONNMETHOD_OTAA) {
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
    char data[3 * KETCUBE_LORA_CFGLEN_DEVEUI];
    ketCube_lora_moduleCfg_t type;
    uint8_t i;

    if (ketCube_cfg_Load
        ((uint8_t *) & (type), KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) !=
        KETCUBE_CFG_OK) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
        return;
    }    

    if (type.devEUIType == KETCUBE_LORA_SELDEVEUI_BOARD) {
        HW_GetUniqueId((uint8_t *) & (data[0]));
        for (i = 0; i < KETCUBE_LORA_CFGLEN_DEVEUI; i++) {
            ketCube_common_Byte2hex(data
                                    [KETCUBE_LORA_CFGLEN_DEVEUI - i - 1],
                                    &(data
                                      [3 * KETCUBE_LORA_CFGLEN_DEVEUI -
                                       (3 * (i + 1))]));

            data[3 * KETCUBE_LORA_CFGLEN_DEVEUI - (3 * (i + 1)) + 2] = '-';
        }
        data[3 * KETCUBE_LORA_CFGLEN_DEVEUI - 1] = 0x00;
    } else {
        if (ketCube_cfg_LoadStr
            ((char *) &(data[0]), 3 * KETCUBE_LORA_CFGLEN_DEVEUI,
             KETCUBE_LISTS_MODULEID_LORA,
             (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_DEVEUI,
             (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_DEVEUI) ==
            KETCUBE_CFG_OK) {
        } else {
            commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
            return;
        }
    }
    
    snprintf(commandIOParams.as_string, KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH,
                            "%s",
                            &(data[0]));
}


/**
 * @brief Show LoRa devEUI type callback
 * 
 */
void ketCube_LoRa_cmd_show_devEUIType(void)
{
    ketCube_lora_moduleCfg_t data;
    
    const char* type = "Unknown";

    if (ketCube_cfg_Load
        ((uint8_t *) & (data), KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) ==
        KETCUBE_CFG_OK) {
        if (data.devEUIType == KETCUBE_LORA_SELDEVEUI_CUSTOM) {
            type = "Custom";
        } else {
            type = "Board";
        }
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
        return;
    }
    
    snprintf(commandIOParams.as_string, KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH,
                            "%s",
                            type);
}

/**
 * @brief Show appEUI command callback
 * 
 */
void ketCube_LoRa_cmd_show_appEUI(void)
{
    uint8_t data[3 * KETCUBE_LORA_CFGLEN_APPEUI];

    if (ketCube_cfg_LoadStr
        ((char *) &(data[0]), 3 * KETCUBE_LORA_CFGLEN_APPEUI,
         KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_APPEUI,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_APPEUI) ==
        KETCUBE_CFG_OK) {
            
        snprintf(commandIOParams.as_string,
                 KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH,
                 "%s",
                 &(data[0]));
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
    uint8_t data[3 * KETCUBE_LORA_CFGLEN_APPKEY];

    if (ketCube_cfg_LoadStr
        ((char *) &(data[0]), 3 * KETCUBE_LORA_CFGLEN_APPKEY,
         KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_APPKEY,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_APPKEY) ==
        KETCUBE_CFG_OK) {
            
        snprintf(commandIOParams.as_string,
                 KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH,
                 "%s",
                 &(data[0]));
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
    uint8_t data[3 * KETCUBE_LORA_CFGLEN_DEVADDR];

    if (ketCube_cfg_LoadStr
        ((char *) &(data[0]), 3 * KETCUBE_LORA_CFGLEN_DEVADDR,
         KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_DEVADDR,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_DEVADDR) ==
        KETCUBE_CFG_OK) {

        snprintf(commandIOParams.as_string,
                 KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH,
                 "%s",
                 &(data[0]));
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
    uint8_t data[3 * KETCUBE_LORA_CFGLEN_APPSKEY];

    if (ketCube_cfg_LoadStr
        ((char *) &(data[0]), 3 * KETCUBE_LORA_CFGLEN_APPSKEY,
         KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_APPSKEY,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_APPSKEY) ==
        KETCUBE_CFG_OK) {

        snprintf(commandIOParams.as_string,
                 KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH,
                 "%s",
                 &(data[0]));
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
    uint8_t data[3 * KETCUBE_LORA_CFGLEN_NWKSKEY];

    if (ketCube_cfg_LoadStr
        ((char *) &(data[0]), 3 * KETCUBE_LORA_CFGLEN_NWKSKEY,
         KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_NWKSKEY,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_NWKSKEY) ==
        KETCUBE_CFG_OK) {

        snprintf(commandIOParams.as_string,
                 KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH,
                 "%s",
                 &(data[0]));
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
    ketCube_lora_moduleCfg_t data;

    if (ketCube_cfg_Load
        ((uint8_t *) & (data), KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) !=
        KETCUBE_CFG_OK) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
        return;
    }

    data.connectionType = KETCUBE_LORA_SELCONNMETHOD_ABP;

    if (ketCube_cfg_Save
        ((uint8_t *) & (data), KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) ==
        KETCUBE_CFG_OK) {
        //
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

/**
 * @brief Set OTAA command callback
 *
 */
void ketCube_LoRa_cmd_set_OTAA(void)
{
    ketCube_lora_moduleCfg_t data;

    if (ketCube_cfg_Load
        ((uint8_t *) & (data), KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) !=
        KETCUBE_CFG_OK) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
        return;
    }

    data.connectionType = KETCUBE_LORA_SELCONNMETHOD_OTAA;

    if (ketCube_cfg_Save
        ((uint8_t *) & (data), KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) ==
        KETCUBE_CFG_OK) {
        //
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }

    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Set appEUI command callback
 *
 */
void ketCube_LoRa_cmd_set_appEUI(void)
{
    ketCube_terminal_saveCfgHEXStr(commandIOParams.as_string,
                                   KETCUBE_LISTS_MODULEID_LORA,
                                   (ketCube_cfg_AllocEEPROM_t)
                                   KETCUBE_LORA_CFGADR_APPEUI,
                                   (ketCube_cfg_LenEEPROM_t)
                                   KETCUBE_LORA_CFGLEN_APPEUI);
}

/**
 * @brief Set appKey command callback
 *
 */
void ketCube_LoRa_cmd_set_appKey(void)
{
    ketCube_terminal_saveCfgHEXStr(commandIOParams.as_string,
                                   KETCUBE_LISTS_MODULEID_LORA,
                                   (ketCube_cfg_AllocEEPROM_t)
                                   KETCUBE_LORA_CFGADR_APPKEY,
                                   (ketCube_cfg_LenEEPROM_t)
                                   KETCUBE_LORA_CFGLEN_APPKEY);
}

/**
 * @brief Set devEUI command callback
 *
 */
void ketCube_LoRa_cmd_set_devEUI(void)
{
    ketCube_terminal_saveCfgHEXStr(commandIOParams.as_string,
                                   KETCUBE_LISTS_MODULEID_LORA,
                                   (ketCube_cfg_AllocEEPROM_t)
                                   KETCUBE_LORA_CFGADR_DEVEUI,
                                   (ketCube_cfg_LenEEPROM_t)
                                   KETCUBE_LORA_CFGLEN_DEVEUI);
}

/**
 * @brief Set custom devEUI command callback
 *
 */
void ketCube_LoRa_cmd_set_devEUICustom(void)
{
    ketCube_lora_moduleCfg_t data;

    if (ketCube_cfg_Load
        ((uint8_t *) & (data), KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) !=
        KETCUBE_CFG_OK) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
        return;
    }

    data.devEUIType = KETCUBE_LORA_SELDEVEUI_CUSTOM;

    if (ketCube_cfg_Save
        ((uint8_t *) & (data), KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) ==
        KETCUBE_CFG_OK) {
        //
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

/**
 * @brief Set board devEUI command callback
 *
 */
void ketCube_LoRa_cmd_set_devEUIBoard(void)
{
    ketCube_lora_moduleCfg_t data;

    if (ketCube_cfg_Load
        ((uint8_t *) & (data), KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) !=
        KETCUBE_CFG_OK) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
        return;
    }

    data.devEUIType = KETCUBE_LORA_SELDEVEUI_BOARD;

    if (ketCube_cfg_Save
        ((uint8_t *) & (data), KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) ==
        KETCUBE_CFG_OK) {
        //
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
}

/**
 * @brief Set devAddr command callback
 *
 */
void ketCube_LoRa_cmd_set_devAddr(void)
{
    ketCube_terminal_saveCfgHEXStr(commandIOParams.as_string,
                                   KETCUBE_LISTS_MODULEID_LORA,
                                   (ketCube_cfg_AllocEEPROM_t)
                                   KETCUBE_LORA_CFGADR_DEVADDR,
                                   (ketCube_cfg_LenEEPROM_t)
                                   KETCUBE_LORA_CFGLEN_DEVADDR);
}

/**
 * @brief Set appSKey command callback
 *
 */
void ketCube_LoRa_cmd_set_appSKey(void)
{
    ketCube_terminal_saveCfgHEXStr(commandIOParams.as_string,
                                   KETCUBE_LISTS_MODULEID_LORA,
                                   (ketCube_cfg_AllocEEPROM_t)
                                   KETCUBE_LORA_CFGADR_APPSKEY,
                                   (ketCube_cfg_LenEEPROM_t)
                                   KETCUBE_LORA_CFGLEN_APPSKEY);
}

/**
 * @brief Set nwkSKey command callback
 *
 */
void ketCube_LoRa_cmd_set_nwkSKey(void)
{
    ketCube_terminal_saveCfgHEXStr(commandIOParams.as_string,
                                   KETCUBE_LISTS_MODULEID_LORA,
                                   (ketCube_cfg_AllocEEPROM_t)
                                   KETCUBE_LORA_CFGADR_NWKSKEY,
                                   (ketCube_cfg_LenEEPROM_t)
                                   KETCUBE_LORA_CFGLEN_NWKSKEY);
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
                KETCUBE_TERMINAL_PARAMS_STRING,
                &ketCube_LoRa_cmd_show_appEUI),
    DEF_COMMAND("appKey",
                "Show LoRa application key.",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_STRING,
                &ketCube_LoRa_cmd_show_appKey),
    DEF_COMMAND("appSKey",
                "Show LoRa app session Key.",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_STRING,
                &ketCube_LoRa_cmd_show_appSKey),
    DEF_COMMAND("devAddr",
                "Show LoRa device address.",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_STRING,
                &ketCube_LoRa_cmd_show_devAddr),
    DEF_COMMAND("devEUI",
                "Show LoRa device EUI.",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_STRING,
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
                KETCUBE_TERMINAL_PARAMS_STRING,
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
                KETCUBE_TERMINAL_PARAMS_STRING,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_LoRa_cmd_set_appEUI),
    DEF_COMMAND("appKey",
                "Set LoRa application key",
                KETCUBE_TERMINAL_PARAMS_STRING,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_LoRa_cmd_set_appKey),
    DEF_COMMAND("appSKey",
                "Set LoRa application session key",
                KETCUBE_TERMINAL_PARAMS_STRING,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_LoRa_cmd_set_appSKey),
    DEF_COMMAND("devAddr",
                "Set LoRa device address",
                KETCUBE_TERMINAL_PARAMS_STRING,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_LoRa_cmd_set_devAddr),
    DEF_COMMAND("devEUI",
                "Set LoRa device EUI",
                KETCUBE_TERMINAL_PARAMS_STRING,
                KETCUBE_TERMINAL_PARAMS_NONE,
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
                KETCUBE_TERMINAL_PARAMS_STRING,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_LoRa_cmd_set_nwkSKey),
    DEF_TERMINATE()
};


/**
* @}
*/

#endif                          /* KETCUBE_LORA_CMD_H */
