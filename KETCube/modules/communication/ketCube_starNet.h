/**
 * @file    ketCube_starNet.h
 * @author  Jan Belohoubek
 * @version alpha
 * @date    2018-03-02
 * @brief   This file contains definitions for the KETCube starNet module
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

#define KETCUBE_STARNET_RF_FREQUENCY                                868000000   //< Hz for US, define: 915000000 // Hz
#define KETCUBE_STARNET_TX_OUTPUT_POWER                             14  //< dBm
#define KETCUBE_STARNET_RX_DATA_BUFFER_LEN                          64  //< Rx Data buffer length

/**
* @brief  Error code type.
*/
typedef enum {
    KETCUBE_STARNET_OK = (uint8_t) 0,
    KETCUBE_STARNET_ERROR = !KETCUBE_STARNET_OK
} ketCube_starNet_Error_t;

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
extern ketCube_cfg_ModError_t ketCube_starNet_receiveData(void);



/**
* @}
*/



/**
* @}
*/

#endif                          /* __KETCUBE_STARNET_H */
