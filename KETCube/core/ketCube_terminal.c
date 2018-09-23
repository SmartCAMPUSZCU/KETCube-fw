/**
 * @file    ketCube_terminal.c
 * @author  Jan Belohoubek
 * @author  Martin Ubl
 * @version 0.1
 * @date    2018-05-07
 * @brief   This file contains the KETCube Terminal
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

#include <string.h>
#include <stdarg.h>

#include "ketCube_terminal.h"
#include "ketCube_common.h"
#include "ketCube_cfg.h"
#include "ketCube_coreCfg.h"
#include "ketCube_eeprom.h"
#include "ketCube_modules.h"
#include "vcom.h"

static ketCube_terminal_buffer_t
    commandHistory[KETCUBE_TERMINAL_HISTORY_LEN];
static char *commandBuffer = (char *) &(commandHistory[0].buffer[0]);
static uint8_t *commandPtr = (uint8_t *) & (commandHistory[0].ptr);
static uint8_t commandHistoryPtr = 0;
static uint8_t commandParams;   //< Index of the first command parameter in buffer

/* Helper function prototypes */
static void ketCube_terminal_printCmdList(uint16_t cmdIndex,
                                          uint8_t level);
static void ketCube_terminal_saveCfgHEXStr(char *data,
                                           ketCube_cfg_moduleIDs_t id,
                                           ketCube_cfg_AllocEEPROM_t addr,
                                           ketCube_cfg_LenEEPROM_t len);
static void ketCube_terminal_saveCfgDECStr(char *data,
                                           ketCube_cfg_moduleIDs_t id,
                                           ketCube_cfg_AllocEEPROM_t addr,
                                           ketCube_cfg_LenEEPROM_t len);

/* Core command callback prototypes */
static void ketCube_terminal_cmd_reload(void);
static void ketCube_terminal_cmd_list(void);
static void ketCube_terminal_cmd_enable(void);
static void ketCube_terminal_cmd_disable(void);

/* Core configuration */
static void ketCube_terminal_cmd_show_core_basePeriod(void);
static void ketCube_terminal_cmd_show_core_startDelay(void);
static void ketCube_terminal_cmd_set_core_basePeriod(void);
static void ketCube_terminal_cmd_set_core_startDelay(void);

// Place module command sets here
#include "ketCube_lora_cmd.c"

/**
  *
	*  @brief List of KETCube terminal commands
	*
  */
ketCube_terminal_cmd_t ketCube_terminal_commands[] = {
    {((char *) &("about")),
     ((char *)
      &("Print information about KETCube, Copyright, License, ...")),
     0,
     0,
     &ketCube_terminal_cmd_about},

    {((char *) &("help")),
     ((char *) &("Print HELP")),
     0,
     0,
     &ketCube_terminal_cmd_help},

    {((char *) &("disable")),
     ((char *) &("Disable KETCube module")),
     0,
     1,
     &ketCube_terminal_cmd_disable},

    {((char *) &("enable")),
     ((char *) &("Enable KETCube module")),
     0,
     1,
     &ketCube_terminal_cmd_enable},

    {((char *) &("list")),
     ((char *) &("List available KETCube modules")),
     0,
     0,
     &ketCube_terminal_cmd_list},

    {((char *) &("reload")),
     ((char *) &("Reload KETCube")),
     0,
     0,
     &ketCube_terminal_cmd_reload},

    {((char *) &("show")),
     ((char *) &("Show LoRa, SigFox ... parameters")),
     0,
     0,
     (void *) NULL},

    {((char *) &("core")),
     ((char *) &("Show KETCube Core parameters")),
     1,
     0,
     (void *) NULL},

    {((char *) &("basePeriod")),
     ((char *) &("KETCube base period")),
     2,
     0,
     &ketCube_terminal_cmd_show_core_basePeriod},

    {((char *) &("startDelay")),
     ((char *) &("First periodic action is delayed after power-up")),
     2,
     0,
     &ketCube_terminal_cmd_show_core_startDelay},

#ifdef KETCUBE_CFG_INC_MOD_LORA

    {((char *) &("LoRa")),
     ((char *) &("Show LoRa parameters")),
     1,
     0,
     (void *) NULL},

    {((char *) &("ABP")),
     ((char *) &("Is ABP enabled?")),
     2,
     0,
     &ketCube_LoRa_cmd_show_ABP},

    {((char *) &("appEUI")),
     ((char *) &("Show LoRa application EUI.")),
     2,
     0,
     &ketCube_LoRa_cmd_show_appEUI},

    {((char *) &("appKey")),
     ((char *) &("Show LoRa application key.")),
     2,
     0,
     &ketCube_LoRa_cmd_show_appKey},


    {((char *) &("appSKey")),
     ((char *) &("Show LoRa app session Key.")),
     2,
     0,
     &ketCube_LoRa_cmd_show_appSKey},

    {((char *) &("devAddr")),
     ((char *) &("Show LoRa device address.")),
     2,
     0,
     &ketCube_LoRa_cmd_show_devAddr},

    {((char *) &("devEUI")),
     ((char *) &("Show LoRa device EUI.")),
     2,
     0,
     &ketCube_LoRa_cmd_show_devEUI},

    {((char *) &("devEUIType")),
     ((char *)
      &
      ("Show LoRa device EUI type: custom (user-defined) or deviceID-based.")),
     2,
     0,
     &ketCube_LoRa_cmd_show_devEUIType},

    {((char *) &("OTAA")),
     ((char *) &("Is OTAA enabled?")),
     2,
     0,
     &ketCube_LoRa_cmd_show_OTAA},

    {((char *) &("nwkSKey")),
     ((char *) &("Show LoRa network session Key.")),
     2,
     0,
     &ketCube_LoRa_cmd_show_nwkSKey},

#endif                          /* KETCUBE_CFG_INC_MOD_LORA */

    {((char *) &("set")),
     ((char *) &("Set LoRa, Sigfox ... parameters")),
     0,
     0,
     (void *) NULL},


    {((char *) &("core")),
     ((char *) &("Set KETCube Core parameters")),
     1,
     0,
     (void *) NULL},

    {((char *) &("basePeriod")),
     ((char *) &("KETCube base period")),
     2,
     1,
     &ketCube_terminal_cmd_set_core_basePeriod},

    {((char *) &("startDelay")),
     ((char *) &("First periodic action is delayed after power-up")),
     2,
     1,
     &ketCube_terminal_cmd_set_core_startDelay},

#ifdef KETCUBE_CFG_INC_MOD_LORA

    {((char *) &("LoRa")),
     ((char *) &("Set LoRa parameters")),
     1,
     0,
     (void *) NULL},

    {((char *) &("ABP")),
     ((char *) &("Enable ABP")),
     2,
     0,
     &ketCube_LoRa_cmd_set_ABP},

    {((char *) &("appEUI")),
     ((char *) &("Set LoRa application EUI")),
     2,
     1,
     &ketCube_LoRa_cmd_set_appEUI},

    {((char *) &("appKey")),
     ((char *) &("Set LoRa application key")),
     2,
     1,
     &ketCube_LoRa_cmd_set_appKey},

    {((char *) &("appSKey")),
     ((char *) &("Set LoRa application session key")),
     2,
     1,
     &ketCube_LoRa_cmd_set_appSKey},

    {((char *) &("devAddr")),
     ((char *) &("Set LoRa device address")),
     2,
     1,
     &ketCube_LoRa_cmd_set_devAddr},

    {((char *) &("devEUI")),
     ((char *) &("Set LoRa device EUI")),
     2,
     1,
     &ketCube_LoRa_cmd_set_devEUI},

    {((char *) &("devEUICustom")),
     ((char *) &("Use custom (user-defined) LoRa device EUI")),
     2,
     0,
     &ketCube_LoRa_cmd_set_devEUICustom},

    {((char *) &("devEUIBoard")),
     ((char *) &("Use board (boardID-based) LoRa device EUI")),
     2,
     0,
     &ketCube_LoRa_cmd_set_devEUIBoard},

    {((char *) &("OTAA")),
     ((char *) &("Enable OTAA")),
     2,
     0,
     &ketCube_LoRa_cmd_set_OTAA},

    {((char *) &("nwkSKey")),
     ((char *) &("Set LoRa network session key")),
     2,
     1,
     &ketCube_LoRa_cmd_set_nwkSKey},

#endif                          /* KETCUBE_CFG_INC_MOD_LORA */

    {(char *) NULL,
     (char *) NULL,
     0,
     0,
     (void *) NULL}
};


/* --------------------------------- */
/* --- Terminal Helper functions --- */

/**
 * @brief A wrapper to ketCube_cfg_SaveStr function 
 *
 * Provides textual output to terminal is provided
 */
void ketCube_terminal_saveCfgHEXStr(char *data, ketCube_cfg_moduleIDs_t id,
                                    ketCube_cfg_AllocEEPROM_t addr,
                                    ketCube_cfg_LenEEPROM_t len)
{
    ketCube_cfg_Error_t ret;

    KETCUBE_TERMINAL_ENDL();

    ret = ketCube_cfg_SaveStr(data, id, addr, len);
    if (ret == KETCUBE_CFG_INV_HEX) {
        KETCUBE_TERMINAL_PRINTF("Invalid HEX number!");
        KETCUBE_TERMINAL_ENDL();
        return;
    } else if (ret == ketCube_cfg_Save_ERROR) {
        KETCUBE_TERMINAL_PRINTF("Write error!");
        KETCUBE_TERMINAL_ENDL();
        return;
    } else {
        KETCUBE_TERMINAL_PRINTF("Write success!");
    }

    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief A wrapper to ketCube_cfg_saveStr function 
 *
 * Provides textual output to terminal is provided
 */
void ketCube_terminal_saveCfgDECStr(char *data, ketCube_cfg_moduleIDs_t id,
                                    ketCube_cfg_AllocEEPROM_t addr,
                                    ketCube_cfg_LenEEPROM_t len)
{
    ketCube_cfg_Error_t ret;

    KETCUBE_TERMINAL_ENDL();

    if (ketCube_common_IsDecString(data, strlen(data)) == FALSE) {
        KETCUBE_TERMINAL_PRINTF("Invalid DEC number!");
        KETCUBE_TERMINAL_ENDL();
        return;
    } else if (len > sizeof(int32_t)) {
        KETCUBE_TERMINAL_PRINTF("Invalid number size!");
        KETCUBE_TERMINAL_ENDL();
        return;
    }

    int32_t target;
    ketCube_common_Dec2int(&target, data, strlen(data));

    ret = ketCube_cfg_Save((uint8_t *) & target, id, addr, len);
    if (ret == ketCube_cfg_Save_ERROR) {
        KETCUBE_TERMINAL_PRINTF("Write error!");
        KETCUBE_TERMINAL_ENDL();
        return;
    } else {
        KETCUBE_TERMINAL_PRINTF("Write success!");
    }

    KETCUBE_TERMINAL_ENDL();
}

/**
  *
  * @brief Prints command list at index/level
  * @param cmdIndex index of the first command to print
  * @param level level of commands to print
  *
  */
static void ketCube_terminal_printCmdList(uint16_t cmdIndex, uint8_t level)
{

    if (level > 0) {
        KETCUBE_TERMINAL_PRINTF("List of sub-commands for command %s: ",
                                ketCube_terminal_commands[cmdIndex -
                                                          1].cmd);
        KETCUBE_TERMINAL_ENDL();
    } else {
        KETCUBE_TERMINAL_PRINTF("List of commands: ");
        KETCUBE_TERMINAL_ENDL();
    }

    while (ketCube_terminal_commands[cmdIndex].cmd != NULL) {
        if (ketCube_terminal_commands[cmdIndex].cmdLevel == level) {
            KETCUBE_TERMINAL_PRINTF("\t%s\t%s",
                                    ketCube_terminal_commands
                                    [cmdIndex].cmd,
                                    ketCube_terminal_commands
                                    [cmdIndex].descr);
            KETCUBE_TERMINAL_ENDL();
        }

        if (ketCube_terminal_commands[cmdIndex].cmdLevel < level) {
            return;
        }
        cmdIndex++;
    }
}

/* ------------------------------ */
/* --- Core command callbacks --- */


void ketCube_terminal_cmd_help(void)
{
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("KETCube Command-line Interface HELP");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("-----------------------------------");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();

    ketCube_terminal_printCmdList(0, 0);

    KETCUBE_TERMINAL_ENDL();
}

void ketCube_terminal_cmd_about(void)
{
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("About KETCube");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("-------------");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();

    KETCUBE_TERMINAL_PRINTF
        ("KETCube was created on University of West Bohemia in Pilsen");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("and is provided under NCSA Open Source License - see LICENSE.txt");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("Copyright (c) 2018 University of West Bohemia in Pilsen");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("All rights reserved.");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("Developed by:");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("The SmartCampus Team");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("Department of Technologies and Measurement");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("www.smartcampus.cz | www.zcu.cz");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();

    KETCUBE_TERMINAL_PRINTF
        ("KETCube includes code provided by SEMTECH S.A.");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("under Revised BSD License - see LICENSE_SEMTECH.txt");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("Copyright (c) 2013, SEMTECH S.A.");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("All rights reserved.");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();

    KETCUBE_TERMINAL_PRINTF("KETCube includes code provided by");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("STMicroelectronics International N.V. - see LICENSE_STM.txt");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("Copyright (c) 2017 STMicroelectronics International N.V.");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("All rights reserved.");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Perform KETCube reset
 * 
 */
void ketCube_terminal_cmd_reload(void)
{
    KETCUBE_TERMINAL_ENDL();

    KETCUBE_TERMINAL_PRINTF
        ("Performing system reset and reloading KETCube configuration ...");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();

    NVIC_SystemReset();
}

/**
 * @brief Show available KETCube modules
 * 
 */
void ketCube_terminal_cmd_list(void)
{
    uint16_t i;
    ketCube_cfg_ModuleCfgByte_t data;

    KETCUBE_TERMINAL_ENDL();

    KETCUBE_TERMINAL_PRINTF
        ("Available modules (E == Enabled; D == Disabled):");
    KETCUBE_TERMINAL_ENDL();

    for (i = 0; i < ketCube_modules_CNT; i++) {
        if (ketCube_cfg_Load
            (((uint8_t *) & data), (ketCube_cfg_moduleIDs_t) i,
             (ketCube_cfg_AllocEEPROM_t) 0,
             (ketCube_cfg_LenEEPROM_t) 1) == KETCUBE_CFG_OK) {
            KETCUBE_TERMINAL_PRINTF("\t");
            if (data.enable != ketCube_modules_List[i].cfgByte.enable) {
                if (ketCube_modules_List[i].cfgByte.enable == TRUE) {
                    KETCUBE_TERMINAL_PRINTF("E -> ");
                } else {
                    KETCUBE_TERMINAL_PRINTF("D -> ");
                }
            }

            if (data.enable == TRUE) {
                KETCUBE_TERMINAL_PRINTF("E");
            } else {
                KETCUBE_TERMINAL_PRINTF("D");
            }

            KETCUBE_TERMINAL_PRINTF("\t%s\t%s",
                                    ketCube_modules_List[i].name,
                                    ketCube_modules_List[i].descr);
            KETCUBE_TERMINAL_ENDL();
        } else {
            KETCUBE_TERMINAL_PRINTF
                ("Error while reading module configuration!");
        }

    }

    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Enable/disable given KETCube module
 *
 * @param enable enable given module
 * 
 */
void ketCube_terminal_cmd_enableDisable(bool enable)
{
    uint8_t j;
    uint16_t i;
    ketCube_cfg_ModuleCfgByte_t tmpCfgByte;

    for (i = 0; i < ketCube_modules_CNT; i++) {
        j = 0;
        while (ketCube_modules_List[i].name[j] != 0x00) {
            if (ketCube_modules_List[i].name[j] !=
                commandBuffer[commandParams + j]) {
                break;
            }
            j++;
        }
        if (ketCube_modules_List[i].name[j] == 0x00) {
            break;
        }
    }

    if (i == ketCube_modules_CNT) {
        KETCUBE_TERMINAL_PRINTF("Invalid module name!");
        return;
    }

    KETCUBE_TERMINAL_PRINTF("Setting module %s:",
                            ketCube_modules_List[i].name);

    //do not enable/disable now but when reload ...
    tmpCfgByte = ketCube_modules_List[i].cfgByte;
    tmpCfgByte.enable = enable; // enable/disable

    if (ketCube_cfg_Save
        (((uint8_t *) & (tmpCfgByte)), (ketCube_cfg_moduleIDs_t) i,
         (ketCube_cfg_AllocEEPROM_t) 0,
         (ketCube_cfg_LenEEPROM_t) 1) == KETCUBE_CFG_OK) {
        KETCUBE_TERMINAL_PRINTF("\t success!");
    } else {
        KETCUBE_TERMINAL_PRINTF("\t error!");
    }
}


/**
 * @brief Enable given KETCube module
 * 
 */
void ketCube_terminal_cmd_enable(void)
{
    KETCUBE_TERMINAL_ENDL();
    ketCube_terminal_cmd_enableDisable(TRUE);
    KETCUBE_TERMINAL_ENDL();
}


/**
 * @brief Enable given KETCube module
 * 
 */
void ketCube_terminal_cmd_disable(void)
{
    KETCUBE_TERMINAL_ENDL();
    ketCube_terminal_cmd_enableDisable(FALSE);
    KETCUBE_TERMINAL_ENDL();
}

/**
  * @brief Init terminal
  *
  */
void ketCube_terminal_Init(void)
{
    commandBuffer[0] = 0x00;

    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("Welcome to KETCube Command-line Interface");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("-----------------------------------------");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("Use [TAB] key to show build-in help for current command");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("Use [ENTER] key to execute current command");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("Use [+]/[-] keys to browse command hostory");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();

    ketCube_terminal_printCmdList(0, 0);

    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PROMPT();
}

/**
  * @brief Execute command-in-buffer
  *
  */
void ketCube_terminal_execCMD(void)
{
    uint16_t cmdIndex, j, cmdBuffIndex;
    bool eq;
    uint8_t cmdLevel = 0;

    KETCUBE_TERMINAL_ENDL();

    if (*commandPtr == 0) {
        KETCUBE_TERMINAL_PROMPT();
        return;
    }

    cmdIndex = 0;
    cmdBuffIndex = 0;

    while (ketCube_terminal_commands[cmdIndex].cmd != NULL) {

        // skip non actual levels
        if (ketCube_terminal_commands[cmdIndex].cmdLevel > cmdLevel) {
            cmdIndex++;
            continue;
        } else if (ketCube_terminal_commands[cmdIndex].cmdLevel < cmdLevel) {
            break;
        }

        eq = TRUE;
        j = 0;

        while ((ketCube_terminal_commands[cmdIndex].cmd[j] != 0x00)
               && (*commandPtr > cmdBuffIndex)) {
            /* compare command characters */
            if (ketCube_terminal_commands[cmdIndex].cmd[j] !=
                commandBuffer[cmdBuffIndex]) {
                eq = FALSE;
            }
            j++;
            cmdBuffIndex++;
        }

        /* cause mismatch for command substrings */
        if ((eq == TRUE) && (*commandPtr > cmdBuffIndex)
            && (commandBuffer[cmdBuffIndex] != 0x00)
            && (commandBuffer[cmdBuffIndex] != ' ')) {
            eq = FALSE;
        }

        /* move to next command if mismatch */
        if (eq == FALSE) {
            cmdBuffIndex -= j;
            cmdIndex++;
            continue;
        }

        /* move to subcommand if exist */
        if ((ketCube_terminal_commands[cmdIndex].cmd[j] == 0x00) &&
            (commandBuffer[cmdBuffIndex] != 0x00) &&
            (ketCube_terminal_commands[cmdIndex].cmdLevel <
             ketCube_terminal_commands[cmdIndex + 1].cmdLevel)) {
            cmdBuffIndex++;     // remove space character: ' '
            cmdIndex++;
            cmdLevel++;
            continue;
        }

        /* execute command */
        if ((eq == TRUE)
            && (ketCube_terminal_commands[cmdIndex].cmd[j] == 0x00)
            && ((commandBuffer[cmdBuffIndex] == 0x00)
                || ((commandBuffer[cmdBuffIndex] == ' ')
                    && (ketCube_terminal_commands[cmdIndex].paramCnt !=
                        0)))) {
            commandParams = cmdBuffIndex + 1;
            if (ketCube_terminal_commands[cmdIndex].callback != NULL) {
                KETCUBE_TERMINAL_PRINTF("Executing command: %s",
                                        ketCube_terminal_commands
                                        [cmdIndex].cmd);
                KETCUBE_TERMINAL_ENDL();
                ketCube_terminal_commands[cmdIndex].callback();
            } else {
                KETCUBE_TERMINAL_PRINTF("Help for command: %s",
                                        ketCube_terminal_commands
                                        [cmdIndex].cmd);
                KETCUBE_TERMINAL_ENDL();
                ketCube_terminal_printCmdList(cmdIndex + 1,
                                              ketCube_terminal_commands
                                              [cmdIndex].cmdLevel + 1);
            }

            KETCUBE_TERMINAL_PROMPT();

            commandHistoryPtr =
                (commandHistoryPtr + 1) % KETCUBE_TERMINAL_HISTORY_LEN;
            commandBuffer =
                (char *) &(commandHistory[commandHistoryPtr].buffer[0]);
            commandPtr =
                (uint8_t *) & (commandHistory[commandHistoryPtr].ptr);
            *commandPtr = 0;

            return;
        }
        cmdIndex++;
    }
    KETCUBE_TERMINAL_PRINTF("Command not found!");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PROMPT();

    commandHistoryPtr =
        (commandHistoryPtr + 1) % KETCUBE_TERMINAL_HISTORY_LEN;
    commandBuffer =
        (char *) &(commandHistory[commandHistoryPtr].buffer[0]);
    commandPtr = (uint8_t *) & (commandHistory[commandHistoryPtr].ptr);
    *commandPtr = 0;

    return;
}

/**
  * @brief Print help related to the current user input
  *
  */
void ketCube_terminal_printCMDHelp(void)
{
    uint16_t cmdIndex, j, cmdBuffIndex, k, l;
    bool eq;
    uint8_t cmdLevel = 0;

    KETCUBE_TERMINAL_ENDL();

    if (*commandPtr == 0) {
        KETCUBE_TERMINAL_PROMPT();
        return;
    }

    cmdIndex = 0;
    cmdBuffIndex = 0;

    while (ketCube_terminal_commands[cmdIndex].cmd != NULL) {

        // skip non actual levels
        if (ketCube_terminal_commands[cmdIndex].cmdLevel > cmdLevel) {
            cmdIndex++;
            continue;
        } else if (ketCube_terminal_commands[cmdIndex].cmdLevel < cmdLevel) {
            break;
        }

        eq = TRUE;
        j = 0;

        while ((ketCube_terminal_commands[cmdIndex].cmd[j] != 0x00)
               && (*commandPtr > cmdBuffIndex)) {
            /* compare command characters */
            if (ketCube_terminal_commands[cmdIndex].cmd[j] !=
                commandBuffer[cmdBuffIndex]) {
                eq = FALSE;
            }
            j++;
            cmdBuffIndex++;
        }

        /* cause mismatch for command substrings */
        if ((eq == TRUE) && (*commandPtr > cmdBuffIndex)
            && (commandBuffer[cmdBuffIndex] != 0x00)
            && (commandBuffer[cmdBuffIndex] != ' ')) {
            eq = FALSE;
        }

        /* move to next command if mismatch */
        if (eq == FALSE) {
            cmdBuffIndex -= j;
            cmdIndex++;
            continue;
        }

        /* move to subcommand if exist */
        if ((ketCube_terminal_commands[cmdIndex].cmd[j] == 0x00) &&
            (commandBuffer[cmdBuffIndex] != 0x00) &&
            (ketCube_terminal_commands[cmdIndex].cmdLevel <
             ketCube_terminal_commands[cmdIndex + 1].cmdLevel)) {
            cmdBuffIndex++;     // remove space character: ' '
            cmdIndex++;
            cmdLevel++;
            continue;
        }

        if ((eq == TRUE)
            && (ketCube_terminal_commands[cmdIndex].cmd[j] == 0x00)
            && ((commandBuffer[cmdBuffIndex] == 0x00))) {
            KETCUBE_TERMINAL_PRINTF("Help for command %s: \t%s",
                                    ketCube_terminal_commands
                                    [cmdIndex].cmd,
                                    ketCube_terminal_commands
                                    [cmdIndex].descr);
            KETCUBE_TERMINAL_ENDL();
            ketCube_terminal_printCmdList(cmdIndex + 1,
                                          ketCube_terminal_commands
                                          [cmdIndex].cmdLevel + 1);
            break;
        } else if (eq == TRUE) {
            KETCUBE_TERMINAL_PRINTF("Available commands: ");
            KETCUBE_TERMINAL_ENDL();

            k = cmdIndex;
            while ((ketCube_terminal_commands[k].cmd != NULL) &&
                   (ketCube_terminal_commands[k].cmdLevel >=
                    ketCube_terminal_commands[cmdIndex].cmdLevel)) {
                if (ketCube_terminal_commands[k].cmdLevel ==
                    ketCube_terminal_commands[cmdIndex].cmdLevel) {
                    eq = TRUE;
                    for (l = 0; l < j; l++) {
                        if (ketCube_terminal_commands[cmdIndex].cmd[l] !=
                            ketCube_terminal_commands[k].cmd[l]) {
                            eq = FALSE;
                        }
                    }
                    if (eq == TRUE) {
                        KETCUBE_TERMINAL_PRINTF("\t%s",
                                                ketCube_terminal_commands
                                                [k].cmd);
                        KETCUBE_TERMINAL_ENDL();
                    }
                }
                k++;
            }
            break;
        }
        cmdIndex++;
    }

    ketCube_terminal_UpdateCmdLine();
}

/**
  * @brief Print current unfinished command 
  *
  * Use when it is necessary to print something and not to disrupt
  *
  */
void ketCube_terminal_UpdateCmdLine(void)
{
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PROMPT();
    KETCUBE_TERMINAL_PRINTF("%s", commandBuffer);
}

/**
  * @brief Process user input
  *
  */
void ketCube_terminal_ProcessCMD(void)
{
    char tmpchar;
    uint8_t i;

    /* Process all commands */
    while (IsNewCharReceived() == TRUE) {
        tmpchar = GetNewChar();

        if (((tmpchar >= 'a') && (tmpchar <= 'z'))
            || ((tmpchar >= 'A') && (tmpchar <= 'Z')) || ((tmpchar >= '0')
                                                          && (tmpchar <=
                                                              '9'))
            || (tmpchar == ' ')) {
            if (*commandPtr > KETCUBE_TERMINAL_CMD_MAX_LEN) {
                KETCUBE_TERMINAL_PRINTF
                    ("Command too long, remove characters or press [ENTER] to exec command!");
                KETCUBE_TERMINAL_ENDL();
                KETCUBE_TERMINAL_PROMPT();
                KETCUBE_TERMINAL_PRINTF("%s", &(commandBuffer[0]));
            }

            commandBuffer[*commandPtr] = tmpchar;
            *commandPtr = (*commandPtr + 1);
            commandBuffer[*commandPtr] = 0x00;

#if KETCUBE_TERMINAL_ECHO == TRUE
            KETCUBE_TERMINAL_PRINTF("%c", tmpchar);
#endif
        } else if ((tmpchar == '\b') || (tmpchar == 127)) {     // delete char
            if (*commandPtr > 0) {
                *commandPtr = (*commandPtr - 1);
                commandBuffer[*commandPtr] = 0x00;
                KETCUBE_TERMINAL_PRINTF("\b \b");
            }
        } else if ((tmpchar == '\n') || (tmpchar == '\r')) {    // end of command
            ketCube_terminal_execCMD();
        } else if (tmpchar == '\t') {   // display help
            ketCube_terminal_printCMDHelp();
        } else if (tmpchar == '+') {    // up history
            KETCUBE_TERMINAL_PRINTF("\r");
            for (i = 0; i < *commandPtr; i++) {
                KETCUBE_TERMINAL_PRINTF(" ");
            }
            KETCUBE_TERMINAL_PRINTF("\r");

            commandHistoryPtr =
                (commandHistoryPtr + 1) % KETCUBE_TERMINAL_HISTORY_LEN;
            if (commandHistoryPtr == 0) {
                commandHistoryPtr = KETCUBE_TERMINAL_HISTORY_LEN - 1;
            }
            commandBuffer =
                (char *) &(commandHistory[commandHistoryPtr].buffer[0]);
            commandPtr =
                (uint8_t *) & (commandHistory[commandHistoryPtr].ptr);
            KETCUBE_TERMINAL_PROMPT();
            KETCUBE_TERMINAL_PRINTF("%s", commandBuffer);
        } else if (tmpchar == '-') {    // down history
            KETCUBE_TERMINAL_PRINTF("\r");
            for (i = 0; i < *commandPtr; i++) {
                KETCUBE_TERMINAL_PRINTF(" ");
            }
            KETCUBE_TERMINAL_PRINTF("\r");

            commandHistoryPtr = commandHistoryPtr - 1;
            if (commandHistoryPtr >= KETCUBE_TERMINAL_HISTORY_LEN) {
                commandHistoryPtr = 0;
            }
            commandBuffer =
                (char *) &(commandHistory[commandHistoryPtr].buffer[0]);
            commandPtr =
                (uint8_t *) & (commandHistory[commandHistoryPtr].ptr);
            KETCUBE_TERMINAL_PROMPT();
            KETCUBE_TERMINAL_PRINTF("%s", commandBuffer);
        } else {                // ignored characters: '\r' , ...
            //KETCUBE_TERMINAL_PRINTF("X >> %d ", (int) tmpchar);
            continue;
        }
    }
}

/**
  * @brief Print line to serial line + newline
  *
  */
void ketCube_terminal_Println(char *format, ...)
{
    char buff[128];

    va_list args;
    va_start(args, format);

    vsprintf(&(buff[0]), format, args);

    va_end(args);

    KETCUBE_TERMINAL_PRINTF("%s", &(buff[0]));
    ketCube_terminal_UpdateCmdLine();
}


/**
  * @brief Print format to serial line
  *
  */
void ketCube_terminal_Print(char *format, ...)
{
    char buff[128];

    va_list args;
    va_start(args, format);

    vsprintf(&(buff[0]), format, args);

    va_end(args);

    KETCUBE_TERMINAL_PRINTF("%s", &(buff[0]));
}


/**
  * @brief Print Debug info to serial line + newline
  *
  */
void ketCube_terminal_DebugPrintln(char *format, ...)
{
    char buff[128];

    if (ketCube_modules_List[KETCUBE_LISTS_MODULEID_DEBUGDISPLAY].
        cfgByte.enable != TRUE) {
        return;
    }

    va_list args;
    va_start(args, format);

    vsprintf(&(buff[0]), format, args);

    va_end(args);

    KETCUBE_TERMINAL_PRINTF("%s", &(buff[0]));
    ketCube_terminal_UpdateCmdLine();
}


/**
  * @brief Print Debug info to serial line
  *
  */
void ketCube_terminal_DebugPrint(char *format, ...)
{
    char buff[128];

    if (ketCube_modules_List[KETCUBE_LISTS_MODULEID_DEBUGDISPLAY].
        cfgByte.enable != TRUE) {
        return;
    }

    va_list args;
    va_start(args, format);

    vsprintf(&(buff[0]), format, args);

    va_end(args);

    KETCUBE_TERMINAL_PRINTF("%s", &(buff[0]));
}


/* -------------------------- */
/* --- Core configuration --- */

/**
 * @brief Show KETCube base period
 * 
 */
void ketCube_terminal_cmd_show_core_basePeriod(void)
{
    KETCUBE_TERMINAL_PRINTF("KETCube Core base period is: %d ms",
                            ketCube_coreCfg_BasePeriod);
    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Show KETCube start delay
 * 
 */
void ketCube_terminal_cmd_show_core_startDelay(void)
{
    KETCUBE_TERMINAL_PRINTF("KETCube Start delay is: %d ms",
                            ketCube_coreCfg_StartDelay);
    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Set KETCube base period
 * 
 */
void ketCube_terminal_cmd_set_core_basePeriod(void)
{
    uint32_t value;
    char *endptr;

    value = strtol(&(commandBuffer[commandParams]), &endptr, 10);

    if (ketCube_EEPROM_WriteBuffer
        (KETCUBE_EEPROM_ALLOC_CORE + KETCUBE_CORECFG_ADR_BASEPERIOD,
         (uint8_t *) & (value), 4) == KETCUBE_EEPROM_OK) {
        KETCUBE_TERMINAL_PRINTF
            ("KETCube Core base period is set to: %d ms", value);
    } else {
        KETCUBE_TERMINAL_PRINTF("KETCube Core base period write error!");
    }
    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Set KETCube start delay
 * 
 */
void ketCube_terminal_cmd_set_core_startDelay(void)
{
    uint32_t value;
    char *endptr;

    value = strtol(&(commandBuffer[commandParams]), &endptr, 10);

    if (ketCube_EEPROM_WriteBuffer
        (KETCUBE_EEPROM_ALLOC_CORE + KETCUBE_CORECFG_ADR_STARTDELAY,
         (uint8_t *) & (value), 4) == KETCUBE_EEPROM_OK) {
        KETCUBE_TERMINAL_PRINTF("KETCube Start delay is set to: %d ms",
                                value);
    } else {
        KETCUBE_TERMINAL_PRINTF("KETCube Start delay write error!");
    }
    KETCUBE_TERMINAL_ENDL();
}
