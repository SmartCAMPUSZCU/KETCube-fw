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


// Declaring extern function HW_GetUniqueId()
extern void HW_GetUniqueId(uint8_t * id);

/**
 * @brief Show devEUI command callback
 * 
 */
void ketCube_LoRa_cmd_show_devEUI(void)
{
    HW_GetUniqueId((uint8_t *) &(commandIOParams.as_byte_array.data[0]));
    commandIOParams.as_byte_array.length = KETCUBE_LORA_CFGLEN_DEVEUI;
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


/* Terminal command definitions */

ketCube_terminal_cmd_t ketCube_terminal_commands_show_LoRa[] = {
    {
        .cmd   = "ABP",
        .descr = "Is ABP enabled?",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isShowCmd = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_BOOLEAN,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, bitCfg),
            .size     = sizeof(ketCube_lora_bitCfg_t),
            .bitSet   = KETCUBE_LORA_SELCONNMETHOD_ABP,
            .bitReset = ~(KETCUBE_LORA_SELCONNMETHOD_ABP),
            .bitMask  = (uint8_t *) & (ketCube_lora_bitCfg_t) {
                .connectionType = (KETCUBE_LORA_SELCONNMETHOD_ABP | ~(KETCUBE_LORA_SELCONNMETHOD_ABP))
            }
        }
    },
                
    {
        .cmd   = "appEUI",
        .descr = "Show LoRa application EUI.",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isShowCmd = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, appEUI),
            .size     = KETCUBE_LORA_CFGLEN_APPEUI,
        }
    },
    
    {
        .cmd   = "appKey",
        .descr = "Show LoRa application key.",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isShowCmd = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, appKey),
            .size     = KETCUBE_LORA_CFGLEN_APPKEY,
        }
    },
    
    {
        .cmd   = "appSKey",
        .descr = "Show LoRa application session key.",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isShowCmd = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, appSKey),
            .size     = KETCUBE_LORA_CFGLEN_APPSKEY,
        }
    },
    
    {
        .cmd   = "devAddr",
        .descr = "Show LoRa device address",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isShowCmd = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, devAddr),
            .size     = KETCUBE_LORA_CFGLEN_DEVADDR,
        }
    },
    
    {
        .cmd   = "devEUI",
        .descr = "Show device-based LoRa device EUI",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isShowCmd = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .settingsPtr.callback = &ketCube_LoRa_cmd_show_devEUI
    },
    
    {
        .cmd   = "devEUICustom",
        .descr = "Show user-defined LoRa device EUI",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isShowCmd = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, devEUI),
            .size     = KETCUBE_LORA_CFGLEN_DEVEUI,
        }
    },
    
    DEF_COMMAND("devEUIType",
                "Show LoRa device EUI type: custom (user-defined) "
                "or deviceID-based",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_STRING,
                &ketCube_LoRa_cmd_show_devEUIType),
                
    {
        .cmd   = "OTAA",
        .descr = "Is OTAA enabled?",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isShowCmd = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_BOOLEAN,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, bitCfg),
            .size     = sizeof(ketCube_lora_bitCfg_t),
            .bitSet   = KETCUBE_LORA_SELCONNMETHOD_OTAA,
            .bitReset = !(KETCUBE_LORA_SELCONNMETHOD_OTAA),
            .bitMask  = (uint8_t *) & (ketCube_lora_bitCfg_t) {
                .connectionType = (KETCUBE_LORA_SELCONNMETHOD_OTAA | ~(KETCUBE_LORA_SELCONNMETHOD_OTAA))
            }
        }
    },
                
    {
        .cmd   = "nwkSKey",
        .descr = "Show LoRa network session Key",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isShowCmd = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, nwkSKey),
            .size     = KETCUBE_LORA_CFGLEN_NWKSKEY,
        }
    },
    
    DEF_TERMINATE()
};

ketCube_terminal_cmd_t ketCube_terminal_commands_set_LoRa[] = {
    {
        .cmd   = "ABP",
        .descr = "Enable ABP",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isSetCmd  = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_NONE,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, bitCfg),
            .size     = sizeof(ketCube_lora_bitCfg_t),
            .bitSet   = KETCUBE_LORA_SELCONNMETHOD_ABP,
            .bitReset = !(KETCUBE_LORA_SELCONNMETHOD_ABP),
            .bitMask  = (uint8_t *) & (ketCube_lora_bitCfg_t) {
                .connectionType = (KETCUBE_LORA_SELCONNMETHOD_ABP | ~(KETCUBE_LORA_SELCONNMETHOD_ABP))
            }
        }
    },
                
    {
        .cmd   = "appEUI",
        .descr = "Set LoRa application EUI.",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isSetCmd  = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, appEUI),
            .size     = KETCUBE_LORA_CFGLEN_APPEUI,
        }
    },
    
    {
        .cmd   = "appKey",
        .descr = "Set LoRa application key.",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isSetCmd  = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, appKey),
            .size     = KETCUBE_LORA_CFGLEN_APPKEY,
        }
    },
    
    {
        .cmd   = "appKey",
        .descr = "Set LoRa application session key.",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isSetCmd  = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, appSKey),
            .size     = KETCUBE_LORA_CFGLEN_APPSKEY,
        }
    },
    
    {
        .cmd   = "devAddr",
        .descr = "Set LoRa device address.",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isSetCmd  = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, devAddr),
            .size     = KETCUBE_LORA_CFGLEN_DEVADDR,
        }
    },
    
    {
        .cmd   = "devEUI",
        .descr = "Set LoRa device EUI",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isSetCmd  = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, devEUI),
            .size     = KETCUBE_LORA_CFGLEN_DEVEUI,
        }
    },
                
    {
        .cmd   = "devEUIBoard",
        .descr = "Use board (boardID-based) LoRa device EUI",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isSetCmd  = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_NONE,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, bitCfg),
            .size     = sizeof(ketCube_lora_bitCfg_t),
            .bitSet   = KETCUBE_LORA_SELDEVEUI_BOARD,
            .bitReset = !(KETCUBE_LORA_SELDEVEUI_BOARD),
            .bitMask  = (uint8_t *) & (ketCube_lora_bitCfg_t) {
                .connectionType = (KETCUBE_LORA_SELDEVEUI_BOARD | ~(KETCUBE_LORA_SELDEVEUI_BOARD))
            }
        }
    },
    
    {
        .cmd   = "devEUICustom",
        .descr = "Use custom (user-defined) LoRa device EUI",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isSetCmd  = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_NONE,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, bitCfg),
            .size     = sizeof(ketCube_lora_bitCfg_t),
            .bitSet   = KETCUBE_LORA_SELDEVEUI_CUSTOM,
            .bitReset = !(KETCUBE_LORA_SELDEVEUI_CUSTOM),
            .bitMask  = (uint8_t *) & (ketCube_lora_bitCfg_t) {
                .connectionType = (KETCUBE_LORA_SELDEVEUI_CUSTOM | ~(KETCUBE_LORA_SELDEVEUI_CUSTOM))
            }
        }
    },
                
    {
        .cmd   = "OTAA",
        .descr = "Enable OTAA",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isSetCmd  = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_NONE,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, bitCfg),
            .size     = sizeof(ketCube_lora_bitCfg_t),
            .bitSet   = KETCUBE_LORA_SELCONNMETHOD_OTAA,
            .bitReset = ~(KETCUBE_LORA_SELCONNMETHOD_OTAA),
            .bitMask  = (uint8_t *) & (ketCube_lora_bitCfg_t) {
                .connectionType = (KETCUBE_LORA_SELCONNMETHOD_OTAA | ~(KETCUBE_LORA_SELCONNMETHOD_OTAA))
            }
        }
    },
                
    {
        .cmd   = "nwkSKey",
        .descr = "Set LoRa network session key",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isSetCmd  = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_LORA,
            .offset   = offsetof(ketCube_lora_moduleCfg_t, nwkSKey),
            .size     = KETCUBE_LORA_CFGLEN_NWKSKEY,
        }
    },
    
    DEF_TERMINATE()
};


/**
* @}
*/

#endif                          /* KETCUBE_LORA_CMD_H */
