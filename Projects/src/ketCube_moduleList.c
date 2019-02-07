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

#ifdef KETCUBE_CFG_INC_MOD_LORA
    DEF_MODULE("LoRa",
               "LoRa radio.",
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
     ((char *) &("Display debug output on serial line.")),
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
    {((char *) &("HDC1080")),
     ((char *) &("On-board RHT sensor based on TI HDC1080.")),
     &ketCube_hdc1080_Init,     /*·Module Init() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·SleepEnter() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·SleepExit() */
     &ketCube_hdc1080_ReadData, /*·GetSensorData() */
     (ketCube_cfg_ModDataFn_t) NULL,    /*·SendData() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·ReceiveData() */
     (ketCube_cfg_ModDataPtrFn_t) NULL, /*·ProcessData() */
     0,                         /*·CFG base addr -- set dynamicaly */
     1,                         /*·CFG len in bytes */
     TRUE                       /*·module CFG byte -- set dynamically */
     },
#endif
#ifdef KETCUBE_CFG_INC_MOD_BATMEAS
    {((char *) &("batMeas")),
     ((char *) &("On-chip battery voltage measurement.")),
     &ketCube_batMeas_Init,     /*·Module Init() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·SleepEnter() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·SleepExit() */
     &ketCube_batMeas_ReadData, /*·GetSensorData() */
     (ketCube_cfg_ModDataFn_t) NULL,    /*·SendData() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·ReceiveData() */
     (ketCube_cfg_ModDataPtrFn_t) NULL, /*·ProcessData() */
     0,                         /*·CFG base addr -- set dynamicaly */
     2,                         /*·CFG len in bytes */
     TRUE                       /*·module CFG byte -- set dynamically */
     },
#endif
#ifdef KETCUBE_CFG_INC_MOD_ADC
    {((char *) &("ADC")),
     ((char *) &("Measure mVolts on PA4.")),
     &ketCube_ADC_Init,         /*·Module Init() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·SleepEnter() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·SleepExit() */
     &ketCube_ADC_ReadData,     /*·GetSensorData() */
     (ketCube_cfg_ModDataFn_t) NULL,    /*·SendData() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·ReceiveData() */
     (ketCube_cfg_ModDataPtrFn_t) NULL, /*·ProcessData() */
     0,                         /*·CFG base addr -- set dynamicaly */
     1,                         /*·CFG len in bytes */
     TRUE                       /*·module CFG byte -- set dynamically */
     },
#endif
#ifdef KETCUBE_CFG_INC_MOD_STARNET
    {((char *) &("StarNetConcentrator")),
     ((char *)
      &("Compose a star net with this module as network concentrator.")),
     &ketCube_starNet_ConcentratorInit, /*·Module Init() */
     &ketCube_starNet_SleepEnter,       /*·SleepEnter() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·SleepExit() */
     (ketCube_cfg_ModDataFn_t) NULL,    /*·GetSensorData() */
     (ketCube_cfg_ModDataFn_t) NULL,    /*·SendData() */
     &ketCube_starNet_receiveData,      /*·ReceiveData() */
     (ketCube_cfg_ModDataPtrFn_t) NULL, /*·ProcessData() */
     0,                         /*·CFG base addr -- set dynamicaly */
     1,                         /*·CFG len in bytes */
     TRUE                       /*·module CFG byte -- set dynamically */
     },

    {((char *) &("StarNetNode")),
     ((char *) &("Compose a star net with this module as sensor node.")),
     &ketCube_starNet_NodeInit, /*·Module Init() */
     &ketCube_starNet_SleepEnter,       /*·SleepEnter() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·SleepExit() */
     (ketCube_cfg_ModDataFn_t) NULL,    /*·GetSensorData() */
     &ketCube_starNet_sendData, /*·SendData() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·ReceiveData() */
     (ketCube_cfg_ModDataPtrFn_t) NULL, /*·ProcessData() */
     0,                         /*·CFG base addr -- set dynamicaly */
     1,                         /*·CFG len in bytes */
     TRUE                       /*·module CFG byte -- set dynamically */
     },
#endif
#ifdef KETCUBE_CFG_INC_MOD_FDC2214
    {((char *) &("FDC2214")),
     ((char *) &("TI\'s FDC2214 capacity to digital converter.")),
     &ketCube_fdc2214_Init,     /*·Module Init() */
     &ketCube_fdc2214_SleepEnter,       /*·SleepEnter() */
     &ketCube_fdc2214_SleepExit,        /*·SleepExit() */
     &ketCube_fdc2214_ReadData, /*·GetSensorData() */
     (ketCube_cfg_ModDataFn_t) NULL,    /*·SendData() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·ReceiveData() */
     (ketCube_cfg_ModDataPtrFn_t) NULL, /*·ProcessData() */
     0,                         /*·CFG base addr -- set dynamicaly */
     1,                         /*·CFG len in bytes */
     TRUE                       /*·module CFG byte -- set dynamically */
     },
#endif
#ifdef KETCUBE_CFG_INC_MOD_RXDISPLAY
    {((char *) &("RxDisplay")),
     ((char *) &("Display received data on serial line.")),
     &ketCube_rxDisplay_Init,   /*·Module Init() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·SleepEnter() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·SleepExit() */
     (ketCube_cfg_ModDataFn_t) NULL,    /*·GetSensorData() */
     (ketCube_cfg_ModDataFn_t) NULL,    /*·SendData() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·ReceiveData() */
     &ketCube_rxDisplay_ProcessData,    /*·ProcessData() */
     0,                         /*·CFG base addr -- set dynamicaly */
     1,                         /*·CFG len in bytes */
     TRUE                       /*·module CFG byte -- set dynamically */
     },
#endif
#ifdef KETCUBE_CFG_INC_MOD_ASYNCTX
    {((char *) &("AsyncTx")),
     ((char *) &("Asynchronously transmit data.")),
     &ketCube_AsyncTx_Init,     /*·Module Init() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·SleepEnter() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·SleepExit() */
     (ketCube_cfg_ModDataFn_t) NULL,    /*·GetSensorData() */
     (ketCube_cfg_ModDataFn_t) NULL,    /*·SendData() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·ReceiveData() */
     &ketCube_AsyncTx_ProcessData,      /*·ProcessData() */
     0,                         /*·CFG base addr -- set dynamicaly */
     1,                         /*·CFG len in bytes */
     TRUE                       /*·module CFG byte -- set dynamically */
     },
#endif
#ifdef KETCUBE_CFG_INC_MOD_TXDISPLAY
    {((char *) &("TxDisplay")),
     ((char *) &("Display Tx KETCube data.")),
     &ketCube_txDisplay_Init,   /*·Module Init() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·SleepEnter() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·SleepExit() */
     (ketCube_cfg_ModDataFn_t) NULL,    /*·GetSensorData() */
     &ketCube_txDisplay_Send,   /*·SendData() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*·ReceiveData() */
     (ketCube_cfg_ModDataPtrFn_t) NULL, /*·ProcessData() */
     0,                         /*·CFG base addr -- set dynamicaly */
     1,                         /*·CFG len in bytes */
     TRUE                       /*·module CFG byte -- set dynamically */
     },
#endif
#ifdef KETCUBE_CFG_INC_MOD_BMEX80
    {((char *) &("BMEx80")),
     ((char *)
      &("On-board environmental sensor based on Bosch BME family.")),
     &ketCube_bmeX80_Init,      /* Module Init() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /* SleepEnter() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /* SleepExit() */
     &ketCube_bmeX80_ReadData,  /* GetSensorData() */
     (ketCube_cfg_ModDataFn_t) NULL,    /* SendData() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /* ReceiveData() */
     (ketCube_cfg_ModDataPtrFn_t) NULL, /* ProcessData() */
     0,                         /* CFG base addr -- set dynamicaly */
     1,                         /* CFG len in bytes */
     TRUE                       /* module CFG byte -- set dynamically */
     },
#endif
#ifdef KETCUBE_CFG_INC_MOD_LIS2HH12
    {((char *) &("LIS2HH12")),
     ((char *) &("3 axis accelerometer")),
     &ketCube_lis2hh12_Init,    /* Module Init() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /* SleepEnter() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /* SleepExit() */
     &ketCube_lis2hh12_ReadData,        /* GetSensorData() */
     (ketCube_cfg_ModDataFn_t) NULL,    /* SendData() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /* ReceiveData() */
     (ketCube_cfg_ModDataPtrFn_t) NULL, /* ProcessData() */
     0,                         /* CFG base addr -- set dynamicaly */
     1,                         /* CFG len in bytes */
     TRUE                       /* module CFG byte -- set dynamically */
     },
#endif
#ifdef KETCUBE_CFG_INC_MOD_ICS43432
    {((char *) &("ICS43432")),
     ((char *) &("MEMS microphone")),
     &ketCube_ics43432_Init,    /*Module Init() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*SleepEnter() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*SleepExit() */
     &ketCube_ics43432_ReadData,        /*GetSensorData() */
     (ketCube_cfg_ModDataFn_t) NULL,    /*SendData() */
     (ketCube_cfg_ModVoidFn_t) NULL,    /*ReceiveData() */
     (ketCube_cfg_ModDataPtrFn_t) NULL, /*ProcessData() */
     0,                         /* CFG base addr -- set dynamicaly */
     1,                         /* CFG len in bytes */
     TRUE                       /* module CFG byte -- set dynamically */
     },
#endif
};
