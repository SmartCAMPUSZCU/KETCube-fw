/**
 *
 * @file    ketCube_hdcX080_cmd.c
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2020-05-28
 * @brief   The command definitions for HDC X080 (1080 and 2080) RH+T sensors
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2018 - 2020 University of West Bohemia in Pilsen
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

#ifndef __KETCUBE_HDCX080_CMD_H
#define __KETCUBE_HDCX080_CMD_H

#include "ketCube_cfg.h"
#include "ketCube_common.h"
#include "ketCube_terminal.h"
#include "ketCube_hdcX080.h"


/**
 * @brief Terminal command definitions 
 */
ketCube_terminal_cmd_t ketCube_hdcX080_commands[] = {
    {
        .cmd   = "type",
        .descr = "Select sensor type (0: auto-detect; 1: HDC1080; 2: HDC2080)",
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
            .moduleID = KETCUBE_LISTS_MODULEID_HDCX080,
            .offset   = offsetof(ketCube_hdcX080_moduleCfg_t, sensType),
            .size     = sizeof(ketCube_hdcX080_sensType_t)
        }
    },
    
    DEF_TERMINATE()
    
};

#endif                          /* __KETCUBE_HDCX080_CMD_H */
