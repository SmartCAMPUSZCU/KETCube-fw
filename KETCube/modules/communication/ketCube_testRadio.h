/**
 * @file    ketCube_testRadio.h
 * @author  Jan Belohoubek
 * @version 0.2-dev
 * @date    2020-06-18
 * @brief   This file contains the KETCube radio test module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KETCUBE_TEST_RADIO_H
#define __KETCUBE_TEST_RADIO_H

#include "ketCube_cfg.h"
#include "ketCube_common.h"
#include "ketCube_terminal.h"

/** @defgroup KETCube_testRadio KETCube testRadio
  * @brief KETCube testRadio module
  * @ingroup KETCube_CommMods
  * @{
  */

/**
* @brief  KETCube module configuration
*/
typedef struct ketCube_testRadio_moduleCfg_t {
    ketCube_cfg_ModuleCfgByte_t coreCfg;           /*!< KETCube core cfg byte */
    
    uint32_t cwFreq;                              /*!< Continuous wave frequency */
    uint8_t cwPwr;                                /*!< Continuous wave power */
    uint16_t cwDur;                               /*!< Continuous wave duration */
} ketCube_testRadio_moduleCfg_t;

extern ketCube_testRadio_moduleCfg_t ketCube_testRadio_moduleCfg;


#define KETCUBE_TEST_RADIO_MIN_OUTPUT_POWER                           7  /*!< dBm */
#define KETCUBE_TEST_RADIO_MAX_OUTPUT_POWER                          16  /*!< dBm */
#define KETCUBE_TEST_RADIO_DEFAULT_OUTPUT_POWER                      14  /*!< dBm */
#define KETCUBE_TEST_RADIO_MIN_FREQUENCY                      400000000  /*!< Hz */
#define KETCUBE_TEST_RADIO_MAX_FREQUENCY                     1000000000  /*!< Hz */
#define KETCUBE_TEST_RADIO_DEFAULT_FREQUENCY                  868000000  /*!< Hz */
#define KETCUBE_TEST_RADIO_DEFAULT_CW_DUR                           10  /*!< Defaul continuous wave time in seconds */

/**
* @brief  StarNet states
*/
typedef enum {
    KETCUBE_TEST_RADIO_STATE_SLEEP,         /*!< Sleep mode */
    KETCUBE_TEST_RADIO_STATE_CW,            /*!< Continuous wave */
    KETCUBE_TEST_RADIO_STATE_TX_TIMEOUT,    /*!< Tx Timeout */
    KETCUBE_TEST_RADIO_STATE_TX_DONE,       /*!< Tx Done */
    KETCUBE_TEST_RADIO_STATE_RX_TIMEOUT,    /*!< Rx Timeout */
    KETCUBE_TEST_RADIO_STATE_RX_DONE,       /*!< Rx Done */
    KETCUBE_TEST_RADIO_STATE_RX_ERROR,      /*!< Rx Error */
} ketCube_testRadio_State_t;


/** @defgroup KETCube_testRadio_fn Public Functions
* @{
*/

extern ketCube_cfg_ModError_t ketCube_testRadio_Init(ketCube_InterModMsg_t *** msg);
extern ketCube_cfg_ModError_t ketCube_testRadio_SleepEnter(void);

extern void ketCube_testRadio_cmd_GoSleep(void);
extern void ketCube_testRadio_cmd_GoCW(void);


/**
* @}
*/



/**
* @}
*/

#endif                          /* __KETCUBE_TEST_RADIO_H */
