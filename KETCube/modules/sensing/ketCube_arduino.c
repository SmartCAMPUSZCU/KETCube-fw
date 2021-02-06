/**
 * @file    ketCube_arduino.c
 * @author  Tomáš Hák
 * @author  Jan Bělohoubek
 * @version 0.2
 * @date    2020-05-31
 * @brief   This file contains definitions for the Arduino driver
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

#include "ketCube_cfg.h"
#include "ketCube_terminal.h"
#include "ketCube_arduino.h"

#ifdef KETCUBE_CFG_INC_MOD_ARDUINO

ketCube_arduino_moduleCfg_t ketCube_arduino_moduleCfg; /*!< Module configuration storage */

/* API init definition */
extern void ketCube_Arduino_API_Init();
extern void ketCube_Arduino_API_getBuffer(uint8_t *buffer, uint8_t *len);
extern void ketCube_Arduino_API_SleepExitHandler();
extern void ketCube_Arduino_API_NewLPWANData(uint8_t * buffer, uint8_t len);

// extern setup will be over this weak implementation
__weak void setup(void) {
    // do nothing
    ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_ARDUINO, "This is _weak setup():");
}

/**
 * @brief Initialize the Arduino sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_arduino_Init(ketCube_InterModMsg_t *** msg)
{

    // Init API
    ketCube_Arduino_API_Init();
    
    // Init Drivers
    ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_ARDUINO, "Executing extern function setup():");
    setup();
    
    // Custom Init Code

    return KETCUBE_CFG_MODULE_OK;
}

// extern loop will be over this weak implementation
__weak void loop(void) {
    // do nothing
    ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_ARDUINO, "This is _weak loop():");
}

/**
  * @brief Read data from Arduino sensor
  *
  * @param buffer pointer to fuffer for storing the result of measurements
  * @param len data len in bytes
  *
  * @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_arduino_ReadData(uint8_t * buffer,
                                                uint8_t * len)
{
    
    ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_ARDUINO, "Executing extern function loop():");
    loop();
    
    /* fill buffer here */
    ketCube_Arduino_API_getBuffer(buffer, len);

    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief SleepExit Handler
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_arduino_sleepExit(void)
{
    ketCube_Arduino_API_SleepExitHandler();
    
    return KETCUBE_CFG_MODULE_OK;
}


/**
  * @brief Process data -- display on serial line
  *
  * @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_arduino_ProcessData(ketCube_InterModMsg_t * msg)
{
    /* msg->msgLen-2 to remove terminating zeroes and the first byte */
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_ARDUINO, "LPWAN Rx (%d)=%s", msg->msgLen-2,
    ketCube_common_bytes2Str(&(msg->msg[1]), msg->msgLen-2));

    // remove the terminating zero byte + the leading byte -> RX_DISPLAY Type
    ketCube_Arduino_API_NewLPWANData(&(msg->msg[1]), msg->msgLen-2);

    // confirm msg reception 
    msg->msgLen = 0;
    
    return KETCUBE_CFG_MODULE_OK;
}

#endif                          /* KETCUBE_CFG_INC_MOD_ARDUINO */
