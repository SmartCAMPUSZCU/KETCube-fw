/**
 * @file    ketCube_lora_cmd.c
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2019-04-19
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

#include <stddef.h>

#include "ketCube_cfg.h"
#include "ketCube_common.h"
#include "ketCube_lora.h"
#ifndef DESKTOP_BUILD
#include "LoRaMac.h"
#endif

/** @defgroup KETCube_LoRa_CMD KETCube LoRa CMD
  * @brief KETCube LoRa module commandline deffinition
  * @ingroup KETCube_Terminal
  * @{
  */

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

/* Terminal command definitions */

ketCube_terminal_cmd_t ketCube_lora_commands[] = {
   {
      .cmd   = "appEUI",
      .descr = "LoRa application EUI.",
      .flags = {
         .isLocal   = TRUE,
         .isEEPROM  = TRUE,
         .isShowCmd = TRUE,
         .isSetCmd  = TRUE,
         .isGeneric = TRUE,
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
      .descr = "LoRa application key.",
      .flags = {
         .isLocal   = TRUE,
         .isEEPROM  = TRUE,
         .isShowCmd = TRUE,
         .isSetCmd  = TRUE,
         .isGeneric = TRUE,
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
      .cmd   = "appSKey",
      .descr = "LoRa application session key.",
      .flags = {
         .isLocal   = TRUE,
         .isEEPROM  = TRUE,
         .isShowCmd = TRUE,
         .isSetCmd  = TRUE,
         .isGeneric = TRUE,
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
      .descr = "LoRa device address.",
      .flags = {
         .isLocal   = TRUE,
         .isEEPROM  = TRUE,
         .isShowCmd = TRUE,
         .isSetCmd  = TRUE,
         .isGeneric = TRUE,
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
      .cmd   = "devClass",
      .descr = "Lora device class: 0 = A, 1 = B, 2 = C",
      .flags = {
         .isLocal   = TRUE,
         .isEEPROM  = TRUE,
         .isRAM     = TRUE,
         .isShowCmd = TRUE,
         .isSetCmd  = TRUE,
         .isGeneric = TRUE,
      },
      .paramSetType  = KETCUBE_TERMINAL_PARAMS_BYTE,
      .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE,
      .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
         .moduleID = KETCUBE_LISTS_MODULEID_LORA,
         .offset   = offsetof(ketCube_lora_moduleCfg_t, devClass),
         .size     = sizeof(DeviceClass_t),
      }
   },    

   {
      .cmd   = "devEUIBoard",
      .descr = "Board (boardID-based) LoRa device EUI",
      .flags = {
         .isLocal   = TRUE,
         .isEEPROM  = TRUE,
         .isShowCmd = TRUE,
      },
      .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
      .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
      .settingsPtr.callback = &ketCube_LoRa_cmd_show_devEUI,
   },

   {
      .cmd   = "devEUICustom",
      .descr = "Custom LoRa device EUI",
      .flags = {
         .isLocal   = TRUE,
         .isEEPROM  = TRUE,
         .isShowCmd = TRUE,
         .isSetCmd  = TRUE,
         .isGeneric = TRUE,
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
      .cmd   = "enableABP",
      .descr = "Enable ABP",
      .flags = {
         .isLocal   = TRUE,
         .isEEPROM  = TRUE,
         .isShowCmd = TRUE,
         .isSetCmd  = TRUE,
         .isGeneric = TRUE,
      },
      .paramSetType  = KETCUBE_TERMINAL_PARAMS_BOOLEAN,
      .outputSetType = KETCUBE_TERMINAL_PARAMS_BOOLEAN,
      .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
         .moduleID = KETCUBE_LISTS_MODULEID_LORA,
         .offset   = (offsetof(ketCube_lora_moduleCfg_t, cfg) 
                     + offsetof(ketCube_lora_cfg_t, connectionType)),
         .size     = sizeof(ketCube_lora_cfg_t),
      }
   },
   
   {
      .cmd   = "enableCustomDevEUI",
      .descr = "Custom (user-defined) LoRa device EUI",
      .flags = {
         .isLocal   = TRUE,
         .isEEPROM  = TRUE,
         .isShowCmd = TRUE,
         .isSetCmd  = TRUE,
         .isGeneric = TRUE,
      },
      .paramSetType  = KETCUBE_TERMINAL_PARAMS_BOOLEAN,
      .outputSetType = KETCUBE_TERMINAL_PARAMS_BOOLEAN,
      .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
         .moduleID = KETCUBE_LISTS_MODULEID_LORA,
         .offset   = (offsetof(ketCube_lora_moduleCfg_t, cfg) 
                      + offsetof(ketCube_lora_cfg_t, devEUIType)),
         .size     = sizeof(ketCube_lora_cfg_t),
      }
   },
   
#if (KETCUBE_LORA_LRWAN_VERSION == KETCUBE_LORA_LRWAN_VERSION_V11x)
   {
      .cmd   = "fNwkSIntKey",
      .descr = "Forwarding Network session integrity key",
      .flags = {
         .isLocal   = TRUE,
         .isEEPROM  = TRUE,
         .isRAM     = TRUE,
         .isShowCmd = TRUE,
         .isSetCmd  = TRUE,
         .isGeneric = TRUE,
      },
      .paramSetType  = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
      .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
      .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
         .moduleID = KETCUBE_LISTS_MODULEID_LORA,
         .offset   = offsetof(ketCube_lora_moduleCfg_t, fNwkSIntKey),
         .size     = KETCUBE_LORA_CFGLEN_FNWKSINTKEY,
      }
   },
   
   {
      .cmd   = "nwkKey",
      .descr = "Network key",
      .flags = {
         .isLocal   = TRUE,
         .isEEPROM  = TRUE,
         .isRAM     = TRUE,
         .isShowCmd = TRUE,
         .isSetCmd  = TRUE,
         .isGeneric = TRUE,
      },
      .paramSetType  = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
      .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
      .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
         .moduleID = KETCUBE_LISTS_MODULEID_LORA,
         .offset   = offsetof(ketCube_lora_moduleCfg_t, nwkKey),
         .size     = KETCUBE_LORA_CFGLEN_NWKKEY,
      }
   },
   
   {
      .cmd   = "nwkSEncKey",
      .descr = "Network session encryption key",
      .flags = {
         .isLocal   = TRUE,
         .isEEPROM  = TRUE,
         .isRAM     = TRUE,
         .isShowCmd = TRUE,
         .isSetCmd  = TRUE,
         .isGeneric = TRUE,
      },
      .paramSetType  = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
      .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
      .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
         .moduleID = KETCUBE_LISTS_MODULEID_LORA,
         .offset   = offsetof(ketCube_lora_moduleCfg_t, nwkSEncKey),
         .size     = KETCUBE_LORA_CFGLEN_NWKSENCKEY,
      }
   },
   
   {
      .cmd   = "sNwkSIntKey",
      .descr = "Serving Network session integrity key",
      .flags = {
         .isLocal   = TRUE,
         .isEEPROM  = TRUE,
         .isRAM     = TRUE,
         .isShowCmd = TRUE,
         .isSetCmd  = TRUE,
         .isGeneric = TRUE,
      },
      .paramSetType  = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
      .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
      .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
         .moduleID = KETCUBE_LISTS_MODULEID_LORA,
         .offset   = offsetof(ketCube_lora_moduleCfg_t, sNwkSIntKey),
         .size     = KETCUBE_LORA_CFGLEN_SNWKSINTKEY,
      }
   },
   
#else
   {
      .cmd   = "nwkSKey",
      .descr = "LoRa network session key",
      .flags = {
         .isLocal   = TRUE,
         .isEEPROM  = TRUE,
         .isShowCmd = TRUE,
         .isSetCmd  = TRUE,
         .isGeneric = TRUE,
      },
      .paramSetType  = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
      .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY,
      .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
         .moduleID = KETCUBE_LISTS_MODULEID_LORA,
         .offset   = offsetof(ketCube_lora_moduleCfg_t, nwkSEncKey),
         .size     = KETCUBE_LORA_CFGLEN_NWKSENCKEY,
      }
   },
   
#endif
   {
      .cmd   = "txDatarate",
      .descr = "Uplink datarate (0-15)",
      .flags = {
         .isLocal   = TRUE,
         .isEEPROM  = TRUE,
         .isRAM     = TRUE,
         .isShowCmd = TRUE,
         .isSetCmd  = TRUE,
         .isGeneric = TRUE,
      },
      .paramSetType  = KETCUBE_TERMINAL_PARAMS_BYTE,
      .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE,
      .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
         .moduleID = KETCUBE_LISTS_MODULEID_LORA,
         .offset   = offsetof(ketCube_lora_moduleCfg_t, txDatarate),
         .size     = sizeof(uint8_t),
      }
   },

   DEF_TERMINATE()
};


/**
* @}
*/

#endif                          /* KETCUBE_LORA_CMD_H */
