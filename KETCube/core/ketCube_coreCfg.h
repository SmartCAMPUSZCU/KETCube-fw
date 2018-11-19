/**
 * @file    ketCube_coreCfg.h
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-04-27
 * @brief   This file contains the KETCube core configuration defs
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KETCUBE_CORECFG_H
#define __KETCUBE_CORECFG_H

#include "ketCube_cfg.h"

/** @defgroup KETCube_coreCfg KETCube CfgCore
  * @brief KETCube Core configuration
  * @ingroup KETCube_Core
  * @{
  */

#define KETCUBE_CORECFG_SKIP_SLEEP_PERIOD          FALSE        //< Skip sleep period (perform sensing at maximum speed)
#define KETCUBE_CORECFG_MIN_BASEPERIOD             5000 //< Minimal period for periodic events
#define KETCUBE_CORECFG_MIN_STARTDELAY             500  //< Minimal delay - the first periodic action is run at this time after power-on
#define KETCUBE_CORECFG_DEFAULT_SEVERITY           KETCUBE_CFG_SEVERITY_ERROR  //< Default KETCube core severity

/**
* @brief  Core CFG data relative addr.
*/
typedef enum {
    KETCUBE_CORECFG_ADR_BASEPERIOD = 0, /*<! Base period configuration address (in ms) */
    KETCUBE_CORECFG_ADR_STARTDELAY = 4, /*<! Start delay configuration address (in ms) */
    KETCUBE_CORECFG_ADR_SEVERITY   = 8, /*<! Core severity, see @ketCube_severity_t */
} ketCube_coreCfg_Addr_t;

extern uint32_t           ketCube_coreCfg_BasePeriod;     //< This period is used by KETCube core to run periodic events
extern uint32_t           ketCube_coreCfg_StartDelay;     //< This delay is used instead ketCube_coreCfg_BasePeriod to run periodic events at the first time
extern ketCube_severity_t ketCube_coreCfg_severity;       //< Core messages severity

/** @defgroup KETCube_coreCfg_fn Public Functions
* @{
*/

extern ketCube_cfg_Error_t ketCube_coreCfg_Init(void);

/**
* @}
*/



/**
* @}
*/

#endif                          /* __KETCUBE_CORECFG_H */
