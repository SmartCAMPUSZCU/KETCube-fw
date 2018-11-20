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

    KETCUBE_TERMINAL_ENDL();

    if (ketCube_cfg_Load
        ((uint8_t *) & data, KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) ==
        KETCUBE_CFG_OK) {
        if (data.connectionType == KETCUBE_LORA_SELCONNMETHOD_ABP) {
            KETCUBE_TERMINAL_PRINTF("ABP is enabled!");
        } else {
            KETCUBE_TERMINAL_PRINTF("ABP is disabled!");
        }
    } else {
        KETCUBE_TERMINAL_PRINTF("Error while reading LoRa configuration!");
    }

    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Is OTAA Enabled ? callback
 * 
 */
void ketCube_LoRa_cmd_show_OTAA(void)
{
    ketCube_lora_moduleCfg_t data;

    KETCUBE_TERMINAL_ENDL();

    if (ketCube_cfg_Load
        ((uint8_t *) & data, KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) ==
        KETCUBE_CFG_OK) {
        if (data.connectionType == KETCUBE_LORA_SELCONNMETHOD_OTAA) {
            KETCUBE_TERMINAL_PRINTF("OTAA is enabled!");
        } else {
            KETCUBE_TERMINAL_PRINTF("OTAA is disabled!");
        }
    } else {
        KETCUBE_TERMINAL_PRINTF("Error while reading LoRa configuration!");
    }

    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Show devEUI command callback
 * 
 */
void ketCube_LoRa_cmd_show_devEUI(void)
{
    char data[3 * KETCUBE_LORA_CFGLEN_DEVEUI];
    ketCube_lora_moduleCfg_t type;
    uint8_t i;
    
    KETCUBE_TERMINAL_ENDL();

    if (ketCube_cfg_Load
        ((uint8_t *) & (type), KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) !=
        KETCUBE_CFG_OK) {
        KETCUBE_TERMINAL_PRINTF("Error while reading LoRa configuration!");
    }    
    
    if (type.devEUIType == KETCUBE_LORA_SELDEVEUI_BOARD) {
        HW_GetUniqueId(&(data[0]));
        for (i = 0; i < KETCUBE_LORA_CFGLEN_DEVEUI; i++) {
            ketCube_common_Byte2hex(data[KETCUBE_LORA_CFGLEN_DEVEUI-i-1], &(data[3*KETCUBE_LORA_CFGLEN_DEVEUI-(3*(i+1))]));
            
            data[3*KETCUBE_LORA_CFGLEN_DEVEUI-(3*(i+1))+2] = '-';
        }
        data[3*KETCUBE_LORA_CFGLEN_DEVEUI-1] = 0x00;
    } else {
        if (ketCube_cfg_LoadStr
            ((char *) &(data[0]), 3 * KETCUBE_LORA_CFGLEN_DEVEUI,
             KETCUBE_LISTS_MODULEID_LORA,
             (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_DEVEUI,
             (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_DEVEUI) ==
            KETCUBE_CFG_OK) {
        } else {
            KETCUBE_TERMINAL_PRINTF("Error while reading devEUI from EEPROM!");
        }
    }
    
    KETCUBE_TERMINAL_PRINTF("DevEUI: %s", &(data[0]));

    KETCUBE_TERMINAL_ENDL();
}


/**
 * @brief Show LoRa devEUI type callback
 * 
 */
void ketCube_LoRa_cmd_show_devEUIType(void)
{
    ketCube_lora_moduleCfg_t data;

    KETCUBE_TERMINAL_ENDL();

    if (ketCube_cfg_Load
        ((uint8_t *) & (data), KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) ==
        KETCUBE_CFG_OK) {
        if (data.devEUIType == KETCUBE_LORA_SELDEVEUI_CUSTOM) {
            KETCUBE_TERMINAL_PRINTF
                ("Custom (user-defined) devEUI is used!");
        } else {
            KETCUBE_TERMINAL_PRINTF
                ("Board (deviceID-based) devEUI is used!");
        }
    } else {
        KETCUBE_TERMINAL_PRINTF("Error while reading LoRa configuration!");
    }

    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Show appEUI command callback
 * 
 */
void ketCube_LoRa_cmd_show_appEUI(void)
{
    uint8_t data[3 * KETCUBE_LORA_CFGLEN_APPEUI];

    KETCUBE_TERMINAL_ENDL();

    if (ketCube_cfg_LoadStr
        ((char *) &(data[0]), 3 * KETCUBE_LORA_CFGLEN_APPEUI,
         KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_APPEUI,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_APPEUI) ==
        KETCUBE_CFG_OK) {
        KETCUBE_TERMINAL_PRINTF("AppEUI: %s", &(data[0]));
    } else {
        KETCUBE_TERMINAL_PRINTF("Error while reading appEUI!");
    }

    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Show appKey command callback
 * 
 */
void ketCube_LoRa_cmd_show_appKey(void)
{
    uint8_t data[3 * KETCUBE_LORA_CFGLEN_APPKEY];

    KETCUBE_TERMINAL_ENDL();

    if (ketCube_cfg_LoadStr
        ((char *) &(data[0]), 3 * KETCUBE_LORA_CFGLEN_APPKEY,
         KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_APPKEY,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_APPKEY) ==
        KETCUBE_CFG_OK) {
        KETCUBE_TERMINAL_PRINTF("AppKey: %s", &(data[0]));
    } else {
        KETCUBE_TERMINAL_PRINTF("Error while reading appKey!");
    }

    KETCUBE_TERMINAL_ENDL();
}


/**
 * @brief Show devAddr command callback
 * 
 */
void ketCube_LoRa_cmd_show_devAddr(void)
{
    uint8_t data[3 * KETCUBE_LORA_CFGLEN_DEVADDR];

    KETCUBE_TERMINAL_ENDL();

    if (ketCube_cfg_LoadStr
        ((char *) &(data[0]), 3 * KETCUBE_LORA_CFGLEN_DEVADDR,
         KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_DEVADDR,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_DEVADDR) ==
        KETCUBE_CFG_OK) {
        KETCUBE_TERMINAL_PRINTF("DevAddr: %s", &(data[0]));
    } else {
        KETCUBE_TERMINAL_PRINTF("Error while reading devAddr!");
    }

    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Show appSKey command callback
 * 
 */
void ketCube_LoRa_cmd_show_appSKey(void)
{
    uint8_t data[3 * KETCUBE_LORA_CFGLEN_APPSKEY];

    KETCUBE_TERMINAL_ENDL();

    if (ketCube_cfg_LoadStr
        ((char *) &(data[0]), 3 * KETCUBE_LORA_CFGLEN_APPSKEY,
         KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_APPSKEY,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_APPSKEY) ==
        KETCUBE_CFG_OK) {
        KETCUBE_TERMINAL_PRINTF("AppSKey: %s", &(data[0]));
    } else {
        KETCUBE_TERMINAL_PRINTF("Error while reading appSKey!");
    }

    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Show appSKey command callback
 * 
 */
void ketCube_LoRa_cmd_show_nwkSKey(void)
{
    uint8_t data[3 * KETCUBE_LORA_CFGLEN_NWKSKEY];

    KETCUBE_TERMINAL_ENDL();

    if (ketCube_cfg_LoadStr
        ((char *) &(data[0]), 3 * KETCUBE_LORA_CFGLEN_NWKSKEY,
         KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_NWKSKEY,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_NWKSKEY) ==
        KETCUBE_CFG_OK) {
        KETCUBE_TERMINAL_PRINTF("NwkSKey: %s", &(data[0]));
    } else {
        KETCUBE_TERMINAL_PRINTF("Error while reading nwkSKey!");
    }

    KETCUBE_TERMINAL_ENDL();
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
        KETCUBE_TERMINAL_PRINTF("Set ABP error!");
        return;
    }

    data.connectionType = KETCUBE_LORA_SELCONNMETHOD_ABP;

    if (ketCube_cfg_Save
        ((uint8_t *) & (data), KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) ==
        KETCUBE_CFG_OK) {
        KETCUBE_TERMINAL_PRINTF("Set ABP success!");
    } else {
        KETCUBE_TERMINAL_PRINTF("Set ABP error!");
    }

    KETCUBE_TERMINAL_ENDL();
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
        KETCUBE_TERMINAL_PRINTF("Set OTAA error!");
        return;
    }

    data.connectionType = KETCUBE_LORA_SELCONNMETHOD_OTAA;

    if (ketCube_cfg_Save
        ((uint8_t *) & (data), KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) ==
        KETCUBE_CFG_OK) {
        KETCUBE_TERMINAL_PRINTF("Set OTAA success!");
    } else {
        KETCUBE_TERMINAL_PRINTF("Set OTAA error!");
    }

    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Set appEUI command callback
 *
 */
void ketCube_LoRa_cmd_set_appEUI(void)
{
    ketCube_terminal_saveCfgHEXStr(&(commandBuffer[commandParams]),
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
    ketCube_terminal_saveCfgHEXStr(&(commandBuffer[commandParams]),
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
    ketCube_terminal_saveCfgHEXStr(&(commandBuffer[commandParams]),
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
        KETCUBE_TERMINAL_PRINTF("Set custom devEUI error!");
        return;
    }

    data.devEUIType = KETCUBE_LORA_SELDEVEUI_CUSTOM;

    if (ketCube_cfg_Save
        ((uint8_t *) & (data), KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) ==
        KETCUBE_CFG_OK) {
        KETCUBE_TERMINAL_PRINTF("Set custom devEUI success!");
    } else {
        KETCUBE_TERMINAL_PRINTF("Set custom devEUI error!");
    }

    KETCUBE_TERMINAL_ENDL();
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
        KETCUBE_TERMINAL_PRINTF("Set board devEUI error!");
        return;
    }

    data.devEUIType = KETCUBE_LORA_SELDEVEUI_BOARD;

    if (ketCube_cfg_Save
        ((uint8_t *) & (data), KETCUBE_LISTS_MODULEID_LORA,
         (ketCube_cfg_AllocEEPROM_t) KETCUBE_LORA_CFGADR_CFG,
         (ketCube_cfg_LenEEPROM_t) KETCUBE_LORA_CFGLEN_CFG) ==
        KETCUBE_CFG_OK) {
        KETCUBE_TERMINAL_PRINTF("Set board devEUI success!");
    } else {
        KETCUBE_TERMINAL_PRINTF("Set board devEUI error!");
    }

    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Set devAddr command callback
 *
 */
void ketCube_LoRa_cmd_set_devAddr(void)
{
    ketCube_terminal_saveCfgHEXStr(&(commandBuffer[commandParams]),
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
    ketCube_terminal_saveCfgHEXStr(&(commandBuffer[commandParams]),
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
    ketCube_terminal_saveCfgHEXStr(&(commandBuffer[commandParams]),
                                   KETCUBE_LISTS_MODULEID_LORA,
                                   (ketCube_cfg_AllocEEPROM_t)
                                   KETCUBE_LORA_CFGADR_NWKSKEY,
                                   (ketCube_cfg_LenEEPROM_t)
                                   KETCUBE_LORA_CFGLEN_NWKSKEY);
}


/**
* @}
*/

#endif                          /* KETCUBE_LORA_CMD_H */
