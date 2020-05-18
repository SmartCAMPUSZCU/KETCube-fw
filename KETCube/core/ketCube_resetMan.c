/**
 * @file    ketCube_resetMan.c
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2020-05-15
 * @brief   This file contains the KETCube reset management implementaion
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

#include "ketCube_resetMan.h"
#include "ketCube_terminal.h"
#include "ketCube_coreCfg.h"

/**
 * @brief Request software reset
 * 
 * @param reason reset reason
 *
 */
void ketCube_resetMan_requestReset(ketCube_resetMan_reason_t reason) {
    // save reason
    ketCube_coreCfg.volatileData.resetInfo.reason = reason;
    
    // perform reset
    NVIC_SystemReset();
}


/**
 * @brief Provide reset reasoning to user if necessary
 *
 */
void ketCube_resetMan_info(void) {
    
    switch(ketCube_coreCfg.volatileData.resetInfo.reason) {
        case KETCUBE_RESETMAN_REASON_USER_RQ:
        case KETCUBE_RESETMAN_REASON_USER_REMOTE_TERM:
            // this is valid reset reason - reset was invoked by SW - no error
//             KETCUBE_TERMINAL_CLR_LINE();
//             KETCUBE_TERMINAL_PRINTF("!!! The reset reason is USER REQUEST !!!");
//             KETCUBE_TERMINAL_ENDL();
//             KETCUBE_TERMINAL_ENDL();
            break;
        case KETCUBE_RESETMAN_REASON_POR:
            // this is common reset reason
//             KETCUBE_TERMINAL_CLR_LINE();
//             KETCUBE_TERMINAL_PRINTF("!!! The reset reason is POR !!!");
//             KETCUBE_TERMINAL_ENDL();
//             KETCUBE_TERMINAL_ENDL();
            break;
        case KETCUBE_RESETMAN_REASON_PORSW:
            // this is errorneous after-POR-workaround
            // this should be ignored - this is normal situation
//             KETCUBE_TERMINAL_CLR_LINE();
//             KETCUBE_TERMINAL_PRINTF("!!! The reset reason is PORSW !!!");
//             KETCUBE_TERMINAL_ENDL();
//             KETCUBE_TERMINAL_ENDL();
            break;
        case KETCUBE_RESETMAN_REASON_HARDFAULT:
            // hard faul - this requires user attention!
            // this may arise during SWD connector (dis)connecting - probably as a result of bounces @reset PIN
            KETCUBE_TERMINAL_CLR_LINE();
            KETCUBE_TERMINAL_PRINTF("!!! HardFault occured prior the RESET Event !!!");
            KETCUBE_TERMINAL_ENDL();
            KETCUBE_TERMINAL_ENDL();  
            ketCube_MCU_DumpHardFaultRegs(&(ketCube_coreCfg.volatileData.resetInfo.info.dbg.hardFault));
            KETCUBE_TERMINAL_ENDL();
            break;
        case KETCUBE_RESETMAN_REASON_UNKNOWN:
        default:
            // something goes wrong! This should never happen!
            KETCUBE_TERMINAL_CLR_LINE();
            KETCUBE_TERMINAL_PRINTF("The RESET was invoked by HW. This may be caused by:");
            KETCUBE_TERMINAL_ENDL();
            KETCUBE_TERMINAL_PRINTF("  - the watchdog");
            KETCUBE_TERMINAL_ENDL();
            KETCUBE_TERMINAL_PRINTF("  - the reset button");
            KETCUBE_TERMINAL_ENDL();
            KETCUBE_TERMINAL_PRINTF("  - unknown error");
            KETCUBE_TERMINAL_ENDL();
            KETCUBE_TERMINAL_ENDL();
            break;
    }
}
