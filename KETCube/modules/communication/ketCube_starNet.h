/**
 * @file    ketCube_starNet.h
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2019-08-27
 * @brief   This file contains definitions for the KETCube starNet node and concentrator modules
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
#ifndef __KETCUBE_STARNET_H
#define __KETCUBE_STARNET_H

#include "ketCube_cfg.h"
#include "ketCube_common.h"
#include "ketCube_terminal.h"

/** @defgroup KETCube_StarNet KETCube StarNet
  * @brief KETCube StarNet module
  * @ingroup KETCube_CommMods
  * @{
  */

/**
* @brief  KETCube module configuration
*/
typedef struct ketCube_starNet_moduleCfg_t {
    ketCube_cfg_ModuleCfgByte_t coreCfg;           /*!< KETCube core cfg byte */
} ketCube_starNet_moduleCfg_t;

extern ketCube_starNet_moduleCfg_t ketCube_starNetConcentrator_moduleCfg;
extern ketCube_starNet_moduleCfg_t ketCube_starNetNode_moduleCfg;


#if defined( REGION_AS923 )

#define KETCUBE_STARNET_RF_FREQUENCY                923000000 // Hz

#elif defined( REGION_AU915 )

#define KETCUBE_STARNET_RF_FREQUENCY                915000000 // Hz

#elif defined( REGION_CN470 )

#define KETCUBE_STARNET_RF_FREQUENCY                470000000 // Hz

#elif defined( REGION_CN779 )

#define KETCUBE_STARNET_RF_FREQUENCY                779000000 // Hz

#elif defined( REGION_EU433 )

#define KETCUBE_STARNET_RF_FREQUENCY                433000000 // Hz

#elif defined( REGION_EU868 )

#define KETCUBE_STARNET_RF_FREQUENCY                868000000 // Hz

#elif defined( REGION_KR920 )

#define KETCUBE_STARNET_RF_FREQUENCY                920000000 // Hz

#elif defined( REGION_IN865 )

#define KETCUBE_STARNET_RF_FREQUENCY                865000000 // Hz

#elif defined( REGION_US915 )

#define KETCUBE_STARNET_RF_FREQUENCY                915000000 // Hz

#elif defined( REGION_RU864 )

#define KETCUBE_STARNET_RF_FREQUENCY                864000000 // Hz

#else
    #error "Define the frequency band in the compiler options."
#endif


#define KETCUBE_STARNET_TX_OUTPUT_POWER                             14  /*!< dBm */
#define KETCUBE_STARNET_DATA_BUFFER_LEN                             64  /*!< Tx/Rx Data buffer length */

/**
* @brief  Error code type.
*/
typedef enum {
    KETCUBE_STARNET_OK = (uint8_t) 0,
    KETCUBE_STARNET_ERROR = !KETCUBE_STARNET_OK
} ketCube_starNet_Error_t;

/**
* @brief  StarNet states
*/
typedef enum {
    KETCUBE_STARNET_STATE_TX_READY,
    KETCUBE_STARNET_STATE_TX_DONE,
    KETCUBE_STARNET_STATE_TX_NEW_DATA,
    KETCUBE_STARNET_STATE_TX_PROGRESS,
    KETCUBE_STARNET_STATE_TX_TIMEOUT,
    KETCUBE_STARNET_STATE_TX_ERROR,
    
    KETCUBE_STARNET_STATE_RX_READY,
    KETCUBE_STARNET_STATE_RX_DONE,
    KETCUBE_STARNET_STATE_RX_PROGRESS,
    KETCUBE_STARNET_STATE_RX_TIMEOUT,
    KETCUBE_STARNET_STATE_RX_ERROR,
} ketCube_starNet_State_t;

/**
* @brief  Node type
*/
typedef enum {
    KETCUBE_STARNET_CONCENTRATOR,
    KETCUBE_STARNET_NODE
} ketCube_starNet_NodeType_t;

extern ketCube_starNet_NodeType_t ketCube_starNet_nodeType;

/** @defgroup KETCube_StarNet_fn Public Functions
* @{
*/

extern ketCube_cfg_ModError_t
ketCube_starNet_NodeInit(ketCube_InterModMsg_t *** msg);
extern ketCube_cfg_ModError_t
ketCube_starNet_ConcentratorInit(ketCube_InterModMsg_t *** msg);

extern ketCube_cfg_ModError_t ketCube_starNet_SleepEnter(void);
extern ketCube_cfg_ModError_t ketCube_starNet_sendData(uint8_t * buffer,
                                                       uint8_t * len);



/**
* @}
*/



/**
* @}
*/

#endif                          /* __KETCUBE_STARNET_H */
