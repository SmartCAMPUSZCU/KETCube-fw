/**
 * @file    ketCube_starNet.c
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2019-08-27
 * @brief   This file contains the KETCube starNet node and concentrator modules
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

ketCube_starNet_moduleCfg_t ketCube_starNetConcentrator_moduleCfg;       /*!< Module starNetConcentrator configuration storage */
ketCube_starNet_moduleCfg_t ketCube_starNetNode_moduleCfg;               /*!< Module starNetNode configuration storage */

/* private function prototypes */
static void ketCube_starNet_OnRxError(void);
static void ketCube_starNet_OnRxTimeout(void);
static void ketCube_starNet_OnTxTimeout(void);
static void ketCube_starNet_OnTxDone(void);
static void ketCube_starNet_OnRxDone(uint8_t * payload, uint16_t size,
                                     int16_t rssi, int8_t snr);
ketCube_cfg_ModError_t ketCube_starNet_Init(ketCube_starNet_NodeType_t
                                            nodeType);

/* private variables */
static RadioEvents_t RadioEvents;
uint8_t txBuffer_len = 0;   /* how many bytes are ready to send */

/* State machine control */
static ketCube_starNet_State_t moduleState;
static uint8_t ketCube_starNet_dataBuff[KETCUBE_STARNET_DATA_BUFFER_LEN];

/* global variables */
ketCube_starNet_NodeType_t ketCube_starNet_nodeType;
ketCube_starNet_NodeType_t nodeType; /*!< This device type: Node/Concentrator */

/* FSK parameters */
#define FSK_FDEV                                    25000       /*!<  Hz */
#define FSK_DATARATE                                50000       /*!<  bps */
#define FSK_BANDWIDTH                               50000       /*!<  Hz */
#define FSK_AFC_BANDWIDTH                           83333       /*!<  Hz */
#define FSK_PREAMBLE_LENGTH                         5           /*!<  Same for Tx and Rx */
#define FSK_FIX_LENGTH_PAYLOAD_ON                   FALSE
#define RX_TIMEOUT_VALUE                            1000
#define TX_TIMEOUT_VALUE                            500         /*!< Timeout should be small, it should not occur in general, but it occurs (with some parts), see the in-code note above the woraround */

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
        
        switch (moduleState) {
            case KETCUBE_STARNET_STATE_RX_DONE:
                ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_STARNET_CONCENTRATOR,
                                             "RSSI=%d", (int16_t) (ketCube_starNet_dataBuff[0] | (uint16_t) (ketCube_starNet_dataBuff[1] << 8)));
                ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_STARNET_CONCENTRATOR,
                                             "SNR=%d", (int8_t) ketCube_starNet_dataBuff[2]);
                ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_STARNET_CONCENTRATOR,
                                             "DATA=%s", 
                                             ketCube_common_bytes2Str(&(ketCube_starNet_dataBuff[3]), txBuffer_len-3));
                break;
            case KETCUBE_STARNET_STATE_RX_TIMEOUT:
                ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_STARNET_CONCENTRATOR, "Receiving data: TIMEOUT");
                break;
            case KETCUBE_STARNET_STATE_RX_ERROR:
                ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_STARNET_CONCENTRATOR, "Receiving data: ERROR");
                break;
            case KETCUBE_STARNET_STATE_RX_READY:
                /* ready to start */
                break;
            case KETCUBE_STARNET_STATE_RX_PROGRESS:
                // Do nothing -> exit!
                
                /* Do not enter sleep mode */
                return KETCUBE_CFG_MODULE_ERROR;
            default:
                // in case of misconfiguration ... should never happen
                break;
        }
        
        /* Initialize Rx */
        ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_STARNET_CONCENTRATOR, "Rx START");
        moduleState = KETCUBE_STARNET_STATE_RX_PROGRESS;        
        Radio.Rx(RX_TIMEOUT_VALUE);
                
        /* Do not enter sleep mode */
        return KETCUBE_CFG_MODULE_ERROR;
    } else {
        
        /* State machione execution - do it periodically here*/
        switch (moduleState) {
            case KETCUBE_STARNET_STATE_TX_TIMEOUT:
                ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_STARNET_NODE, "Transmitting sensor data: TIMEOUT");
                
                // The following note is a part of sx1276 code:
                // ---
                //
                // Tx timeout shouldn't happen.
                // But it has been observed that when it happens it is a result of a corrupted SPI transfer
                // it depends on the platform design.
                //
                // The workaround is to put the radio in a known state. Thus, we re-initialize it.
                //
                // ---
                // It has been observed, that Tx Timeout occurs (often) with some KETCubes, 
                // but with other pieces of HW (from the same manufacturing serie), 
                // it has not been observed ... it seems, that the problem is part-specific, 
                // not design-specific ... strange!
                //
                HAL_Delay(10);
                ketCube_starNet_Init(KETCUBE_STARNET_NODE);
                HAL_Delay(10);
                
                moduleState = KETCUBE_STARNET_STATE_TX_READY;
                break;
            case KETCUBE_STARNET_STATE_TX_NEW_DATA:
                ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_STARNET_NODE,
                                 "Transmitting sensor data: %s",
                                 ketCube_common_bytes2Str(&(ketCube_starNet_dataBuff[0]), txBuffer_len));
                moduleState = KETCUBE_STARNET_STATE_TX_PROGRESS;
                
                HAL_Delay(10);
                
                Radio.Send(&(ketCube_starNet_dataBuff[0]), txBuffer_len);
                break;
            case KETCUBE_STARNET_STATE_TX_DONE:
                ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_STARNET_NODE, "Transmitting sensor data: SUCCESS");
                moduleState = KETCUBE_STARNET_STATE_TX_READY;
                break;
            case KETCUBE_STARNET_STATE_TX_ERROR:
                ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_STARNET_NODE, "Transmitting sensor data: ERROR");
                moduleState = KETCUBE_STARNET_STATE_TX_READY;
                break;
            case KETCUBE_STARNET_STATE_TX_PROGRESS:
                // do nothing
                break;
            default:
                // in case of misconfiguration ... should never happen
                moduleState = KETCUBE_STARNET_STATE_TX_READY;
                break;
        }
        
        /* Do not enter sleep mode */
        return KETCUBE_CFG_MODULE_ERROR;
        //return KETCUBE_CFG_MODULE_OK;
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

    Radio.Init(&RadioEvents);
    Radio.SetChannel(KETCUBE_STARNET_RF_FREQUENCY);

    if (nodeType == KETCUBE_STARNET_CONCENTRATOR) {
        RadioEvents.RxDone = ketCube_starNet_OnRxDone;
        RadioEvents.RxTimeout = ketCube_starNet_OnRxTimeout;
        RadioEvents.RxError = ketCube_starNet_OnRxError;
        Radio.SetRxConfig(MODEM_FSK, FSK_BANDWIDTH,
                          FSK_DATARATE, 0, 
                          FSK_AFC_BANDWIDTH, FSK_PREAMBLE_LENGTH,
                          0, FSK_FIX_LENGTH_PAYLOAD_ON, 0, TRUE,
                          0, 0, FALSE, TRUE);
        
        moduleState = KETCUBE_STARNET_STATE_RX_READY;
    } else {
        RadioEvents.TxDone = ketCube_starNet_OnTxDone;
        RadioEvents.TxTimeout = ketCube_starNet_OnTxTimeout;
        Radio.SetTxConfig(MODEM_FSK, KETCUBE_STARNET_TX_OUTPUT_POWER,
                          FSK_FDEV, 0, FSK_DATARATE, 0,
                          FSK_PREAMBLE_LENGTH, FSK_FIX_LENGTH_PAYLOAD_ON,
                          TRUE, 0, 0, 0, TX_TIMEOUT_VALUE);
        
        moduleState = KETCUBE_STARNET_STATE_TX_READY;
    }
    
    return KETCUBE_CFG_MODULE_OK;
}

/**
  * @brief Initialize starNet Concentrator
  * 
  * @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t
ketCube_starNet_ConcentratorInit(ketCube_InterModMsg_t *** msg)
{
    nodeType = KETCUBE_STARNET_CONCENTRATOR;
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
    if (moduleState != KETCUBE_STARNET_STATE_TX_READY) {
        /* Tx in progress */
        return KETCUBE_CFG_MODULE_ERROR;
    }
    
    memcpy(&(ketCube_starNet_dataBuff[0]),  &(buffer[0]), ketCube_common_Min(*len, KETCUBE_STARNET_DATA_BUFFER_LEN));
    txBuffer_len = ketCube_common_Min(*len, KETCUBE_STARNET_DATA_BUFFER_LEN);
    moduleState = KETCUBE_STARNET_STATE_TX_NEW_DATA;
    
    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief This function is executed on Tx Done event
 */
static void ketCube_starNet_OnTxDone(void)
{
    moduleState = KETCUBE_STARNET_STATE_TX_DONE;
    Radio.Sleep();
}

/**
 * @brief This function is executed on Rx Done event
 */
static void ketCube_starNet_OnRxDone(uint8_t * payload, uint16_t size,
                                     int16_t rssi, int8_t snr)
{
    Radio.Sleep();
    moduleState = KETCUBE_STARNET_STATE_RX_DONE;


    ketCube_starNet_dataBuff[0] = (uint8_t) (rssi & 0xFF);
    ketCube_starNet_dataBuff[1] = (uint8_t) (((uint16_t) rssi >> 8) & 0xFF);
    ketCube_starNet_dataBuff[2] = (uint8_t) snr;

    memcpy(&(ketCube_starNet_dataBuff[3]), &(payload[0]), ketCube_common_Min(size, (KETCUBE_STARNET_DATA_BUFFER_LEN - 3)));
    txBuffer_len = ketCube_common_Min((size + 3), KETCUBE_STARNET_DATA_BUFFER_LEN);
}

/**
 * @brief This function is executed on radio Tx Timeout event
 */
static void ketCube_starNet_OnTxTimeout(void)
{
    moduleState = KETCUBE_STARNET_STATE_TX_TIMEOUT;
    Radio.Sleep();
}

/**
 * @brief This function is executed on radio Rx Timeout event
 */
static void ketCube_starNet_OnRxTimeout(void)
{
    moduleState = KETCUBE_STARNET_STATE_RX_TIMEOUT;
    Radio.Sleep();
}

/**
 * @brief This function is executed on radio Rx Error event
 */
static void ketCube_starNet_OnRxError(void)
{
    moduleState = KETCUBE_STARNET_STATE_RX_ERROR;
    Radio.Sleep();
}

#endif                          /* KETCUBE_CFG_INC_MOD_STARNET */
