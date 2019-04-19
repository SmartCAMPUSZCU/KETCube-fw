/**
 *
 * @file    ketCube_s0_cmd.c
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-08-19
 * @brief   The command definitions for batMeas
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

#ifndef __KETCUBE_BATMEAS_CMD_H
#define __KETCUBE_BATMEAS_CMD_H

#include "ketCube_cfg.h"
#include "ketCube_common.h"
#include "ketCube_terminal.h"
#include "ketCube_batMeas.h"

void ketCube_terminal_cmd_show_batMeas_list(void)
{
    uint8_t i;

    KETCUBE_TERMINAL_PRINTF("Available batteries:");
    KETCUBE_TERMINAL_ENDL();

    /* TODO: reimplement this to comply with new interface; this will probably
        need some kind of list return type, or at least list iterator (specify
        start, length, size, step size) so it could be easily serialized into
        UART terminal or e.g. LoRa packet. For a list iterator, we would need
        to specify start pointer (here: ketCube_batMeas_batList), end pointer
        (here: ketCube_batMeas_batList + KETCUBE_BATMEAS_BATLIST_LAST),
        type (here: string) and step (here: sizeof(ketCube_batMeas_battery_t))
    */
    
    for (i = 0; i < KETCUBE_BATMEAS_BATLIST_LAST; i++) {
        KETCUBE_TERMINAL_PRINTF("%d)\t %s (%s)", i,
                                ketCube_batMeas_batList[i].batName,
                                ketCube_batMeas_batList[i].batDescr);
        KETCUBE_TERMINAL_ENDL();
    }
}

/* Terminal command definitions */
    

ketCube_terminal_cmd_t ketCube_batMeas_commands[] = {
    {
        .cmd   = "list",
        .descr = "Show supported batteries",
        .flags = {
            .isLocal   = TRUE,
            .isShowCmd = TRUE,
            .isRAM     = TRUE,
        },
        .paramSetType  = KETCUBE_TERMINAL_PARAMS_NONE,
        .outputSetType = KETCUBE_TERMINAL_PARAMS_NONE,
        .settingsPtr.callback = &ketCube_terminal_cmd_show_batMeas_list,
    },
    
    {
        .cmd   = "bat",
        .descr = "Select battery (battery #)",
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
            .moduleID = KETCUBE_LISTS_MODULEID_BATMEAS,
            .offset   = offsetof(ketCube_batMeas_moduleCfg_t, selectedBattery),
            .size     = sizeof(ketCube_batMeas_battList_t)
        }
    },
    
    DEF_TERMINATE()
    
};

#endif                          /* __KETCUBE_BATMEAS_CMD_H */
