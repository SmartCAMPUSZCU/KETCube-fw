/**
 * @file    ketCube_batMeas.h
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-08-09
 * @brief   This file contains definitions for the KETCube batMeas module
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
#ifndef __KETCUBE_BATMEAS_H
#define __KETCUBE_BATMEAS_H

#include "ketCube_cfg.h"

/** @defgroup KETCube_batMeas KETCube batMeas
  * @brief KETCube batMeas cnt module
  * @ingroup KETCube_SensMods
  * @{
  */

/**
 * @brief enumerator of M-BUS configuration options addresses
 */
typedef enum {
    KETCUBE_BATMEAS_CFGADR_CFG = 0,
    KETCUBE_BATMEAS_CFGADR_BAT = 1
} ketCube_batMeas_cfgAdr_t;

/**
 * @brief enumerator of M-BUS configuration options lengths
 */
typedef enum {
    KETCUBE_BATMEAS_CFGLEN_CFG = 1,
    KETCUBE_BATMEAS_CFGLEN_BAT = 1
} ketCube_batMeas_cfgLen_t;

/**
* @brief  List of available S0 errors
*/
typedef enum {
    KETCUBE_BATMEAS_BATLIST_CR2032 = 0, /*< CR2032 battery */
    KETCUBE_BATMEAS_BATLIST_LS33600,    /*< LS33600 battery */

    KETCUBE_BATMEAS_BATLIST_LAST        /*< Last battery index -- do not modify this line! */
} ketCube_batMeas_battList_t;

/**
* @brief  KETCube battery deffinition
*/
typedef struct ketCube_batMeas_battery_t {
    char *batName;              /*< Battery name */
    char *batDescr;             /*< Battery description */
    uint16_t batCharged;        /*< Battery fully charged [mV] */
    uint16_t batDischarged;     /*< Battery discharged [mV] */
} ketCube_batMeas_battery_t;

/**
* @brief  KETCube batMeas module configuration
*/
typedef struct ketCube_batMeas_moduleCfg_t {
    ketCube_cfg_ModuleCfgByte_t coreCfg;           /*<! KETCube core cfg byte */
    
    ketCube_batMeas_battList_t selectedBattery;
} ketCube_batMeas_moduleCfg_t;

extern ketCube_batMeas_moduleCfg_t ketCube_batMeas_moduleCfg;

extern ketCube_batMeas_battery_t ketCube_batMeas_batList[];
extern uint8_t ketCube_batMeas_selectedBattery;


/** @defgroup KETCube_batMeas_fn Public Functions
* @{
*/

extern ketCube_cfg_ModError_t ketCube_batMeas_Init(ketCube_InterModMsg_t
                                                   *** msg);
extern ketCube_cfg_ModError_t ketCube_batMeas_ReadData(uint8_t * buffer,
                                                       uint8_t * len);


/**
* @}
*/


/**
* @}
*/

#endif                          /* __KETCUBE_BATMEAS_H */
