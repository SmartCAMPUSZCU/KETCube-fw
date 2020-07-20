/**
 * @file    ketCube_coreCfg.c
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-04-27
 * @brief   This file contains the KETCube core configuration
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

#include "ketCube_coreCfg.h"
#include "ketCube_terminal.h"

/**
 * KETCube core configuration
 */
ketCube_coreCfg_t ketCube_coreCfg  __attribute__((section (".KETCubeCoreCfg_NoInit")));

/**
  * @brief Initialize rxDisplay module
	*
  * @retval KETCUBE_CFG_OK in case of success
  * @retval ketCube_cfg_Load_ERROR in case of failure
  */
ketCube_cfg_Error_t ketCube_coreCfg_Init(void)
{
    if (ketCube_coreCfg.basePeriod < KETCUBE_CORECFG_MIN_BASEPERIOD) {
        ketCube_coreCfg.basePeriod = KETCUBE_CORECFG_MIN_BASEPERIOD;
    }

    if (ketCube_coreCfg.startDelay < KETCUBE_CORECFG_MIN_STARTDELAY) {
        ketCube_coreCfg.startDelay = KETCUBE_CORECFG_MIN_STARTDELAY;
    }
    
    if (ketCube_coreCfg.repeatDelay == 0) {
        // repeat is disabled ...
    } else if (ketCube_coreCfg.repeatDelay < KETCUBE_CORECFG_MIN_REPEATDELAY) {
        ketCube_coreCfg.repeatDelay = KETCUBE_CORECFG_MIN_REPEATDELAY;
    }

    if (ketCube_coreCfg.severity > KETCUBE_CFG_SEVERITY_DEBUG) {
        ketCube_coreCfg.severity = KETCUBE_CORECFG_DEFAULT_SEVERITY;
    }

    ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO,
                                         "KETCube core base period set to: %d ms",
                                         ketCube_coreCfg.basePeriod);
    ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO,
                                         "KETCube start delay set to: %d ms",
                                         ketCube_coreCfg.startDelay);
    if (ketCube_coreCfg.repeatDelay != 0) {
        ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO,
                                             "KETCube repeat delay set to: %d ms",
                                             ketCube_coreCfg.repeatDelay);
    }
    
    ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO,
                                             "KETCube core severity level: %s", ketCube_severity_strAlias[ketCube_coreCfg.severity]);
    

    ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO,
                                             "KETCube driver severity level: %s", ketCube_severity_strAlias[ketCube_coreCfg.driverSeverity]);

#if (KETCUBE_CORECFG_SKIP_SLEEP_PERIOD == TRUE)
    ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO,
                                         "KETCube sleep period disabled!");
#endif

    return KETCUBE_CFG_OK;
}
