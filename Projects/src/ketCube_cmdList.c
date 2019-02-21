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

/* define command group - parent for other commands, needs a pointer to
   command subtree */
#define DEF_COMMAND_GROUP(cmd, hlp, cptr) {((char*)&(cmd)),((char*)&(hlp)),\
    {.isGroup = TRUE}, KETCUBE_TERMINAL_PARAMS_NONE,\
    KETCUBE_TERMINAL_PARAMS_NONE,\
    .settingsPtr.subCmdList = (ketCube_terminal_cmd_t*)cptr}

/* define tree leaf - a command, with given in/out parameters and handler;
   by default, a command does not have any flags */
#define DEF_COMMAND(cmd, hlp, parType, outType, fnc) {((char*)&(cmd)),\
    ((char*)&(hlp)), {.isLocal = TRUE, .isEEPROM = TRUE}, parType, outType,\
    (void(*)(void))fnc}

/* define tree leaf - a command, with given in/out parameters and handler;
   in addition to DEF_COMMAND macro, this macro allows setting flags */
#define DEF_COMMAND_WITH_FLAGS(cmd, hlp, flags, parType, outType, fnc) \
    {((char*)&(cmd)), ((char*)&(hlp)), flags, parType, outType,\
    (void(*)(void))fnc}
    
/* define subcommand list (tree level) terminator */
#define DEF_TERMINATE() {((char*)NULL),((char*)NULL),\
    {0} , KETCUBE_TERMINAL_PARAMS_NONE,\
    KETCUBE_TERMINAL_PARAMS_NONE, (void(*)(void))NULL}

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
 * @brief "show" command group
 */
ketCube_terminal_cmd_t ketCube_terminal_commands_show[] = {
    DEF_COMMAND_GROUP("core",
                      "Show KETCube Core parameters",
                      ketCube_terminal_commands_show_core),
    
    DEF_COMMAND_GROUP("driver",
                      "Show KETCube Driver(s) parameters",
                      ketCube_terminal_commands_show_driver),

#ifdef KETCUBE_CFG_INC_MOD_BATMEAS
    DEF_COMMAND_GROUP("batMeas",
                      "Show batMeas parameters",
                      ketCube_terminal_commands_show_batMeas),
#endif /* KETCUBE_CFG_INC_MOD_BATMEAS */
     
#ifdef KETCUBE_CFG_INC_MOD_LORA
    DEF_COMMAND_GROUP("LoRa",
                      "Show LoRa parameters",
                      ketCube_terminal_commands_show_LoRa),
#endif /* KETCUBE_CFG_INC_MOD_LORA */

    DEF_TERMINATE()
};

/**
 * @brief "set" command group
 */
ketCube_terminal_cmd_t ketCube_terminal_commands_set[] = {
    DEF_COMMAND_GROUP("core",
                      "Set KETCube Core parameters",
                      ketCube_terminal_commands_set_core),
    
    DEF_COMMAND_GROUP("driver",
                      "Set KETCube Driver(s) parameters",
                      ketCube_terminal_commands_set_driver),

#ifdef KETCUBE_CFG_INC_MOD_BATMEAS
    DEF_COMMAND_GROUP("batMeas",
                      "Set batMeas parameters",
                      ketCube_terminal_commands_set_batMeas),
#endif /* KETCUBE_CFG_INC_MOD_BATMEAS */

#ifdef KETCUBE_CFG_INC_MOD_LORA
    DEF_COMMAND_GROUP("LoRa",
                      "Set LoRa parameters",
                      ketCube_terminal_commands_set_LoRa),
#endif /* KETCUBE_CFG_INC_MOD_LORA */

    DEF_TERMINATE()
};

/**
 * @brief List of KETCube terminal commands
 */
ketCube_terminal_cmd_t ketCube_terminal_commands[] = {
    DEF_COMMAND("about",
                "Print ABOUT information: Copyright, License, ...",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_terminal_cmd_about),
    DEF_COMMAND("help",
                "Print HELP",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_terminal_cmd_help),
    DEF_COMMAND("disable",
                "Disable KETCube module",
                KETCUBE_TERMINAL_PARAMS_STRING,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_terminal_cmd_disable),
    DEF_COMMAND("enable",
                "Enable KETCube module",
                KETCUBE_TERMINAL_PARAMS_STRING,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_terminal_cmd_enable),
    DEF_COMMAND("list",
                "List available KETCube modules",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_terminal_cmd_list),
    DEF_COMMAND("reload",
                "Reload KETCube",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_terminal_cmd_reload),
    DEF_COMMAND_GROUP("show",
                      "Show LoRa, SigFox ... parameters",
                      ketCube_terminal_commands_show),
    DEF_COMMAND_GROUP("set",
                      "Set LoRa, SigFox ... parameters",
                      ketCube_terminal_commands_set),  
    DEF_TERMINATE()
};
