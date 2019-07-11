/**
 * @file    ketCube_core_cmd.c
 * @author  Martin Ubl
 * @version 0.1
 * @date    2019-01-01
 * @brief   This file contains the KETCube core commandline definitions
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
#ifndef __KETCUBE_CORE_CMD_H
#define __KETCUBE_CORE_CMD_H

#include <stddef.h>

#include "ketCube_cfg.h"
#include "ketCube_common.h"

/** @defgroup KETCube_core_CMD KETCube core CMD
  * @brief KETCube core commandline definitions
  * @ingroup KETCube_Terminal
  * @{
  */


/**
 * @brief Erase EEPROM configuration - set factory defaults
 * 
 */
void ketCube_core_CMD_FactoryDefaults(void)
{
    uint8_t i;
    uint16_t addr = KETCUBE_EEPROM_ALLOC_MODULES;

    for (i = 0; i < ketCube_modules_CNT; i++) {
        
        // save zeroes to EEPROM
        if (ketCube_cfg_SetDefaults((ketCube_cfg_moduleIDs_t) i,
                                    (ketCube_cfg_AllocEEPROM_t) 0,
                                    ketCube_modules_List[i].cfgLen) == KETCUBE_CFG_OK) {
        } else {
             ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_ERROR, "Unable to restore factory defaults!");
            return;
        }
        
        // set current EEPROM pointer
        addr += ketCube_modules_List[i].cfgLen;
    }
    
    ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO, "KETCube was set to factory defaults!");
    ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO, "Reload to apply new settings!");
}

/**
 * @brief Initialize STM32 MCU to allow bootloader startup 
 * 
 * This allows KETCube flash programming over communication interface(s)
 * 
 * TODO some of Println() call do not make sense ... requires some rewriting
 * 
 */
void ketCube_core_CMD_startBootloader(void) {
    FLASH_EraseInitTypeDef EraseInitStruct; 
    uint32_t SECTORError = 0;
    FLASH_AdvOBProgramInitTypeDef pAdvOBInit;
    
    ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO, "Note, that this operation causes firmware malfunction!");
    
    /* Introduce small amount of delay here to be sure, that above note will successfully print */
    HAL_Delay(2000);
    
    /* Unlock Flash */
    HAL_FLASH_Unlock();
    
    /* Erase page ... */
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = FLASH_BANK2_BASE;
    EraseInitStruct.NbPages = 1;
    
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK) {
        ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_ERROR, "Unable to erase BANK 2 START!");
        return;
    }
    
    /* Write invalid data to BANK START addresses */
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_BANK2_BASE, 0xFFFFFFFF) != HAL_OK) {
        ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_ERROR, "Unable to init BANK 1!");
        HAL_FLASH_Lock();
        return;
    }
    
    /* Erase page ... */
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = FLASH_BASE;
    EraseInitStruct.NbPages = 1;
    
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK) {
        ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_ERROR, "Unable to erase BANK 1 START!");
        HAL_FLASH_Lock();
        return;
    }
    
    /* Write invalid data to BANK START addresses */
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_BASE, 0xFFFFFFFF) != HAL_OK) {
        ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_ERROR, "Unable to init BANK 1!");
        HAL_FLASH_Lock();
        return;
    }

    /* Get error cause*/
    /* uint32_t status = HAL_FLASH_GetError();
    ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO, "FE: %d", status);*/
    
    pAdvOBInit.OptionType = OPTIONBYTE_BOOTCONFIG;
    pAdvOBInit.BootConfig = OB_BOOT_BANK2;
    HAL_FLASH_OB_Unlock();
    if (HAL_FLASHEx_AdvOBProgram(&pAdvOBInit) != HAL_OK) {
        HAL_FLASH_OB_Lock();
        ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_ERROR, "Unable to change BOOT settings (BFB2)!");
        HAL_FLASH_Lock();
        return;
    }
    
    /* Commit OB change  */
    HAL_FLASH_OB_Launch();
    HAL_FLASH_OB_Lock();
    
    /* Lock flash - just to be coherent */
    HAL_FLASH_Lock();
    
    /* Report ... */
    ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO, "Memory BANKs invalidated!");
    ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO, "");
    ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO, "Starting STM32 Bootloader ...");
    
    HAL_Delay(2000);
    
    /* Start bootloader */
    NVIC_SystemReset();
}

/* Terminal command definitions */
ketCube_terminal_cmd_t ketCube_terminal_commands_core[] = {
    {
        .cmd   = "basePeriod",
        .descr = "KETCube base period",
        .flags = {
            .isLocal   = TRUE,
            .isRemote  = TRUE,
            .isEEPROM  = TRUE,
            .isRAM     = TRUE,
            .isShowCmd = TRUE,
            .isSetCmd  = TRUE,
            .isGeneric = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_UINT32,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_UINT32,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_ID_CORE,
            .offset   = offsetof(ketCube_coreCfg_t, basePeriod),
            .size     = sizeof(uint32_t)
        }
    },
    
    {
        .cmd   = "startBootloader",
        .descr = "Initialize MCU to allow STM bootloader startup.",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isSetCmd  = TRUE,
        },
        .settingsPtr.callback = &ketCube_core_CMD_startBootloader,
    },
    
    {
        .cmd   = "factoryDefaults",
        .descr = "Erase EEPROM configuration.",
        .flags = {
            .isLocal   = TRUE,
            .isEEPROM  = TRUE,
            .isSetCmd  = TRUE,
        },
        .settingsPtr.callback = &ketCube_core_CMD_FactoryDefaults,
    },
    
    {
        .cmd   = "startDelay",
        .descr = "First periodic action is delayed after power-up and initialization",
        .flags = {
            .isLocal   = TRUE,
            .isRemote  = TRUE,
            .isEEPROM  = TRUE,
            .isRAM     = TRUE,
            .isShowCmd = TRUE,
            .isSetCmd  = TRUE,
            .isGeneric = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_UINT32,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_UINT32,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_ID_CORE,
            .offset   = offsetof(ketCube_coreCfg_t, startDelay),
            .size     = sizeof(uint32_t)
        }
    },
    
    {
        .cmd   = "severity",
        .descr = "Core messages severity: 0 = NONE, 1 = ERROR; 2 = INFO;"
                 " 3 = DEBUG",
        .flags = {
            .isLocal   = TRUE,
            .isRemote  = TRUE,
            .isEEPROM  = TRUE,
            .isRAM     = TRUE,
            .isShowCmd = TRUE,
            .isSetCmd  = TRUE,
            .isGeneric = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_BYTE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_ID_CORE,
            .offset   = offsetof(ketCube_coreCfg_t, severity),
            .size     = sizeof(ketCube_severity_t)
        }
    },
    
    {
        .cmd   = "remoteTerminalCounter",
        .descr = "If set to value > 0, no application data is sent through"
                 " radio, but rather just remote terminal commands and"
                 " responses",
        .flags = {
            .isLocal   = TRUE,
            .isRemote  = TRUE,
            .isRAM     = TRUE,
            .isEEPROM  = TRUE,
            .isShowCmd = TRUE,
            .isSetCmd  = TRUE,
            .isGeneric = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_UINT32,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_UINT32,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_ID_CORE,
            .offset   = offsetof(ketCube_coreCfg_t, remoteTerminalCounter),
            .size     = sizeof(uint16_t)
        }
    },
    
    DEF_TERMINATE()
    
};

/* Terminal command definitions for driver subgroup */
ketCube_terminal_cmd_t ketCube_terminal_commands_driver[] = {
    {
        .cmd   = "severity",
        .descr = "Driver(s) messages severity: 0 = NONE, 1 = ERROR; 2 = INFO;"
                 " 3 = DEBUG",
        .flags = {
            .isLocal   = TRUE,
            .isRemote  = TRUE,
            .isEEPROM  = TRUE,
            .isRAM     = TRUE,
            .isShowCmd = TRUE,
            .isSetCmd  = TRUE,
            .isGeneric = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_BYTE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_BYTE,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_ID_CORE,
            .offset   = offsetof(ketCube_coreCfg_t, driverSeverity),
            .size     = sizeof(ketCube_severity_t)
        }
    },
    
    DEF_TERMINATE()
    
};


/**
* @}
*/

#endif                          /* KETCUBE_CORE_CMD_H */
