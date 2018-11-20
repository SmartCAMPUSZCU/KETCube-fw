/**
 * @file    ketCube_asyncTx.c
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-08-16
 * @brief   This file contains the KETCube AsyncTx module
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
#include "ketCube_asyncTx.h"
#include "ketCube_lora.h"
#include "ketCube_common.h"
#include "ketCube_modules.h"
#include "ketCube_terminal.h"


#ifdef KETCUBE_CFG_INC_MOD_ASYNCTX

/**
  * @brief Initialize AsyncTx module
  *
  * @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_AsyncTx_Init(ketCube_InterModMsg_t *** msg)
{
    return KETCUBE_CFG_MODULE_OK;
}

/**
  * @brief Process data -- send using enabled interfaces
  *
  * @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_AsyncTx_ProcessData(ketCube_InterModMsg_t
                                                   * msg)
{
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_ASYNCTX, "Msg from module = %d",
                                  (int) msg->msg[0]);


#ifdef KETCUBE_CFG_INC_MOD_LORA
    if ((ketCube_modules_List[KETCUBE_LISTS_MODULEID_LORA].cfgByte.enable & 0x01) == TRUE) {
        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_ASYNCTX, "LoRa msg from module = %d", (int) msg->msg[0]);
        ketCube_lora_AsyncSend((uint8_t *) & (msg->msg[0]),
                               &(msg->msgLen));
    }
    // confirm msg reception 
    msg->msgLen = 0;
#endif // KETCUBE_CFG_INC_MOD_LORA

    return KETCUBE_CFG_MODULE_OK;
}


#endif                          /* KETCUBE_CFG_INC_MOD_ASYNCTX */
