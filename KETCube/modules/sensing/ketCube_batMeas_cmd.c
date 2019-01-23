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


void ketCube_terminal_cmd_set_batMeas_bat(void)
{
    ketCube_terminal_saveCfgDECStr(&(commandBuffer[commandParams]),
                                   KETCUBE_LISTS_MODULEID_BATMEAS,
                                   (ketCube_cfg_AllocEEPROM_t)
                                   KETCUBE_BATMEAS_CFGADR_BAT,
                                   (ketCube_cfg_LenEEPROM_t)
                                   KETCUBE_BATMEAS_CFGLEN_BAT);
}


void ketCube_terminal_cmd_show_batMeas_bat(void)
{
    ketCube_batMeas_battList_t selected;

    ketCube_cfg_Load((uint8_t *) & selected,
                     KETCUBE_LISTS_MODULEID_BATMEAS,
                     (ketCube_cfg_AllocEEPROM_t) KETCUBE_BATMEAS_CFGADR_BAT,
                     (ketCube_cfg_LenEEPROM_t) KETCUBE_BATMEAS_CFGLEN_BAT);

    if (selected >= KETCUBE_BATMEAS_BATLIST_LAST) {
        selected = KETCUBE_BATMEAS_BATLIST_CR2032;
    }

    KETCUBE_TERMINAL_PRINTF("Selected battery: %s (%s)",
                            ketCube_batMeas_batList[selected].batName,
                            ketCube_batMeas_batList[selected].batDescr);
    KETCUBE_TERMINAL_ENDL();
}

void ketCube_terminal_cmd_show_batMeas_list(void)
{
    uint8_t i;

    KETCUBE_TERMINAL_PRINTF("Available batteries:");
    KETCUBE_TERMINAL_ENDL();

    for (i = 0; i < KETCUBE_BATMEAS_BATLIST_LAST; i++) {
        KETCUBE_TERMINAL_PRINTF("%d)\t %s (%s)", i,
                                ketCube_batMeas_batList[i].batName,
                                ketCube_batMeas_batList[i].batDescr);
        KETCUBE_TERMINAL_ENDL();
    }
}

#endif                          /* __KETCUBE_BATMEAS_CMD_H */
