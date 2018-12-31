/**
 * @file    ketCube_lora.c
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2018-11-20
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
#include "ketCube_terminal.h"
#include "ketCube_modules.h"
#include "ketCube_rxDisplay.h"

#include "hw.h"
#include "lora.h"
#include "timeServer.h"
#include "version.h"

#include "ketCube_i2c.h"
#include "ketCube_adc.h"
#include "ketCube_hdc1080.h"

#ifdef KETCUBE_CFG_INC_MOD_LORA

/*!
 * LoRaWAN Adaptive Data Rate
 * @note Please note that when ADR is enabled the end-device should be static
 */
#define LORAWAN_ADR_ON                              1
/*!
 * LoRaWAN confirmed messages
 */
#define LORAWAN_CONFIRMED_MSG                    DISABLE
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

/* call back when LoRa will transmit a frame*/
static void ketCube_lora_TxData(lora_AppData_t * AppData,
                                FunctionalState * IsTxConfirmed);

/* call back when LoRa has received a frame*/
static void ketCube_lora_RxData(lora_AppData_t * AppData);

/* TxLED turnOFF */
static void ketCube_lora_OnTimerLed(void);

/* Private variables ---------------------------------------------------------*/
static TimerEvent_t TxLedTimer;

/* load call backs*/
static LoRaMainCallback_t LoRaMainCallbacks = { HW_GetBatteryLevel,
    HW_GetUniqueId,
    HW_GetRandomSeed,
    ketCube_lora_TxData,
    ketCube_lora_RxData
};


/* !
 *Initialises the Lora Parameters
 */
static LoRaParam_t LoRaParamInit = { TX_ON_TIMER,
    10000,                      // is not valid. The true period is given by KETCUBE_PERIOD !!
    CLASS_A,
    LORAWAN_ADR_ON,
    DR_0,
    LORAWAN_PUBLIC_NETWORK
};

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
    ketCube_lora_rxData.msg = &(ketCube_lora_rxDataBuff[0]);
    ketCube_lora_rxData.msg[0] = KETCUBE_RXDISPLAY_DATATYPE_DATA;
    ketCube_lora_rxData.msgLen = 0;
    ketCube_lora_rxData.modID = KETCUBE_LISTS_MODULEID_RXDISPLAY;

    modMsgQueue[0] = &ketCube_lora_rxData;
    modMsgQueue[1] = NULL;

    *msg = &(modMsgQueue[0]);

#if (KETCUBE_LORA_SELCFG_SELECTED == KETCUBE_LORA_SELCFG_KETCube)
    if (lora_ketCubeInit() != KETCUBE_CFG_OK) {
        return KETCUBE_CFG_MODULE_ERROR;
    }
#endif
    lora_Init(&LoRaMainCallbacks, &LoRaParamInit);
    KETCUBE_TERMINAL_PRINTF("VERSION: %X\n\r", VERSION);

    return KETCUBE_CFG_MODULE_OK;
}

static uint8_t *dataBuffer;
static uint8_t dataBufferLen;
static uint8_t txPort;

/**
 * @brief Process lora state and prepare data...
 */
ketCube_cfg_ModError_t ketCube_lora_Send(uint8_t * buffer, uint8_t * len)
{
    txPort = LORAWAN_APP_PORT;
    dataBuffer = buffer;
    dataBufferLen = *len;

    // when timer elapsed tx next packet or join ...
    OnSendEvent();

    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Process lora state and prepare data (for asynchronous send)...
 */
ketCube_cfg_ModError_t ketCube_lora_AsyncSend(uint8_t * buffer,
                                              uint8_t * len)
{
    txPort = LORAWAN_ASYNCAPP_PORT;
    dataBuffer = buffer;
    dataBufferLen = *len;

    // when timer elapsed tx next packet or join ...
    OnSendEvent();

    return KETCUBE_CFG_MODULE_OK;
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
    lora_fsm();

    if (lora_getDeviceState() == DEVICE_STATE_SLEEP) {
        return KETCUBE_CFG_MODULE_OK;
    } else {
        return KETCUBE_CFG_MODULE_ERROR;
    }
}


/**
 * @brief LoRa Tx Data callback ...
 */
static void ketCube_lora_TxData(lora_AppData_t * AppData,
                                FunctionalState * IsTxConfirmed)
{
    AppData->Port = txPort;
    *IsTxConfirmed = LORAWAN_CONFIRMED_MSG;

    memcpy(&(AppData->Buff[0]), &(dataBuffer[0]), dataBufferLen);

    AppData->BuffSize = dataBufferLen;

    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "Transmitting sensor data ...");

    TimerInit(&TxLedTimer, ketCube_lora_OnTimerLed);
    TimerSetValue(&TxLedTimer, 200);
    //KETCUBE_MAIN_BOARD_LED1_On();
    TimerStart(&TxLedTimer);
}


/**
 * @brief Process custom data 
 * 
 * @param buffer received data
 * 
 * @note Redefine this function if you would like to process data received on port 12
 */
__weak void ketCube_lora_processCustomData(uint8_t * buffer, uint8_t len) {
    
}

/**
 * @brief LoRa Rx Data callback ...
 */
static void ketCube_lora_RxData(lora_AppData_t * AppData)
{
    uint16_t i;

    // previous msg not processed ... ignore this one
    if (ketCube_lora_rxData.msgLen > 0) {
        return;
    }

    for (i = 0; (i < AppData->BuffSize) && ((3*(i+1)) < KETCUBE_COMMON_BUFFER_LEN); i++) {
        sprintf(&(ketCube_common_buffer[3*i]), "%02X-", AppData->Buff[i]);
    }
    ketCube_common_buffer[(3*i)-1] = 0x00; // remove last -
    
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "Rx DATA=%s", &(ketCube_common_buffer[0]));
    
    for (i = 0; (i < AppData->BuffSize) && ((i + 1) < KETCUBE_LORA_RX_BUFFER_LEN); i++) {
        ketCube_lora_rxData.msg[i + 1] = AppData->Buff[i];
    }

    // update i to the actual position in ketCube_lora_rxData.msg buffer
    i++;

    if (AppData->Port == 10) {  // received HEX
        ketCube_lora_rxData.msg[0] = KETCUBE_RXDISPLAY_DATATYPE_DATA;
    } else if (AppData->Port == 11) {   // received STRING
        if (i < KETCUBE_LORA_RX_BUFFER_LEN) {
            ketCube_lora_rxData.msg[i] = (char) 0;
            i++;
        } else {
            ketCube_lora_rxData.msg[i - 1] = (char) 0;
        }
        ketCube_lora_rxData.msg[0] = KETCUBE_RXDISPLAY_DATATYPE_STRING;
    } else if (AppData->Port == 12) {
        // custom data
        ketCube_lora_processCustomData(&(AppData->Buff[0]), AppData->BuffSize);
    }

    ketCube_lora_rxData.msgLen = i;
}

/**
 * @brief LoRa Tx LED callback ...
 */
static void ketCube_lora_OnTimerLed(void)
{
    //KETCUBE_MAIN_BOARD_LED1_Off();
}

#endif                          /* KETCUBE_CFG_INC_MOD_LORA */
