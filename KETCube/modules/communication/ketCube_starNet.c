/**
 * @file    ketCube_starNet.c
 * @author  Jan Belohoubek
 * @version alpha
 * @date    2018-03-02
 * @brief   This file contains the KETCube starNet module
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
#include "hw.h"
#include "radio.h"
#include "low_power.h"
#include "ketCube_starNet.h"
#include "ketCube_common.h"
#include "ketCube_terminal.h"
#include "ketCube_modules.h"

#include "ketCube_i2c.h"
#include "ketCube_adc.h"
#include "ketCube_hdc1080.h"
#include "ketCube_rxDisplay.h"

#ifdef KETCUBE_CFG_INC_MOD_STARNET

// Node Type
ketCube_starNet_NodeType_t nodeType;

// private functions
static void ketCube_starNet_OnRxError(void);
static void ketCube_starNet_OnRxTimeout(void);
static void ketCube_starNet_OnTxTimeout(void);
static void ketCube_starNet_OnTxDone(void);
static void ketCube_starNet_OnRxDone(uint8_t * payload, uint16_t size,
                                     int16_t rssi, int8_t snr);
static void ketCube_starNet_OnCadDone(bool channelActivityDetected);

// provate variables
static RadioEvents_t RadioEvents;

static volatile uint8_t rxDone = TRUE;
static volatile uint8_t txDone = TRUE;

// global variables
ketCube_starNet_NodeType_t ketCube_starNet_nodeType;

// KETCube messaging
ketCube_InterModMsg_t ketCube_starNet_rssi;
ketCube_InterModMsg_t ketCube_starNet_snr;
ketCube_InterModMsg_t ketCube_starNet_rxData;
uint8_t ketCube_starNet_rxDataBuff[KETCUBE_STARNET_RX_DATA_BUFFER_LEN];
uint8_t ketCube_starNet_snrMsgBuff[2];
uint8_t ketCube_starNet_rssiMsgBuff[2];

static ketCube_InterModMsg_t *modMsgQueue[4];


#define FSK_FDEV                                    25e3        // Hz
#define FSK_DATARATE                                50e3        // bps
#define FSK_BANDWIDTH                               50e3        // Hz
#define FSK_AFC_BANDWIDTH                           83.333e3    // Hz
#define FSK_PREAMBLE_LENGTH                         5   // Same for Tx and Rx
#define FSK_FIX_LENGTH_PAYLOAD_ON                   FALSE
#define RX_TIMEOUT_VALUE                            1000

/**
 * @brief Prepare sleep mode
 *
 * @retval KETCUBE_CFG_MODULE_OK go sleep
 * @retval KETCUBE_CFG_MODULE_ERROR do not go sleep
 *
 * @todo starNet module currently does not support sleep mode
 */
ketCube_cfg_ModError_t ketCube_starNet_SleepEnter(void)
{
    if (ketCube_starNet_nodeType == KETCUBE_STARNET_CONCENTRATOR) {
        return KETCUBE_CFG_MODULE_ERROR;
    } else {
        // will be changed for nodes in the future...
        return KETCUBE_CFG_MODULE_ERROR;
    }
}

/**
  * @brief Initialize starNet module
	* @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_starNet_Init(ketCube_starNet_NodeType_t
                                            nodeType)
{
    ketCube_starNet_nodeType = nodeType;

    // Radio initialization
    RadioEvents.TxDone = NULL;
    RadioEvents.RxDone = NULL;
    RadioEvents.TxTimeout = NULL;
    RadioEvents.RxTimeout = NULL;
    RadioEvents.RxError = NULL;
    RadioEvents.CadDone = NULL;
    if (nodeType == KETCUBE_STARNET_CONCENTRATOR) {
        RadioEvents.RxDone = ketCube_starNet_OnRxDone;
        RadioEvents.RxTimeout = ketCube_starNet_OnRxTimeout;
        RadioEvents.RxError = ketCube_starNet_OnRxError;
        Radio.SetRxConfig(MODEM_FSK, FSK_BANDWIDTH, FSK_DATARATE,
                          0, FSK_AFC_BANDWIDTH, FSK_PREAMBLE_LENGTH,
                          0, FSK_FIX_LENGTH_PAYLOAD_ON, 0, TRUE,
                          0, 0, FALSE, TRUE);
    } else {
        RadioEvents.TxDone = ketCube_starNet_OnTxDone;
        RadioEvents.TxTimeout = ketCube_starNet_OnTxTimeout;
        RadioEvents.CadDone = ketCube_starNet_OnCadDone;
        Radio.SetTxConfig(MODEM_FSK, KETCUBE_STARNET_TX_OUTPUT_POWER,
                          FSK_FDEV, 0, FSK_DATARATE, 0,
                          FSK_PREAMBLE_LENGTH, FSK_FIX_LENGTH_PAYLOAD_ON,
                          TRUE, 0, 0, 0, 10000);
    }

    Radio.Init(&RadioEvents);
    Radio.SetChannel(KETCUBE_STARNET_RF_FREQUENCY);

    return KETCUBE_CFG_MODULE_OK;
}

/**
  * @brief Initialize starNet Concentrator
	* @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t
ketCube_starNet_ConcentratorInit(ketCube_InterModMsg_t *** msg)
{
    nodeType = KETCUBE_STARNET_CONCENTRATOR;

    ketCube_starNet_rssi.msg = &(ketCube_starNet_rssiMsgBuff[0]);
    ketCube_starNet_rssi.msg[0] = KETCUBE_RXDISPLAY_DATATYPE_RSSI;
    ketCube_starNet_rssi.msgLen = 0;
    ketCube_starNet_rssi.modID = KETCUBE_LISTS_MODULEID_RXDISPLAY;

    ketCube_starNet_snr.msg = &(ketCube_starNet_snrMsgBuff[0]);
    ketCube_starNet_snr.msg[0] = KETCUBE_RXDISPLAY_DATATYPE_SNR;
    ketCube_starNet_snr.msgLen = 0;
    ketCube_starNet_snr.modID = KETCUBE_LISTS_MODULEID_RXDISPLAY;

    ketCube_starNet_rxData.msg = &(ketCube_starNet_rxDataBuff[0]);
    ketCube_starNet_rxData.msg[0] = KETCUBE_RXDISPLAY_DATATYPE_DATA;
    ketCube_starNet_rxData.msgLen = 0;
    ketCube_starNet_rxData.modID = KETCUBE_LISTS_MODULEID_RXDISPLAY;

    modMsgQueue[0] = &ketCube_starNet_rssi;
    modMsgQueue[1] = &ketCube_starNet_snr;
    modMsgQueue[2] = &ketCube_starNet_rxData;
    modMsgQueue[3] = NULL;

    *msg = &(modMsgQueue[0]);

    return ketCube_starNet_Init(KETCUBE_STARNET_CONCENTRATOR);
}

/**
  * @brief Initialize starNet Node
  *
	* @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_starNet_NodeInit(ketCube_InterModMsg_t ***
                                                msg)
{
    nodeType = KETCUBE_STARNET_NODE;
    return ketCube_starNet_Init(KETCUBE_STARNET_NODE);
}

/**
  * @brief Receive sensor data
  *
  * @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_starNet_receiveData(void)
{
    if (rxDone == TRUE) {
        ketCube_terminal_DebugPrintln("starNet :: Rx START");
        rxDone = FALSE;
        Radio.Rx(RX_TIMEOUT_VALUE);
    }
    return KETCUBE_CFG_MODULE_OK;
}

/**
  * @brief Send sensor data
	*
  * @param buffer data buffer
  * @param len data length
  *
	* @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_starNet_sendData(uint8_t * buffer,
                                                uint8_t * len)
{
    uint8_t i;

    if (txDone != TRUE) {
        return KETCUBE_CFG_MODULE_ERROR;
    }
    txDone = FALSE;

    ketCube_terminal_DebugPrint("StarNet :: transmitting sensor data: ");
    ketCube_terminal_DebugPrint("DATA=");
    for (i = 0; i < *len; i++) {
        ketCube_terminal_DebugPrint("%02X-", buffer[i]);
    }
    ketCube_terminal_DebugPrintln("\b; ");

    Radio.Send(buffer, *len);

    return KETCUBE_CFG_MODULE_OK;
}

static void ketCube_starNet_OnTxDone(void)
{
    ketCube_terminal_DebugPrintln("starNet :: Tx DONE");

    txDone = TRUE;

    Radio.Sleep();
}

static void ketCube_starNet_OnRxDone(uint8_t * payload, uint16_t size,
                                     int16_t rssi, int8_t snr)
{
    uint16_t i;

    Radio.Sleep();

    rxDone = TRUE;

    // previous msg not processed ... ignore this one
    if ((ketCube_starNet_rssi.msgLen > 0) ||
        (ketCube_starNet_snr.msgLen > 0) ||
        (ketCube_starNet_rxData.msgLen > 0)) {
        return;
    }

    ketCube_starNet_rssi.msg[1] = rssi;
    ketCube_starNet_snr.msg[1] = snr;

    ketCube_terminal_DebugPrint("starNet :: Rx :: RSSI=%d; ", (int) rssi);
    ketCube_terminal_DebugPrint("SNR=%d; ", (int) snr);

    ketCube_terminal_DebugPrint("DATA=");
    for (i = 0;
         (i < size) && ((i + 1) < KETCUBE_STARNET_RX_DATA_BUFFER_LEN);
         i++) {
        ketCube_terminal_DebugPrint("%02X-", payload[i]);
        ketCube_starNet_rxData.msg[i + 1] = payload[i];
    }
    ketCube_terminal_DebugPrintln("\b; ");

    ketCube_starNet_rssi.msgLen = 2;
    ketCube_starNet_snr.msgLen = 2;
    ketCube_starNet_rxData.msgLen = i + 1;
}

static void ketCube_starNet_OnTxTimeout(void)
{
    ketCube_terminal_DebugPrintln("starNet :: Tx TIMEOUT");

    txDone = TRUE;

    Radio.Sleep();
}

static void ketCube_starNet_OnRxTimeout(void)
{
    ketCube_terminal_DebugPrintln("starNet :: Rx :: TIMEOUT");

    rxDone = TRUE;

    Radio.Sleep();
}

static void ketCube_starNet_OnRxError(void)
{
    ketCube_terminal_DebugPrintln("starNet :: Rx :: ERROR");

    rxDone = TRUE;

    Radio.Sleep();
}

static void ketCube_starNet_OnCadDone(bool channelActivityDetected)
{
    ketCube_terminal_DebugPrintln
        ("starNet :: Tx :: ketCube_starNet_OnCadDone");

    txDone = TRUE;

    Radio.Sleep();
}

#endif                          /* KETCUBE_CFG_INC_MOD_STARNET */
