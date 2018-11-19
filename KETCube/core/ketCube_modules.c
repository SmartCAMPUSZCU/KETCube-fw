/**
 * @file    ketCube_modules.c
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-01-05
 * @brief   This file contains the KETCube module management
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
#include "ketCube_eeprom.h"
#include "ketCube_common.h"
#include "ketCube_modules.h"
#include "ketCube_terminal.h"

#include "hw_msp.h"
#include "ketCube_lora.h"
#include "ketCube_i2c.h"
#include "ketCube_hdc1080.h"
#include "ketCube_batMeas.h"
#include "ketCube_fdc2214.h"
#include "ketCube_adc.h"
#include "ketCube_starNet.h"
#include "ketCube_rxDisplay.h"
#include "ketCube_asyncTx.h"

// List of KETCube modules
#include "../../Projects/src/ketCube_moduleList.c" // include a project-specific file

uint8_t SensorBuffer[KETCUBE_MODULES_SENSOR_BYTES];     //<�sensor data are stored here
uint8_t SensorBufferSize = 0;   //<�sensor data buffer size

ketCube_InterModMsg_t **InterModMsgBuffer[ketCube_modules_CNT]; //<�Intra module message pointers; mesasages are stored and managed local-to modules

/**
 * @brief Load basic module configuration data from EEPROM and execute periodic functions for enabled modules
 * @retval KETCUBE_CFG_OK in case of success
 * @retval ketCube_cfg_Load_ERROR in case of failure
 */
ketCube_cfg_Error_t ketCube_modules_Init(void)
{
    uint8_t i;
    ketCube_cfg_ModuleCfgByte_t data;
    uint16_t addr = KETCUBE_EEPROM_ALLOC_MODULES;

    for (i = 0; i < ketCube_modules_CNT; i++) {
        ketCube_modules_List[i].cfgBase = addr;
        if (ketCube_cfg_Load
            ((uint8_t *) & (data), (ketCube_cfg_moduleIDs_t) i,
             (ketCube_cfg_AllocEEPROM_t) 0,
             (ketCube_cfg_LenEEPROM_t) 1) == KETCUBE_CFG_OK) {
            ketCube_modules_List[i].cfgByte = data;
        } else {
            return ketCube_cfg_Load_ERROR;
        }
        addr += ketCube_modules_List[i].cfgLen;
        // Init msg array
        InterModMsgBuffer[i] = (ketCube_InterModMsg_t **) NULL;
    }

    // Run module init functions
    for (i = 0; i < ketCube_modules_CNT; i++) {
        if ((ketCube_modules_List[i].cfgByte.enable & 0x01) == TRUE) {
            if (ketCube_modules_List[i].fnInit != NULL) {
                KETCUBE_TERMINAL_ENDL();
                KETCUBE_TERMINAL_PRINTF
                    ("--- Module \"%s\" Init() START ---",
                     ketCube_modules_List[i].name);
                KETCUBE_TERMINAL_ENDL();
                KETCUBE_TERMINAL_PRINTF("Module severity: ");
                switch (ketCube_modules_List[i].cfgByte.severity) {
                    case KETCUBE_CFG_SEVERITY_NONE:
                        KETCUBE_TERMINAL_PRINTF("NONE");
                        break;
                    case KETCUBE_CFG_SEVERITY_ERROR:
                        KETCUBE_TERMINAL_PRINTF("ERROR");
                        break;
                    case KETCUBE_CFG_SEVERITY_INFO:
                        KETCUBE_TERMINAL_PRINTF("INFO");
                        break;
                    case KETCUBE_CFG_SEVERITY_DEBUG:
                        KETCUBE_TERMINAL_PRINTF("DEBUG");
                        break;
                }
                KETCUBE_TERMINAL_ENDL();
                (ketCube_modules_List[i].fnInit) (&(InterModMsgBuffer[i]));
                KETCUBE_TERMINAL_PRINTF("--- Module \"%s\" Init() END ---",
                                        ketCube_modules_List[i].name);
                KETCUBE_TERMINAL_ENDL();
            }
        }
    }

    ketCube_terminal_UpdateCmdLine();
    return KETCUBE_CFG_OK;
}


/**
 * @brief Execute periodic functions for enabled modules
 * @retval KETCUBE_CFG_OK in case of success
 * @retval KETCUBE_CFG_ERROR in case of failure
 */
ketCube_cfg_Error_t ketCube_modules_ExecutePeriodic(void)
{
    uint8_t len;
    uint8_t i;

    SensorBufferSize = 0;

    // Run module getData functions periodicaly
    for (i = 0; i < ketCube_modules_CNT; i++) {
        if ((ketCube_modules_List[i].cfgByte.enable & 0x01) == TRUE) {
            if (ketCube_modules_List[i].fnGetSensorData != NULL) {
                ketCube_terminal_DebugPrintln
                    ("Module \"%s\" GetSensorData()",
                     ketCube_modules_List[i].name);

                len = 0;
                (ketCube_modules_List[i].fnGetSensorData) (&
                                                           (SensorBuffer
                                                            [SensorBufferSize]),
                                                           &len);
                SensorBufferSize += len;
            }
        }
    }

    // Run module communication functions
    for (i = 0; i < ketCube_modules_CNT; i++) {
        if ((ketCube_modules_List[i].cfgByte.enable & 0x01) == TRUE) {
            if (ketCube_modules_List[i].fnSendData != NULL) {
                ketCube_terminal_DebugPrintln("Module \"%s\" SendData()",
                                              ketCube_modules_List
                                              [i].name);

                (ketCube_modules_List[i].fnSendData) (&(SensorBuffer[0]),
                                                      &SensorBufferSize);
            }
        }
    }

    for (i = 0; i < ketCube_modules_CNT; i++) {
        if ((ketCube_modules_List[i].cfgByte.enable & 0x01) == TRUE) {
            if (ketCube_modules_List[i].fnReceiveData != NULL) {
                ketCube_terminal_DebugPrintln
                    ("Module \"%s\" ReceiveData()",
                     ketCube_modules_List[i].name);

                (ketCube_modules_List[i].fnReceiveData) ();
            }
        }
    }

    return KETCUBE_CFG_OK;
}


/**
 * @brief Process Intra module messages
 *
 * @retval KETCUBE_CFG_OK in case of success
 * @retval KETCUBE_CFG_ERROR in case of failure
 */
ketCube_cfg_Error_t ketCube_modules_ProcessMsgs(void)
{
    uint8_t i;
    uint8_t msgID;

    for (i = 0; i < ketCube_modules_CNT; i++) {
        if (InterModMsgBuffer[i] != (ketCube_InterModMsg_t **) NULL) {
            // data are ready to process
            msgID = 0;
            if (ketCube_modules_List
                [(InterModMsgBuffer[i])[msgID]->modID].fnProcessMsg !=
                NULL) {
                while ((InterModMsgBuffer[i])[msgID] != NULL) {
                    // first byte is the target module ID (recipient)
                    if ((InterModMsgBuffer[i])[msgID]->msgLen > 0) {
                        ketCube_terminal_DebugPrintln
                            ("Module \"%s\" ProcessData()",
                             ketCube_modules_List[(InterModMsgBuffer[i])
                                                  [msgID]->modID].name);
                        (ketCube_modules_List
                         [(InterModMsgBuffer[i])[msgID]->
                          modID].fnProcessMsg) ((InterModMsgBuffer[i])
                                                [msgID]);
                    }
                    msgID++;
                }
            }
        }
    }

    return KETCUBE_CFG_OK;
}


/**
 * @brief Process modules sleepEnter functions
 *
 * @retval KETCUBE_CFG_OK in case of ready-to-sleep
 * @retval KETCUBE_CFG_ERROR in case of not-ready-to-sleep
 */
ketCube_cfg_Error_t ketCube_modules_SleepEnter(void)
{
    uint8_t i;
    bool enableSleep = TRUE;

    for (i = 0; i < ketCube_modules_CNT; i++) {
        if ((ketCube_modules_List[i].cfgByte.enable & 0x01) == TRUE) {
            if (ketCube_modules_List[i].fnSleepEnter != NULL) {
                ketCube_terminal_DebugPrintln("Module \"%s\" SleepEnter()",
                                              ketCube_modules_List
                                              [i].name);

                if (((ketCube_modules_List[i].fnSleepEnter) ()) ==
                    KETCUBE_CFG_MODULE_ERROR) {
                    enableSleep = FALSE;
                }
            }
        }
    }

    if (enableSleep == TRUE) {
        return KETCUBE_CFG_OK;
    } else {
        return KETCUBE_CFG_ERROR;
    }
}

/**
 * @brief Process modules sleepExit functions
 *
 * @retval KETCUBE_CFG_OK in case of success
 * @retval KETCUBE_CFG_ERROR in case of failure
 */
ketCube_cfg_Error_t ketCube_modules_SleepExit(void)
{
    uint8_t i;

    for (i = 0; i < ketCube_modules_CNT; i++) {
        if ((ketCube_modules_List[i].cfgByte.enable & 0x01) == TRUE) {
            if (ketCube_modules_List[i].fnSleepExit != NULL) {
                ketCube_terminal_DebugPrintln("Module \"%s\" SleepExit()",
                                              ketCube_modules_List
                                              [i].name);

                (ketCube_modules_List[i].fnSleepExit) ();
            }
        }
    }

    return KETCUBE_CFG_OK;
}
