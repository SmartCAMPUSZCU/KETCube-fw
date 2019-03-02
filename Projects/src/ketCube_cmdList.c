/**
 * @file    ketCube_cmdList.c
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2018-10-30
 * @brief   This file contains the KETCube Command List
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

#include "ketCube_terminal.h"
#include "ketCube_common.h"
#include "ketCube_cfg.h"


/** 
 * Define (sub)command list (tree level) terminator
 * 
 */
#define DEF_TERMINATE() { .cmd = ((char*) NULL), \
                          .descr = ((char*) NULL),\
                          .settingsPtr.callback = (void(*)(void)) NULL }

/* always include core configuration commands */
#include "ketCube_core_cmd.c"

/* conditionally include module configuration commands */
#ifdef KETCUBE_CFG_INC_MOD_BATMEAS
#include "ketCube_batMeas_cmd.c"
#endif

#ifdef KETCUBE_CFG_INC_MOD_LORA
#include "ketCube_lora_cmd.c"
#endif

/**
 * @brief SET/SHOW command group(s)
 */
ketCube_terminal_cmd_t ketCube_terminal_commands_setShow[] = {
    {
        .cmd   = "core",
        .descr = "KETCube Core parameters",
        .flags = {
            .isGroup   = TRUE,
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isRAM     = TRUE,
            .isGeneric = TRUE,
            .isShowCmd = TRUE,
            .isSetCmd  = TRUE,
            .isEnvCmd  = TRUE,
        },
        .settingsPtr.subCmdList = ketCube_terminal_commands_core,
    },
    
    {
        .cmd   = "driver",
        .descr = "KETCube Driver(s) parameters",
        .flags = {
            .isGroup   = TRUE,
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isRAM     = TRUE,
            .isGeneric = TRUE,
            .isShowCmd = TRUE,
            .isSetCmd  = TRUE,
            .isEnvCmd  = TRUE,
        },
        .settingsPtr.subCmdList = ketCube_terminal_commands_driver,
    },
                      
#ifdef KETCUBE_CFG_INC_MOD_BATMEAS
    {
        .cmd   = "batMeas",
        .descr = "batMeas parameters",
        .flags = {
            .isGroup   = TRUE,
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isRAM     = TRUE,
            .isGeneric = TRUE,
            .isShowCmd = TRUE,
            .isSetCmd  = TRUE,
            .isEnvCmd  = TRUE,
        },
        .settingsPtr.subCmdList = ketCube_batMeas_commands,
    },
#endif /* KETCUBE_CFG_INC_MOD_BATMEAS */
     
#ifdef KETCUBE_CFG_INC_MOD_LORA
    {
        .cmd   = "LoRa",
        .descr = "LoRa perisitent parameters",
        .flags = {
            .isGroup   = TRUE,
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isGeneric = TRUE,
            .isShowCmd = TRUE,
            .isSetCmd  = TRUE,
            .isEnvCmd  = TRUE,
        },
        .settingsPtr.subCmdList = ketCube_lora_commands,
    },
    
    {
        .cmd   = "LoRa",
        .descr = "LoRa running parameters",
        .flags = {
            .isGroup   = TRUE,
            .isLocal   = TRUE,
            .isRAM     = TRUE,
            .isGeneric = TRUE,
            .isShowCmd = TRUE,
            .isSetCmd  = TRUE,
            .isEnvCmd  = TRUE,
        },
        .settingsPtr.subCmdList = ketCube_lora_commands,
    },
#endif /* KETCUBE_CFG_INC_MOD_LORA */

    DEF_TERMINATE()
};


/**
 * @brief KETCube root terminal commands
 */
ketCube_terminal_cmd_t ketCube_terminal_commands[] = {
    {
        .cmd   = "about",
        .descr = "Print ABOUT information: Copyright, License, ...",
        .flags = {
            .isLocal   = TRUE,
            .isEnvCmd  = TRUE,
        },
        .settingsPtr.callback = &ketCube_terminal_cmd_about,
    },
    
    {
        .cmd   = "help",
        .descr = "Print HELP",
        .flags = {
            .isLocal   = TRUE,
            .isEnvCmd  = TRUE,
        },
        .settingsPtr.callback = &ketCube_terminal_cmd_help,
    },
    
    {
        .cmd   = "disable",
        .descr = "Disable KETCube module",
        .flags = {
            .isLocal   = TRUE,
            .isRemote  = TRUE,
            .isEnvCmd  = TRUE,
        },
        .paramSetType = KETCUBE_TERMINAL_PARAMS_STRING,
        .settingsPtr.callback = &ketCube_terminal_cmd_disable,
    },
    
    {
        .cmd   = "enable",
        .descr = "Enable KETCube module",
        .flags = {
            .isLocal   = TRUE,
            .isRemote  = TRUE,
            .isEnvCmd  = TRUE,
        },
        .paramSetType = KETCUBE_TERMINAL_PARAMS_STRING,
        .settingsPtr.callback = &ketCube_terminal_cmd_enable,
    },
    
    {
        .cmd   = "list",
        .descr = "List available KETCube modules",
        .flags = {
            .isLocal   = TRUE,
            .isEnvCmd  = TRUE,
        },
        .settingsPtr.callback = &ketCube_terminal_cmd_list,
    },
    
    {
        .cmd   = "reload",
        .descr = "Reload KETCube",
        .flags = {
            .isLocal   = TRUE,
            .isRemote  = TRUE,
            .isEnvCmd  = TRUE,
        },
        .settingsPtr.callback = &ketCube_terminal_cmd_reload,
    },
    
    {
        .cmd   = "show",
        .descr = "Show LoRa, SigFox ... parameters",
        .flags = {
            .isGroup   = TRUE,
            .isLocal   = TRUE,
            .isRemote  = TRUE,
            .isEEPROM  = TRUE,
            .isGeneric = TRUE,
            .isShowCmd = TRUE,
            .isEnvCmd  = TRUE,
        },
        .settingsPtr.subCmdList = ketCube_terminal_commands_setShow,
    },
    
    {
        .cmd   = "showr",
        .descr = "Show LoRa, SigFox ... RUNNING parameters",
        .flags = {
            .isGroup   = TRUE,
            .isLocal   = TRUE,
            .isRemote  = TRUE,
            .isRAM     = TRUE,
            .isGeneric = TRUE,
            .isShowCmd = TRUE,
            .isEnvCmd  = TRUE,
        },
        .settingsPtr.subCmdList = ketCube_terminal_commands_setShow,
    },
    
    {
        .cmd   = "set",
        .descr = "Set LoRa, SigFox ... parameters",
        .flags = {
            .isGroup   = TRUE,
            .isLocal   = TRUE,
            .isRemote  = TRUE,
            .isEEPROM  = TRUE,
            .isGeneric = TRUE,
            .isSetCmd  = TRUE,
            .isEnvCmd  = TRUE,
        },
        .settingsPtr.subCmdList = ketCube_terminal_commands_setShow,
    },
    
    {
        .cmd   = "setr",
        .descr = "Set LoRa, SigFox ... RUNNING parameters",
        .flags = {
            .isGroup   = TRUE,
            .isLocal   = TRUE,
            .isRemote  = TRUE,
            .isRAM     = TRUE,
            .isGeneric = TRUE,
            .isSetCmd  = TRUE,
            .isEnvCmd  = TRUE,
        },
        .settingsPtr.subCmdList = ketCube_terminal_commands_setShow,
    },

    DEF_TERMINATE()
};
