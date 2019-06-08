/**
 * @file    ketCube_terminal_common.c
 * @author  Martin Ubl
 * @version 0.1
 * @date    2019-03-30
 * @brief   This file contains the KETCube Terminal shared definitions
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
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "utilities.h"
#include "ketCube_terminal_common.h"
#include "ketCube_modules.h"
#include "ketCube_coreCfg.h"

/* Core command callback prototypes */
static void ketCube_terminal_cmd_reload(void);
static void ketCube_terminal_cmd_list(void);
static void ketCube_terminal_cmd_enable(void);
static void ketCube_terminal_cmd_disable(void);

// List of KETCube commands
#include "../../Projects/src/ketCube_cmdList.c" // include a project-specific file

ketCube_terminal_paramSet_t commandIOParams;
ketCube_terminal_command_errorCode_t commandErrorCode;


/* --------------------------------- */
/* --- Terminal Helper functions --- */

/**
  *
  * @brief Prints command list at index/level
  * 
  * @param cmdIndex index of the first command to print
  * @param level level of commands to print
  * @param contextFlags comamnd flag context
  */
void ketCube_terminal_printCmdList(ketCube_terminal_cmd_t * parent,
                                   ketCube_terminal_cmd_t * cmdList,
                                   ketCube_terminal_command_flags_t * contextFlags)
{
    int cmdIndex = 0;
    ketCube_terminal_command_flags_t localContext;
    
    if (parent != NULL &&
        (parent->flags.isGroup == FALSE))
    {
        KETCUBE_TERMINAL_PRINTF("Description: %s", parent->descr);
        KETCUBE_TERMINAL_ENDL();
        return;
    }

    if (parent != NULL) {
        KETCUBE_TERMINAL_PRINTF("List of sub-commands for command %s: ",
                                parent->cmd);
        KETCUBE_TERMINAL_ENDL();
    } else {
        KETCUBE_TERMINAL_PRINTF("List of commands: ");
        KETCUBE_TERMINAL_ENDL();
    }

    while ((cmdList[cmdIndex].cmd != NULL)) {
        ketCube_terminal_andCmdFlags(&localContext, contextFlags, &(cmdList[cmdIndex].flags));
        if (ketCube_terminal_checkCmdSubtreeContext(&localContext) == FALSE) {
            cmdIndex++;
            continue;
        }
        
        KETCUBE_TERMINAL_PRINTF("\t%s\t%s",
                                cmdList[cmdIndex].cmd,
                                cmdList[cmdIndex].descr);
        KETCUBE_TERMINAL_ENDL();
        cmdIndex++;
    }
}

/* ------------------------------ */
/* --- Core command callbacks --- */


void ketCube_terminal_cmd_help(void)
{
    ketCube_terminal_command_flags_t context = {
        .isGroup  = TRUE,
        .isLocal  = TRUE,
        .isEnvCmd = TRUE,
        .isEEPROM = TRUE,
        .isRAM    = TRUE,
    };
    
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("%s Command-line Interface HELP",
                            KETCUBE_CFG_DEV_NAME);
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("-----------------------------------");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();

    ketCube_terminal_printCmdList(NULL, ketCube_terminal_commands, &context);

    KETCUBE_TERMINAL_ENDL();
}

void ketCube_terminal_cmd_about(void)
{
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("About %s", KETCUBE_CFG_DEV_NAME);
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("-------------");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();

    KETCUBE_TERMINAL_PRINTF
        ("KETCube was created on University of West Bohemia in Pilsen.");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("KETCube home: https://github.com/SmartCAMPUSZCU/KETCube-docs");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("KETCube is provided under NCSA Open Source License - see LICENSE.txt.");
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
        ("Performing system reset and reloading %s configuration ...",
         KETCUBE_CFG_DEV_NAME);
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
    char severityEEPROM = 'N';
    char severity = 'N';

    KETCUBE_TERMINAL_ENDL();

    KETCUBE_TERMINAL_PRINTF("Available modules:");
    KETCUBE_TERMINAL_ENDL();

    for (i = KETCUBE_LISTS_MODULEID_FIRST; i < ketCube_modules_CNT; i++) {
        if (ketCube_cfg_Load
            (((uint8_t *) & data), (ketCube_cfg_moduleIDs_t) i,
             (ketCube_cfg_AllocEEPROM_t) 0,
             (ketCube_cfg_LenEEPROM_t) 1) == KETCUBE_CFG_OK) {

            switch (data.severity) {
            case KETCUBE_CFG_SEVERITY_NONE:
                severity = 'N';
                break;
            case KETCUBE_CFG_SEVERITY_ERROR:
                severity = 'R';
                break;
            case KETCUBE_CFG_SEVERITY_INFO:
                severity = 'I';
                break;
            case KETCUBE_CFG_SEVERITY_DEBUG:
                severity = 'D';
                break;
            }

            switch (ketCube_modules_List[i].cfgPtr->severity) {
            case KETCUBE_CFG_SEVERITY_NONE:
                severityEEPROM = 'N';
                break;
            case KETCUBE_CFG_SEVERITY_ERROR:
                severityEEPROM = 'R';
                break;
            case KETCUBE_CFG_SEVERITY_INFO:
                severityEEPROM = 'I';
                break;
            case KETCUBE_CFG_SEVERITY_DEBUG:
                severityEEPROM = 'D';
                break;
            }

            if (severityEEPROM != severity) {
                KETCUBE_TERMINAL_PRINTF("%c -> %c\t", severityEEPROM,
                                        severity);
            } else {
                KETCUBE_TERMINAL_PRINTF("  %c\t", severity);
            }

            if (data.enable != ketCube_modules_List[i].cfgPtr->enable) {
                if (ketCube_modules_List[i].cfgPtr->enable == TRUE) {
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

    KETCUBE_TERMINAL_PRINTF
        ("Module State: E == Module Enabled; D == Module Disabled");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("Module severity: N = %s, R = %s; I = %s; D = %s",
            ketCube_severity_strAlias[KETCUBE_CFG_SEVERITY_NONE],
            ketCube_severity_strAlias[KETCUBE_CFG_SEVERITY_ERROR],
            ketCube_severity_strAlias[KETCUBE_CFG_SEVERITY_INFO],
            ketCube_severity_strAlias[KETCUBE_CFG_SEVERITY_DEBUG]);


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
    uint16_t i;
    ketCube_cfg_ModuleCfgByte_t tmpCfgByte;

    for (i = KETCUBE_LISTS_MODULEID_FIRST; i < ketCube_modules_CNT; i++) {
        if (ketCube_modules_List[i].id == commandIOParams.as_module_id.module_id) {
            break;
        }
    }

    if (i == ketCube_modules_CNT) {
        //KETCUBE_TERMINAL_PRINTF("Invalid module name!");
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_INVALID_PARAMS;
        return;
    }

    //KETCUBE_TERMINAL_PRINTF("Setting module %s:", ketCube_modules_List[i].name);

    //do not enable/disable now but when reload ...
    tmpCfgByte = *(ketCube_modules_List[i].cfgPtr);
    tmpCfgByte.enable = enable; // enable/disable
    tmpCfgByte.severity = commandIOParams.as_module_id.severity; // set severity

    if (ketCube_cfg_Save
        (((uint8_t *) & (tmpCfgByte)), (ketCube_cfg_moduleIDs_t) i,
         (ketCube_cfg_AllocEEPROM_t) 0,
         (ketCube_cfg_LenEEPROM_t) 1) == KETCUBE_CFG_OK) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_OK;
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
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
 * @brief Check if the command definition is valid in given subtree context
 * 
 * This function returns TRUE, if the command is in a subtree context that is
 * valid for given flags
 * 
 * @param contextFlags pointer to context flags
 * 
 * @retval TRUE if success, else return FALSE
 * 
 */
bool ketCube_terminal_checkCmdSubtreeContext(
    ketCube_terminal_command_flags_t* flags)
{
    if ((flags->isLocal || flags->isRemote) == TRUE) {
        if (flags->isEnvCmd == TRUE) {
            return TRUE;
        } 
        
        if ((flags->isSetCmd || flags->isShowCmd)
             && (flags->isRAM || flags->isEEPROM)) {
            return TRUE;
        }
    }
    
    return FALSE;
}

/**
 * @brief Check if we can run given command in given context
 * 
 * @retval TRUE if success, else return FALSE
 * 
 */
ketCube_terminal_cmdCheckResult_t ketCube_terminal_checkCmdContext(
    ketCube_terminal_cmd_t* command, bool local)
{
    if (ketCube_terminal_checkCmdSubtreeContext(&(command->flags)) == FALSE) {
        return KETCUBE_CMD_CHECK_FAILED_DEFINITION;
    }
    
    // note: if checkCmdSubtreeContext succeeds, the command is guaranteed to be
    //       either local or remote or both
    
    /* cannot execute remote-only commands in local terminal */
    if (local && command->flags.isLocal == FALSE) {
        return KETCUBE_CMD_CHECK_FAILED_CONTEXT_ONLY_REMOTE;
    }
    /* cannot execute local-only commands in remote terminal */
    if (!local && command->flags.isRemote == FALSE) {
        return KETCUBE_CMD_CHECK_FAILED_CONTEXT_ONLY_LOCAL;
    }
    
    /* add more flag checks here in the future */
    
    return KETCUBE_CMD_CHECK_OK;
}

/**
 * @brief Generic GET EEPROM cfg data
 *
 * @param cmdDescrPtr pointer to a command descriptor
 * 
 */
static void ketCube_terminal_getEEPROMCfg(ketCube_terminal_cmd_t * cmdDescrPtr)
{
    if (cmdDescrPtr->settingsPtr.cfgVarPtr->size == 0) {
        return;
    }
    
    // load data from EEPROM as byte array
    if (ketCube_cfg_Load((uint8_t *) &(commandIOParams.as_byte_array.data[0]),
                         (ketCube_cfg_moduleIDs_t) cmdDescrPtr->settingsPtr.cfgVarPtr->moduleID,
                         (ketCube_cfg_AllocEEPROM_t) cmdDescrPtr->settingsPtr.cfgVarPtr->offset,
                         (ketCube_cfg_LenEEPROM_t) cmdDescrPtr->settingsPtr.cfgVarPtr->size) != KETCUBE_CFG_OK) {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
        return;
    }
    
    // convert to output data type
    switch (cmdDescrPtr->outputSetType) {

        case KETCUBE_TERMINAL_PARAMS_NONE:
        case KETCUBE_TERMINAL_PARAMS_BYTE:
        case KETCUBE_TERMINAL_PARAMS_MODULEID:
        case KETCUBE_TERMINAL_PARAMS_INT32:
        case KETCUBE_TERMINAL_PARAMS_UINT32:
        case KETCUBE_TERMINAL_PARAMS_INT32_PAIR:
        default:
            // do nothing ...
            break;
        case KETCUBE_TERMINAL_PARAMS_BOOLEAN:
            if (commandIOParams.as_byte_array.data[0] != 0x00) {
                commandIOParams.as_bool = TRUE;
            } else {
                commandIOParams.as_bool = FALSE;
            }
            break;
        case KETCUBE_TERMINAL_PARAMS_STRING:
            // just to be sure
            commandIOParams.as_string[KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH - 1] = '\0';
            break;
        case KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY:
            commandIOParams.as_byte_array.length = cmdDescrPtr->settingsPtr.cfgVarPtr->size;
            break;
    }
}

/**
 * @brief Generic SET EEPROM data
 *
 * @param cmdDescrPtr pointer to a command descriptor
 * 
 */
static void ketCube_terminal_setEEPROMCfg(ketCube_terminal_cmd_t * cmdDescrPtr)
{
    if (cmdDescrPtr->settingsPtr.cfgVarPtr->size == 0) {
        return;
    }

    if (ketCube_cfg_Save((uint8_t *) &(commandIOParams.as_byte_array.data[0]),
                         (ketCube_cfg_moduleIDs_t) cmdDescrPtr->settingsPtr.cfgVarPtr->moduleID,
                         (ketCube_cfg_AllocEEPROM_t) cmdDescrPtr->settingsPtr.cfgVarPtr->offset,
                         (ketCube_cfg_LenEEPROM_t) cmdDescrPtr->settingsPtr.cfgVarPtr->size) != KETCUBE_CFG_OK) {

        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
        return;
    }
}


/**
 * @brief Generic GET RAM cfg data
 *
 * @param cmdDescrPtr pointer to a command descriptor
 * 
 */
static void ketCube_terminal_getRAMCfg(ketCube_terminal_cmd_t * cmdDescrPtr)
{
    if (cmdDescrPtr->settingsPtr.cfgVarPtr->size == 0) {
        return;
    }

    memcpy((uint8_t *) &(commandIOParams.as_byte_array.data[0]), 
           (uint8_t *) (ketCube_modules_List[cmdDescrPtr->settingsPtr.cfgVarPtr->moduleID].cfgPtr + cmdDescrPtr->settingsPtr.cfgVarPtr->offset),
           (uint32_t) cmdDescrPtr->settingsPtr.cfgVarPtr->size);

     // convert to output data type
    switch (cmdDescrPtr->outputSetType)
    {
        case KETCUBE_TERMINAL_PARAMS_NONE:
        case KETCUBE_TERMINAL_PARAMS_BYTE:
        case KETCUBE_TERMINAL_PARAMS_MODULEID:
        case KETCUBE_TERMINAL_PARAMS_INT32:
        case KETCUBE_TERMINAL_PARAMS_UINT32:
        case KETCUBE_TERMINAL_PARAMS_INT32_PAIR:
        default:
            // do nothing ...
            break;
        case KETCUBE_TERMINAL_PARAMS_BOOLEAN:
            if (commandIOParams.as_byte_array.data[0] != 0x00) {
                commandIOParams.as_bool = TRUE;
            } else {
                commandIOParams.as_bool = FALSE;
            }
            break;
        case KETCUBE_TERMINAL_PARAMS_STRING:
            // just for sure
            commandIOParams.as_string[KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH - 1] = '\0';
            break;
        case KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY:
            commandIOParams.as_byte_array.length = cmdDescrPtr->settingsPtr.cfgVarPtr->size;
            break;
    }
}

/**
 * @brief Generic SET RAM data
 *
 * @param cmdDescrPtr pointer to a command descriptor
 * 
 */
static void ketCube_terminal_setRAMCfg(ketCube_terminal_cmd_t * cmdDescrPtr)
{
    if (cmdDescrPtr->settingsPtr.cfgVarPtr->size == 0) {
        return;
    }

    memcpy((uint8_t *) (ketCube_modules_List[cmdDescrPtr->settingsPtr.cfgVarPtr->moduleID].cfgPtr + cmdDescrPtr->settingsPtr.cfgVarPtr->offset),
           (uint8_t *) &(commandIOParams.as_byte_array.data[0]), 
           (uint32_t) cmdDescrPtr->settingsPtr.cfgVarPtr->size);
}

void ketCube_terminal_execute(ketCube_terminal_cmd_t* cmd,
    ketCube_terminal_command_flags_t activeFlags)
{
    commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_OK;
                    
    /* Execute specific command callback or generic callback */
    if (activeFlags.isGeneric == TRUE) {
        if (activeFlags.isSetCmd == TRUE) {
            // run generic SET
            if (activeFlags.isEEPROM == TRUE) {
                ketCube_terminal_setEEPROMCfg(cmd);
            }
            if (activeFlags.isRAM == TRUE) {
                ketCube_terminal_setRAMCfg(cmd);
            }
        } else if (activeFlags.isShowCmd == TRUE) {
            // run generic SET
            if (activeFlags.isEEPROM == TRUE) {
                ketCube_terminal_getEEPROMCfg(cmd);
            }
            if (activeFlags.isRAM == TRUE) {
                ketCube_terminal_getRAMCfg(cmd);
            }
        }
    } else {
        cmd->settingsPtr.callback();
    }
}
