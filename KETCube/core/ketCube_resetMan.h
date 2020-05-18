/**
 * @file    ketCube_resetMan.h
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2020-05-15
 * @brief   This file contains the KETCube core reset management configuration defs
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
#ifndef __KETCUBE_RESETMAN_H
#define __KETCUBE_RESETMAN_H

#include "ketCube_cfg.h"
#include "ketCube_mcu.h"

/** @defgroup KETCube_resetMan KETCube resetMan
  * @brief KETCube Core reset manager
  * @ingroup KETCube_Core
  * @{
  */


/**
* @brief  KETCube reset reason list
*/
typedef enum ketCube_resetMan_reason_t {
    KETCUBE_RESETMAN_REASON_UNKNOWN,             ///< Unknown RESET reason
    
    KETCUBE_RESETMAN_REASON_USER_RQ,             ///< RESET requested by USER (e.g. reload command)
    KETCUBE_RESETMAN_REASON_USER_REMOTE_TERM,    ///< RESET requested by USER (e.g. reload command)
    
    KETCUBE_RESETMAN_REASON_POR,                 ///< POR
    KETCUBE_RESETMAN_REASON_PORSW,               ///< automatic RESET rerquested by core after POR
    KETCUBE_RESETMAN_REASON_HARDFAULT            ///< automatic RESET after HardFault with debugInfo included
} ketCube_resetMan_reason_t;

/**
* @brief  KETCube reset-dbg information
* 
* @note The Segger's appnote https://www.segger.com/downloads/application-notes/AN00016 has been used to define dbg struct for HardFault reasoning
* 
*/
typedef struct ketCube_resetMan_dbg_t {
    union {
        ketCube_MCU_HardFaultRegs_t hardFault; ///< HardFault debug info
    } dbg;
} ketCube_resetMan_dbg_t;

/**
* @brief  KETCube RESET manager info struct
* 
* This struct survives soft-reset in RAM and thus provide useful information in case of failure without the need of external debugger usage
* 
*/
typedef struct ketCube_resetMan_t {
    ketCube_resetMan_reason_t reason;   ///< KETCube (last) RESET reason
    
    ketCube_resetMan_dbg_t info;        ///< Debug info related to errorneous reset
} ketCube_resetMan_t;



extern void ketCube_resetMan_requestReset(ketCube_resetMan_reason_t reason);
extern void ketCube_resetMan_info(void);


/**
* @}
*/

#endif                          /* __KETCUBE_RESETMAN_H */
