/**
 * @file    ketCube_coreCfg.h
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2020-05-15
 * @brief   This file contains the KETCube core configuration defs
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KETCUBE_CORECFG_H
#define __KETCUBE_CORECFG_H

#include "ketCube_cfg.h"
#include "ketCube_resetMan.h"

/** @defgroup KETCube_coreCfg KETCube CfgCore
  * @brief KETCube Core configuration
  * @ingroup KETCube_Core
  * @{
  */

#define KETCUBE_CORECFG_SKIP_SLEEP_PERIOD          FALSE        ///< Skip sleep period (perform sensing at maximum speed)
#define KETCUBE_CORECFG_MIN_BASEPERIOD             500          ///< Minimal period for periodic events
#define KETCUBE_CORECFG_MIN_STARTDELAY             1000         ///< Minimal delay - the first periodic action is run at this time after power-on
#define KETCUBE_CORECFG_DEFAULT_SEVERITY           KETCUBE_CFG_SEVERITY_ERROR   ///< Default KETCube core severity

/**
* @brief  KETCube core configuration
* 
* @note 128B is reserved for CORE configuration in EEPROM - see the summary size of the fields and RFU
* 
*/
typedef struct ketCube_coreCfg_t {
    ketCube_cfg_ModuleCfgByte_t coreCfg; ///< KETCube core cfg byte
    
    uint32_t basePeriod;                 ///< This period is used by KETCube core to run periodic events
    uint32_t startDelay;                 ///< This delay is used instead ketCube_coreCfg_BasePeriod to run periodic events at the first time
    ketCube_severity_t severity;         ///< Core messages severity
    ketCube_severity_t driverSeverity;   ///< Driver(s) messages severity
    uint16_t remoteTerminalCounter;      ///< Is currently in remote terminal mode (value > 0)? If so, how many basePeriods to reload?
    
    union {
        ketCube_resetMan_t resetInfo;    ///< Reset Reasoning
        uint8_t RFU[115];                ///< This part of EEPROM is RFU, when adding new field into this struct, decrease the size of this field to preserve configuration padding for module(s) configuration; 128B is reserved for CORE in total
    } volatileData;                      ///< This union should aggregate volatile data, whose require no fixed location over KETCube releases
} ketCube_coreCfg_t;

extern ketCube_coreCfg_t ketCube_coreCfg;

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
