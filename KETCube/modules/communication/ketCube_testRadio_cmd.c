/**
 * @file    ketCube_testRadio_cmd.c
 * @author  Jan Belohoubek
 * @version 0.2-dev
 * @date    2020-06-19
 * @brief   This file contains the KETCube module commandline deffinition
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 University of West Bohemia in Pilsen
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


#ifndef __KETCUBE_TEST_RADIO_CMD_H
#define __KETCUBE_TEST_RADIO_CMD_H

#include "ketCube_cfg.h"
#include "ketCube_common.h"
#include "ketCube_terminal.h"
#include "ketCube_testRadio.h"


/* Terminal command definitions */
ketCube_terminal_cmd_t ketCube_testRadio_commands[] = {
    {
        .cmd   = "cwFreq",
        .descr = "CW frequency [Hz]",
        .flags = {
            .isLocal   = TRUE,
            .isRAM     = TRUE,
            .isEEPROM  = TRUE,
            .isShowCmd = TRUE,
            .isSetCmd  = TRUE,
            .isGeneric = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_UINT32,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_UINT32,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_TEST_RADIO,
            .offset   = offsetof(ketCube_testRadio_moduleCfg_t, cwFreq),
            .size     = sizeof(uint32_t),
        }
    },
    
    {
        .cmd   = "cwPwr",
        .descr = "CW power [dBm]",
        .flags = {
            .isLocal   = TRUE,
            .isRAM     = TRUE,
            .isEEPROM  = TRUE,
            .isShowCmd = TRUE,
            .isSetCmd  = TRUE,
            .isGeneric = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_UINT32,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_UINT32,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_TEST_RADIO,
            .offset   = offsetof(ketCube_testRadio_moduleCfg_t, cwPwr),
            .size     = sizeof(uint8_t),
        }
    },
    
    {
        .cmd   = "cwDur",
        .descr = "CW duration [seconds]",
        .flags = {
            .isLocal   = TRUE,
            .isRAM     = TRUE,
            .isEEPROM  = TRUE,
            .isShowCmd = TRUE,
            .isSetCmd  = TRUE,
            .isGeneric = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_UINT32,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_UINT32,
        .settingsPtr.cfgVarPtr = &(ketCube_cfg_varDescr_t) {
            .moduleID = KETCUBE_LISTS_MODULEID_TEST_RADIO,
            .offset   = offsetof(ketCube_testRadio_moduleCfg_t, cwDur),
            .size     = sizeof(uint16_t),
        }
    },
    
    {
        .cmd   = "getTemp",
        .descr = "Get radio temperature",
        .flags = {
            .isLocal   = TRUE,
            .isRemote  = TRUE,
            .isRAM     = TRUE,
            .isEEPROM  = FALSE,
            .isShowCmd = TRUE,
            .isSetCmd  = FALSE,
            .isGeneric = FALSE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_INT32,
        .settingsPtr.callback = &ketCube_testRadio_cmd_getTemp
    },
    
    {
        .cmd   = "goCW",
        .descr = "Enter radio CW mode",
        .flags = {
            .isLocal   = TRUE,
            .isRemote  = TRUE,
            .isRAM     = TRUE,
            .isEEPROM  = FALSE,
            .isShowCmd = FALSE,
            .isSetCmd  = TRUE,
            .isGeneric = FALSE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_NONE,
        .settingsPtr.callback = &ketCube_testRadio_cmd_GoCW
    },
    
    {
        .cmd   = "goSleep",
        .descr = "Enter radio sleep mode",
        .flags = {
            .isLocal   = TRUE,
            .isRemote  = TRUE,
            .isRAM     = TRUE,
            .isEEPROM  = FALSE,
            .isShowCmd = FALSE,
            .isSetCmd  = TRUE,
            .isGeneric = FALSE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_NONE,
        .settingsPtr.callback = &ketCube_testRadio_cmd_GoSleep
    },
    
    DEF_TERMINATE()
};


#endif                          /* __KETCUBE_TEST_RADIO_CMD_H */
