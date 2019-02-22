/**
 * @file    ketCube_moduleList.c
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2018-10-31
 * @brief   This file contains the KETCube module list
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
#include "ketCube_coreCfg.h"
#include "ketCube_common.h"

#include "hw_msp.h"
#include "ketCube_lora.h"
#include "ketCube_i2c.h"
#include "ketCube_hdc1080.h"
#include "ketCube_batMeas.h"
#include "ketCube_adc.h"
#include "ketCube_starNet.h"
#include "ketCube_rxDisplay.h"
#include "ketCube_asyncTx.h"
#include "ketCube_txDisplay.h"

// pre-alpha and alpha modules
#include "ketCube_fdc2214.h"
#include "ketCube_ics43432.h"
#include "ketCube_bmeX80.h"
#include "ketCube_lis2hh12.h"


/**
 * Define a KETCube module 
 * 
 */
#define DEF_MODULE(name, descr, initFn, sleepEnter, sleepExit, \
                   getSensData, sendData, recvData, processData, cfgStruct) \
                  { \
                      ((char*) &(name)),\
                      ((char*) &(descr)), \
                      (ketCube_cfg_ModInitFn_t) (initFn), \
                      (ketCube_cfg_ModVoidFn_t) (sleepEnter), \
                      (ketCube_cfg_ModVoidFn_t) (sleepExit), \
                      (ketCube_cfg_ModDataFn_t) (getSensData), \
                      (ketCube_cfg_ModDataFn_t) (sendData), \
                      (ketCube_cfg_ModVoidFn_t) (recvData), \
                      (ketCube_cfg_ModDataPtrFn_t) (processData), \
                      (ketCube_cfg_ModuleCfgByte_t *) &(cfgStruct), \
                      sizeof(cfgStruct), \
                      0 \
                   }


/**
* @brief List of KETCube modules
*/
ketCube_cfg_Module_t ketCube_modules_List[ketCube_modules_CNT] = {
    DEF_MODULE("KETCube core",
               "Note: core is always enabled",
               &ketCube_coreCfg_Init,     /* Init() */
               NULL,                      /* SleepEnter() */
               NULL,                      /* SleepExit() */
               NULL,                      /* GetSensorData() */
               NULL,                      /* SendData() */
               NULL,                      /* ReceiveData() */
               NULL,                      /* ProcessData() */
               ketCube_coreCfg            /* KETCube core cfg struct */
              ),
              
#ifdef KETCUBE_CFG_INC_MOD_LORA
    DEF_MODULE("LoRa",
               "LoRa radio",
               &ketCube_lora_Init,        /* Init() */
               &ketCube_lora_SleepEnter,  /* SleepEnter() */
               NULL,                      /* SleepExit() */
               NULL,                      /* GetSensorData() */
               &ketCube_lora_Send,        /* SendData() */
               NULL,                      /* ReceiveData() */
               NULL,                      /* ProcessData() */
               ketCube_lora_moduleCfg     /* Module cfg struct */
              ),
#endif
#ifdef KETCUBE_CFG_INC_MOD_DEBUGDISPLAY
    {((char *) &("DebugDisplay")),
     ((char *) &("Display debug output on serial line")),
     (ketCube_cfg_ModInitFn_t) NULL,    /*·Module Init() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·SleepEnter() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·SleepExit() */
     (ketCube_cfg_ModDataFn_t) NULL,    /*·GetSensorData() */
     (ketCube_cfg_ModDataFn_t) NULL,    /*·SendData() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·ReceiveData() */
     (ketCube_cfg_ModDataPtrFn_t) NULL, /*·ProcessData() */
     0,                         /*·CFG base addr -- set dynamicaly */
     1,                         /*·CFG len in bytes */
     TRUE                       /*·module CFG byte -- set dynamically */
     },
#endif
#ifdef KETCUBE_CFG_INC_MOD_HDC1080
    DEF_MODULE("HDC1080",
               "On-board RHT sensor based on TI HDC1080",
               &ketCube_hdc1080_Init,     /* Init() */
               NULL,                      /* SleepEnter() */
               NULL,                      /* SleepExit() */
               &ketCube_hdc1080_ReadData, /* GetSensorData() */
               NULL,                      /* SendData() */
               NULL,                      /* ReceiveData() */
               NULL,                      /* ProcessData() */
               ketCube_hdc1080_moduleCfg  /* Module cfg struct */
              ),
#endif
#ifdef KETCUBE_CFG_INC_MOD_BATMEAS
    DEF_MODULE("batMeas",
               "On-chip battery voltage measurement",
               &ketCube_batMeas_Init,     /* Init() */
               NULL,                      /* SleepEnter() */
               NULL,                      /* SleepExit() */
               &ketCube_batMeas_ReadData, /* GetSensorData() */
               NULL,                      /* SendData() */
               NULL,                      /* ReceiveData() */
               NULL,                      /* ProcessData() */
               ketCube_batMeas_moduleCfg  /* Module cfg struct */
              ),
#endif
#ifdef KETCUBE_CFG_INC_MOD_ADC
    DEF_MODULE("ADC",
               "Measure mVolts on PA4",
               &ketCube_ADC_Init,         /* Init() */
               NULL,                      /* SleepEnter() */
               NULL,                      /* SleepExit() */
               &ketCube_ADC_ReadData,     /* GetSensorData() */
               NULL,                      /* SendData() */
               NULL,                      /* ReceiveData() */
               NULL,                      /* ProcessData() */
               ketCube_ADC_moduleCfg      /* Module cfg struct */
              ),
#endif
#ifdef KETCUBE_CFG_INC_MOD_STARNET
    DEF_MODULE("StarNetConcentrator",
               "Compose a star-type network with this module as the network concentrator",
               &ketCube_starNet_ConcentratorInit, /* Init() */
               &ketCube_starNet_SleepEnter,       /* SleepEnter() */
               NULL,                              /* SleepExit() */
               &ketCube_ADC_ReadData,             /* GetSensorData() */
               NULL,                              /* SendData() */
               ketCube_starNet_receiveData,       /* ReceiveData() */
               NULL,                              /* ProcessData() */
               ketCube_starNet_moduleCfg          /* Module cfg struct */
              ),
    DEF_MODULE("StarNetNode",
               "Compose a star-type network with this module as a sensor node",
               &ketCube_starNet_NodeInit, /* Init() */
               &ketCube_starNet_SleepEnter,       /* SleepEnter() */
               NULL,                              /* SleepExit() */
               NULL,                              /* GetSensorData() */
               ketCube_starNet_sendData,          /* SendData() */
               NULL,                              /* ReceiveData() */
               NULL,                              /* ProcessData() */
               ketCube_starNet_moduleCfg          /* Module cfg struct */
              ),
#endif
#ifdef KETCUBE_CFG_INC_MOD_FDC2214
    DEF_MODULE("FDC2214",
               "TI\'s FDC2214 capacity to digital converter",
               &ketCube_fdc2214_Init,       /* Init() */
               &ketCube_fdc2214_SleepEnter, /*·SleepEnter() */
               &ketCube_fdc2214_SleepExit,  /*·SleepExit() */
               &ketCube_fdc2214_ReadData,   /* GetSensorData() */
               NULL,                        /* SendData() */
               NULL,                        /* ReceiveData() */
               NULL,                        /* ProcessData() */
               ketCube_fdc2214_moduleCfg    /* Module cfg struct */
              ),
#endif
#ifdef KETCUBE_CFG_INC_MOD_RXDISPLAY
    DEF_MODULE("RxDisplay",
               "Display received data on serial line",
               &ketCube_rxDisplay_Init,         /* Init() */
               NULL,                            /* SleepEnter() */
               NULL,                            /* SleepExit() */
               NULL,                            /* GetSensorData() */
               NULL,                            /* SendData() */
               NULL,                            /* ReceiveData() */
               &ketCube_rxDisplay_ProcessData,  /* ProcessData() */
               ketCube_rxDisplay_moduleCfg      /* Module cfg struct */
              ),
#endif
#ifdef KETCUBE_CFG_INC_MOD_ASYNCTX
    DEF_MODULE("AsyncTx",
               "Asynchronously transmit data",
               &ketCube_asyncTx_Init,         /* Init() */
               NULL,                            /* SleepEnter() */
               NULL,                            /* SleepExit() */
               NULL,                            /* GetSensorData() */
               NULL,                            /* SendData() */
               NULL,                            /* ReceiveData() */
               ketCube_asyncTx_ProcessData,     /* ProcessData() */
               ketCube_asyncTx_moduleCfg        /* Module cfg struct */
              ),
#endif
#ifdef KETCUBE_CFG_INC_MOD_TXDISPLAY
    DEF_MODULE("TxDisplay",
               "Display data ready for transmission on serial line",
               &ketCube_txDisplay_Init,         /* Init() */
               NULL,                            /* SleepEnter() */
               NULL,                            /* SleepExit() */
               NULL,                            /* GetSensorData() */
               ketCube_txDisplay_Send,          /* SendData() */
               NULL,                            /* ReceiveData() */
               NULL,                            /* ProcessData() */
               ketCube_txDisplay_moduleCfg      /* Module cfg struct */
              ),
#endif
#ifdef KETCUBE_CFG_INC_MOD_BMEX80
    DEF_MODULE("BMEx80",
               "On-board environmental sensor based on Bosch BME family",
               &ketCube_bmeX80_Init,        /* Init() */
               NULL,                        /*·SleepEnter() */
               NULL,                        /*·SleepExit() */
               &ketCube_bmeX80_ReadData,    /* GetSensorData() */
               NULL,                        /* SendData() */
               NULL,                        /* ReceiveData() */
               NULL,                        /* ProcessData() */
               ketCube_bmeX80_moduleCfg     /* Module cfg struct */
              ),
#endif
#ifdef KETCUBE_CFG_INC_MOD_LIS2HH12
    DEF_MODULE("LIS2HH12",
               "3 axis accelerometer",
               &ketCube_lis2hh12_Init,      /* Init() */
               NULL,                        /*·SleepEnter() */
               NULL,                        /*·SleepExit() */
               &ketCube_lis2hh12_ReadData,  /* GetSensorData() */
               NULL,                        /* SendData() */
               NULL,                        /* ReceiveData() */
               NULL,                        /* ProcessData() */
               ketCube_lis2hh12_moduleCfg   /* Module cfg struct */
              ),
#endif
#ifdef KETCUBE_CFG_INC_MOD_ICS43432
    DEF_MODULE("ICS43432",
               "MEMS microphone",
               &ketCube_ics43432_Init,      /* Init() */
               NULL,                        /*·SleepEnter() */
               NULL,                        /*·SleepExit() */
               &ketCube_ics43432_ReadData,  /* GetSensorData() */
               NULL,                        /* SendData() */
               NULL,                        /* ReceiveData() */
               NULL,                        /* ProcessData() */
               ketCube_ics43432_moduleCfg   /* Module cfg struct */
              ),
#endif
};
