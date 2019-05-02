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