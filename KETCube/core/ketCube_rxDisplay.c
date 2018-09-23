/**
 * @file    ketCube_rxDisplay.c
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-04-17
 * @brief   This file contains the KETCube RxDisplay module
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


/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "ketCube_rxDisplay.h"
#include "ketCube_common.h"
#include "ketCube_terminal.h"


#ifdef KETCUBE_CFG_INC_MOD_RXDISPLAY

/**
  * @brief Initialize rxDisplay module
	*
	* @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_rxDisplay_Init(ketCube_InterModMsg_t ***
                                              msg)
{
    return KETCUBE_CFG_MODULE_OK;
}

/**
  * @brief Process data -- display on serial line
	*
	* @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_rxDisplay_ProcessData(ketCube_InterModMsg_t
                                                     * msg)
{
    uint8_t i;

    switch (msg->msg[0]) {
    case KETCUBE_RXDISPLAY_DATATYPE_RSSI:
        ketCube_terminal_Println("rxDisplay :: RSSI=%d; ", msg->msg[1]);
        break;
    case KETCUBE_RXDISPLAY_DATATYPE_SNR:
        ketCube_terminal_Println("rxDisplay :: SNR=%d; ", msg->msg[1]);
        break;
    case KETCUBE_RXDISPLAY_DATATYPE_STRING:
        ketCube_terminal_Print("rxDisplay :: STR=");
        for (i = 1; i < msg->msgLen; i++) {
            if (msg->msg[i] == (char) 0) {
                break;
            }
            ketCube_terminal_Print("%c", msg->msg[i]);
        }
        ketCube_terminal_Println("");
        break;
    default:
    case KETCUBE_RXDISPLAY_DATATYPE_DATA:
        ketCube_terminal_Print("rxDisplay :: DATA=");
        for (i = 1; i < msg->msgLen; i++) {
            ketCube_terminal_Print("%02X-", msg->msg[i]);
        }
        ketCube_terminal_Println("\b; ");
        break;
    }

    // confirm msg reception 
    msg->msgLen = 0;

    return KETCUBE_CFG_MODULE_OK;
}


#endif                          /* KETCUBE_CFG_INC_MOD_RXDISPLAY */
