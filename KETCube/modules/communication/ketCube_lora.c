/**
 * @file    ketCube_lora.c
 * @author  Jan Belohoubek, Jan Zaruba
 * @version 0.2
 * @date    2019-05-15
 * @brief   This file contains the KETCube module wrapper for Semtech's LoRa
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

#include <stdio.h>
#include <string.h>

#include "ketCube_lora.h"
#include "ketCube_cfg.h"
#include "ketCube_common.h"
#include "ketCube_mcu.h"
#include "ketCube_terminal.h"
#include "ketCube_remote_terminal.h"
#include "ketCube_modules.h"
#include "ketCube_rxDisplay.h"

#include "hw.h"
#include "lora.h"
#include "timeServer.h"
#include "lora_mac_version.h"

#include "ketCube_radio.h"
#include "ketCube_spi.h"
#include "ketCube_ad.h"
#include "ketCube_batMeas.h"

#ifdef KETCUBE_CFG_INC_MOD_LORA

/*!
 * LoRaWAN Adaptive Data Rate
 * @note Please note that when ADR is enabled the end-device should be stationary
 */
#define LORAWAN_ADR_ON                              1

/*!
 * LoRaWAN default confirm state
 */
 #define LORAWAN_DEFAULT_CONFIRM_MSG_STATE           LORAWAN_UNCONFIRMED_MSG

/*!
 * LoRaWAN application port
 * @note do not use 224. It is reserved for certification
 */
#define LORAWAN_APP_PORT                            2

/*!
 * LoRaWAN application port for Async messagess
 * @note do not use 224. It is reserved for certification
 */
#define LORAWAN_ASYNCAPP_PORT                       3

#define LORAWAN_HEX_DISPLAY_PORT                    10
#define LORAWAN_STRING_DISPLAY_PORT                 11
#define LORAWAN_CUSTOM_DATA_PORT                    12
#define LORAWAN_REMOTE_TERMINAL_PORT                13
#define LORAWAN_UART2WAN_PORT                       14

/**
 *  LoRa module configuration storage
 */
ketCube_lora_moduleCfg_t ketCube_lora_moduleCfg;

/* call back when server needs endNode to send a frame*/
static void ketCube_lora_TxNeeded ( void );

/* call back when LoRa has received a frame*/
static void ketCube_lora_RxData(lora_AppData_t * AppData);

/* call back when LoRa endNode has just joined*/
static void ketCube_lora_HasJoined( void );

/* call back when LoRa endNode has just switch the class*/
static void ketCube_lora_ConfirmClass ( DeviceClass_t Class );

/* tx timer callback function*/
static void ketCube_lora_MacProcessNotify(void);

static void ketCube_lora_DataConfirm(void);

static ketCube_cfg_ModError_t ketCube_lora_SendData(lora_AppData_t * AppData);

/* Events - move println from ISR */
static volatile bool evntJoined = FALSE;
static volatile bool isJoined = FALSE;
static volatile bool evntClassSwitched = FALSE;
static volatile bool evntTxNeeded = FALSE;
static volatile bool evntACKRx = FALSE;


/* load call backs*/
static LoRaMainCallback_t LoRaMainCallbacks =  {ketCube_batMeas_GetBatteryByte,
                                                ketCube_AD_GetTemperature,
                                                ketCube_MCU_GetUniqueId,
                                                ketCube_MCU_GetRandomSeed,
                                                ketCube_lora_RxData,
                                                ketCube_lora_HasJoined,
                                                ketCube_lora_ConfirmClass,
                                                ketCube_lora_TxNeeded,
                                                ketCube_lora_MacProcessNotify,
                                                ketCube_lora_DataConfirm};

LoraFlagStatus LoraMacProcessRequest = LORA_RESET;


/**
 * Initialises the Lora Parameters
 */
static LoRaParam_t LoRaParamInit = {LORAWAN_ADR_ON,
                                    DR_0,
                                    LORAWAN_PUBLIC_NETWORK};


// KETCube messaging
ketCube_InterModMsg_t ketCube_lora_rxData;
uint8_t ketCube_lora_rxDataBuff[KETCUBE_LORA_RX_BUFFER_LEN];

static ketCube_InterModMsg_t *modMsgQueue[2];

                                                      
/**
 * @brief Load basic module configuration data from EEPROM
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_lora_Init(ketCube_InterModMsg_t *** msg)
{
    /* Initialize HW */
    ketCube_SPI_Init();
    ketCube_Radio_Init();
    ketCube_AD_Init();
    
#ifdef KETCUBE_CFG_INC_MOD_RXDISPLAY
    ketCube_lora_rxData.msg = &(ketCube_lora_rxDataBuff[0]);
    ketCube_lora_rxData.msg[0] = KETCUBE_RXDISPLAY_DATATYPE_DATA;
    ketCube_lora_rxData.msgLen = 0;
    ketCube_lora_rxData.modID = KETCUBE_LISTS_MODULEID_RXDISPLAY;
    
    modMsgQueue[0] = &ketCube_lora_rxData;
    modMsgQueue[1] = NULL;
#else
    modMsgQueue[0] = NULL;
#endif //KETCUBE_CFG_INC_MOD_RXDISPLAY
    
    *msg = &(modMsgQueue[0]);

#if (KETCUBE_LORA_SELCFG_SELECTED == KETCUBE_LORA_SELCFG_KETCube)
    if (lora_ketCubeInit() != KETCUBE_CFG_OK) {
       return KETCUBE_CFG_MODULE_ERROR;
    }
#endif

#if (KETCUBE_LORA_LRWAN_VERSION == KETCUBE_LORA_LRWAN_VERSION_V11x)
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "LoRaWAN SPEC version: 1.1.0");
#else
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "LoRaWAN SPEC version: 1.0.3");
#endif
      
    ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_LORA, "LoRaWAN MAC version: %X", LORA_MAC_VERSION);

    if (ketCube_lora_moduleCfg.devClass <= 2) {
       ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "Device class %c", "ABC"[ketCube_lora_moduleCfg.devClass]);
    } else {
       ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_LORA, "Invalid device class: %d", ketCube_lora_moduleCfg.devClass);
    }
    
    if (ketCube_lora_moduleCfg.txDatarate <= 15) {
       LoRaParamInit.TxDatarate = ketCube_lora_moduleCfg.txDatarate;
    } else {
       ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_LORA, "Invalid uplink datarate: %d", ketCube_lora_moduleCfg.txDatarate);
    }
    
    LORA_Init(&LoRaMainCallbacks, &LoRaParamInit);     
    
    LORA_Join();
    
    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Process lora state and prepare data...
 */
ketCube_cfg_ModError_t ketCube_lora_Send(uint8_t * buffer, uint8_t * len)
{
    lora_AppData_t AppData;
    
    AppData.Buff = buffer;
    AppData.BuffSize = *len;
    AppData.Port = LORAWAN_APP_PORT;
    
    return ketCube_lora_SendData(&AppData);
}

/**
 * @brief Process lora state and prepare data (for asynchronous send)...
 */
ketCube_cfg_ModError_t ketCube_lora_AsyncSend(uint8_t * buffer,
                                              uint8_t * len)
{
    lora_AppData_t AppData;
    
    AppData.Buff = buffer;
    AppData.BuffSize = *len;
    AppData.Port = LORAWAN_ASYNCAPP_PORT;
    
    return ketCube_lora_SendData(&AppData);
}

/**
 * @brief Process lora state and prepare data for remote terminal sending
 * 
 */
ketCube_cfg_ModError_t ketCube_lora_RemoteTerminalSend(uint8_t * buffer,
                                                       uint8_t * len)
{
    lora_AppData_t AppData;
    
    AppData.Buff = buffer;
    AppData.BuffSize = *len;
    AppData.Port = LORAWAN_REMOTE_TERMINAL_PORT;
    
    return ketCube_lora_SendData(&AppData);
}

/**
 * @brief Prepare sleep mode
 *
 * @retval KETCUBE_CFG_MODULE_OK go sleep
 * @retval KETCUBE_CFG_MODULE_ERROR do not go sleep
 */
ketCube_cfg_ModError_t ketCube_lora_SleepEnter(void)
{
    // execute LoRa StateMachine
    if (LoraMacProcessRequest == LORA_SET) {
       // reset notification flag
       LoraMacProcessRequest = LORA_RESET;
       LoRaMacProcess();
    }
    
    if (ketCube_MCU_GetSleepMode() == KETCUBE_MCU_LPMODE_SLEEP) {
        if (isJoined == FALSE) {
            // do not enter SLEEP
            return KETCUBE_CFG_MODULE_ERROR;  
        }
    } else if (LoraMacProcessRequest != LORA_SET) {
        
        return KETCUBE_CFG_MODULE_OK;
    }   
    
    // do not enter SLEEP
    return KETCUBE_CFG_MODULE_ERROR;  
}

/**
 * @brief Exit sleep mode
 *
 * @retval KETCUBE_CFG_MODULE_OK
 * @retval KETCUBE_CFG_MODULE_ERROR
 */
ketCube_cfg_ModError_t ketCube_lora_SleepExit(void)
{   
    // execute LoRa StateMachine
    if (LoraMacProcessRequest == LORA_SET) {
       // reset notification flag
       LoraMacProcessRequest = LORA_RESET;
       LoRaMacProcess();
    }
 
    // Event Handling
    if (evntJoined == TRUE) {
        evntJoined = FALSE;
        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "Joined");
        isJoined = TRUE;
    }
    
    if (evntClassSwitched) {
        evntClassSwitched = FALSE;
        
        DeviceClass_t currClass;
        LORA_GetCurrentClass(&currClass);
        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "Switch to class %c done", "ABC"[currClass]);
    }
    
    if (evntTxNeeded) {
        evntTxNeeded = FALSE;
        ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_LORA, "Network Server is asking for an uplink transmission");
    }
    
    if (evntACKRx) {
        evntACKRx = FALSE;
        ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_LORA, "Network Server \"ack\" an uplink data confirmed message transmission");
    }
   
   
    return KETCUBE_CFG_MODULE_OK;  
}


/**
 * @brief Process custom data 
 * 
 * @param buffer received data
 * 
 * @note Redefine this function if you would like to process data received on port 12
 */
__weak void ketCube_lora_processCustomData(uint8_t * buffer, uint8_t len)
{

}

static ketCube_cfg_ModError_t ketCube_lora_SendData(lora_AppData_t * AppData)
{
   if (LORA_JoinStatus() != LORA_SET) {
      // Not joined, try again later
      if (lora_config_otaa_get() == LORA_ENABLE) {
         ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_LORA, "Not joined");
      }
      LORA_Join();
      return KETCUBE_CFG_MODULE_ERROR;
   }

   if (LORA_send(AppData, LORAWAN_DEFAULT_CONFIRM_MSG_STATE) == LORA_SUCCESS) {
      ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "Transmitting sensor data: SUCCESS");
      return KETCUBE_CFG_MODULE_OK;
   }

   ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_LORA, "Transmitting sensor data: Error");

   return KETCUBE_CFG_MODULE_ERROR;
}

/**
 * @brief LoRa Rx Data callback ...
 */
static void ketCube_lora_RxData(lora_AppData_t * AppData)
{
   uint16_t i;

   ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "Rx DATA=%s on PORT=%d",
   ketCube_common_bytes2Str(&(AppData->Buff[0]), AppData->BuffSize), AppData->Port);

   if (AppData->Port == LORAWAN_CUSTOM_DATA_PORT) {
      // custom data
      ketCube_lora_processCustomData(&(AppData->Buff[0]), AppData->BuffSize);
      return;
   } else if (AppData->Port == LORAWAN_REMOTE_TERMINAL_PORT) {
      ketCube_remoteTerminal_deferCmd((char*)&(AppData->Buff[0]),
          AppData->BuffSize, &ketCube_lora_RemoteTerminalSend);
      return;
   } else if (AppData->Port == LORAWAN_HEX_DISPLAY_PORT) {
      // NOTE: Continue below this conditional block; do not return!
   } else if (AppData->Port == LORAWAN_STRING_DISPLAY_PORT) {
      // NOTE: Continue below this conditional block; do not return!
   } else if (AppData->Port == LORAWAN_UART2WAN_PORT) {
      // NOTE: Continue below this conditional block; do not return!
   } else {
      // unknown port
      return;
   }
   
   // NOTE: just inter module messages are handled here

   // previous msg not processed ... ignore this one
   if (ketCube_lora_rxData.msgLen > 0) {
      return;
   }

   for (i = 0; (i < AppData->BuffSize) && ((i + 1) < KETCUBE_LORA_RX_BUFFER_LEN); i++) {
      ketCube_lora_rxData.msg[i + 1] = AppData->Buff[i];
   }

   // update i to the actual position in ketCube_lora_rxData.msg buffer
   i++;

   if (AppData->Port == LORAWAN_HEX_DISPLAY_PORT) {
      // received HEX
      ketCube_lora_rxData.msg[0] = KETCUBE_RXDISPLAY_DATATYPE_DATA;
   } else if (AppData->Port == LORAWAN_STRING_DISPLAY_PORT) {  
      // received STRING
      if (i < KETCUBE_LORA_RX_BUFFER_LEN) {
         ketCube_lora_rxData.msg[i] = (char) 0;
         i++;
      } else {
         ketCube_lora_rxData.msg[i - 1] = (char) 0;
      }
      ketCube_lora_rxData.msg[0] = KETCUBE_RXDISPLAY_DATATYPE_STRING;
   } 
#ifdef KETCUBE_CFG_INC_MOD_UART2WAN
   else if (AppData->Port == LORAWAN_UART2WAN_PORT) {
        if (i < KETCUBE_LORA_RX_BUFFER_LEN) {
            ketCube_lora_rxData.msg[i] = (char) 0;
            i++;
        } else {
            ketCube_lora_rxData.msg[i - 1] = (char) 0;
        }
        ketCube_lora_rxData.modID = KETCUBE_LISTS_MODULEID_UART2WAN;
   }
#endif

   ketCube_lora_rxData.msgLen = i;
}

static void ketCube_lora_MacProcessNotify(void)
{
   LoraMacProcessRequest = LORA_SET;
}

static void ketCube_lora_HasJoined( void )
{
   if (lora_config_otaa_get() == LORA_ENABLE) {
       evntJoined = TRUE;
   }
   if (ketCube_lora_moduleCfg.devClass <= 2) {
      LORA_RequestClass(ketCube_lora_moduleCfg.devClass);
   }
}

static void ketCube_lora_ConfirmClass(DeviceClass_t Class)
{
    evntClassSwitched = TRUE;
    
    /*Optionnal*/
    /*informs the server that switch has occurred ASAP*/
    lora_AppData_t AppData;
    AppData.BuffSize = 0;
    AppData.Port = LORAWAN_APP_PORT;
    
    LORA_send(&AppData, LORAWAN_UNCONFIRMED_MSG);
}

static void ketCube_lora_TxNeeded(void)
{
    evntTxNeeded = TRUE;

    lora_AppData_t AppData;
    AppData.BuffSize = 0;
    AppData.Port = LORAWAN_APP_PORT;

    LORA_send( &AppData, LORAWAN_UNCONFIRMED_MSG);
}

static void ketCube_lora_DataConfirm (void)
{
    evntACKRx = TRUE;
}

#endif                          /* KETCUBE_CFG_INC_MOD_LORA */
