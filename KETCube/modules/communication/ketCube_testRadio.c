/**
 * @file    ketCube_testRadio.c
 * @author  Jan Belohoubek
 * @version 0.2-dev
 * @date    2020-06-18
 * @brief   This file contains the KETCube radio test module
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 University of West Bohemia in Pilsen
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
#include "ketCube_testRadio.h"
#include "ketCube_common.h"
#include "ketCube_terminal.h"
#include "ketCube_modules.h"

#include "ketCube_i2c.h"
#include "ketCube_adc.h"
#include "ketCube_hdcX080.h"
#include "ketCube_rxDisplay.h"

#include "ketCube_radio.h"
#include "ketCube_spi.h"
#include "ketCube_ad.h"

#ifdef KETCUBE_CFG_INC_MOD_TEST_RADIO

ketCube_testRadio_moduleCfg_t ketCube_testRadio_moduleCfg;       /*!< Module testRadio configuration storage */

/* private function prototypes */
static void ketCube_testRadio_OnRxError(void);
static void ketCube_testRadio_OnRxTimeout(void);
static void ketCube_testRadio_OnTxTimeout(void);
static void ketCube_testRadio_OnTxDone(void);
static void ketCube_testRadio_OnRxDone(uint8_t * payload, uint16_t size,
                                     int16_t rssi, int8_t snr);

/* extern function prototypes */
extern int16_t SX1276ReadTemperature(void);

/* private variables */
static RadioEvents_t RadioEvents;

/* State machine control */
static ketCube_testRadio_State_t state;
static ketCube_testRadio_State_t newState;

/* FSK parameters */
#define FSK_FDEV                                    25000       /*!<  Hz */
#define FSK_DATARATE                                50000       /*!<  bps */
#define FSK_BANDWIDTH                               50000       /*!<  Hz */
#define FSK_AFC_BANDWIDTH                           83333       /*!<  Hz */
#define FSK_PREAMBLE_LENGTH                         5           /*!<  Same for Tx and Rx */
#define FSK_FIX_LENGTH_PAYLOAD_ON                   FALSE
#define RX_TIMEOUT_VALUE                            1000
#define TX_TIMEOUT_VALUE                            500         /*!< Timeout should be small */

/**
 * @brief Prepare sleep mode
 *
 * @retval KETCUBE_CFG_MODULE_OK go sleep
 * @retval KETCUBE_CFG_MODULE_ERROR do not go sleep
 *
 * @todo testRadio module currently does not support sleep mode
 */
ketCube_cfg_ModError_t ketCube_testRadio_SleepEnter(void)
{
    /* plan state transition */
    switch (newState) {
        case KETCUBE_TEST_RADIO_STATE_SLEEP:
            break;
        case KETCUBE_TEST_RADIO_STATE_CW:
            ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_TEST_RADIO, "CW mode");
            break;
        case KETCUBE_TEST_RADIO_STATE_TX_TIMEOUT:
            newState = KETCUBE_TEST_RADIO_STATE_SLEEP;
            ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_TEST_RADIO, "TX timeout");
            break;
        case KETCUBE_TEST_RADIO_STATE_TX_DONE:
            newState = KETCUBE_TEST_RADIO_STATE_SLEEP;
            ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_TEST_RADIO, "TX done");
            break;
        case KETCUBE_TEST_RADIO_STATE_RX_TIMEOUT:
            newState = KETCUBE_TEST_RADIO_STATE_SLEEP;
            ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_TEST_RADIO, "RX timeout");
            break;
        case KETCUBE_TEST_RADIO_STATE_RX_DONE:
            newState = KETCUBE_TEST_RADIO_STATE_SLEEP;
            ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_TEST_RADIO, "RX done");
            break;
        case KETCUBE_TEST_RADIO_STATE_RX_ERROR:
            ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_TEST_RADIO, "RX error");
            break;
        default:
            newState = KETCUBE_TEST_RADIO_STATE_SLEEP;
            break;
    }
    
    /* perform state transition-related tasks */
    if (state != newState) {
        switch (newState) {
            case KETCUBE_TEST_RADIO_STATE_SLEEP:
                Radio.Sleep();
                break;
            case KETCUBE_TEST_RADIO_STATE_CW:
                if ((ketCube_testRadio_moduleCfg.cwFreq > KETCUBE_TEST_RADIO_MAX_FREQUENCY) || (ketCube_testRadio_moduleCfg.cwFreq < KETCUBE_TEST_RADIO_MIN_FREQUENCY)) {
                    ketCube_testRadio_moduleCfg.cwFreq = KETCUBE_TEST_RADIO_DEFAULT_FREQUENCY;
                }
                if ((ketCube_testRadio_moduleCfg.cwPwr > KETCUBE_TEST_RADIO_MAX_OUTPUT_POWER) || (ketCube_testRadio_moduleCfg.cwPwr < KETCUBE_TEST_RADIO_MIN_OUTPUT_POWER)) {
                    ketCube_testRadio_moduleCfg.cwPwr = KETCUBE_TEST_RADIO_DEFAULT_OUTPUT_POWER;
                }
                if (ketCube_testRadio_moduleCfg.cwDur == 0) {
                    ketCube_testRadio_moduleCfg.cwDur = KETCUBE_TEST_RADIO_DEFAULT_CW_DUR;
                }
                
                /* start CW */
                ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_TEST_RADIO, "Start CW: %d Hz; %d dBm; %d s", ketCube_testRadio_moduleCfg.cwFreq, ketCube_testRadio_moduleCfg.cwPwr, ketCube_testRadio_moduleCfg.cwDur);
                Radio.SetTxContinuousWave(ketCube_testRadio_moduleCfg.cwFreq, ketCube_testRadio_moduleCfg.cwPwr, ketCube_testRadio_moduleCfg.cwDur);
                ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_TEST_RADIO, "Temperature: %d °C", SX1276ReadTemperature());
                break;
            case KETCUBE_TEST_RADIO_STATE_TX_TIMEOUT:
            case KETCUBE_TEST_RADIO_STATE_TX_DONE:
            case KETCUBE_TEST_RADIO_STATE_RX_TIMEOUT:
            case KETCUBE_TEST_RADIO_STATE_RX_DONE:
            default:
                break;
        }
    }
    
    /* perform state transition */
    state = newState;
    
    return KETCUBE_CFG_MODULE_OK;
}


/**
  * @brief Initialize testRadio
  * 
  * @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_testRadio_Init(ketCube_InterModMsg_t *** msg)
{
    /* Initialize HW */
    ketCube_SPI_Init();
    ketCube_Radio_Init();
    ketCube_AD_Init();
    
    // Radio initialization
    RadioEvents.CadDone = NULL;
    RadioEvents.RxDone = ketCube_testRadio_OnRxDone;
    RadioEvents.RxTimeout = ketCube_testRadio_OnRxTimeout;
    RadioEvents.RxError = ketCube_testRadio_OnRxError;
    RadioEvents.TxDone = ketCube_testRadio_OnTxDone;
    RadioEvents.TxTimeout = ketCube_testRadio_OnTxTimeout;

    Radio.Init(&RadioEvents);
    Radio.SetChannel(KETCUBE_TEST_RADIO_DEFAULT_FREQUENCY);
    
    Radio.SetRxConfig(MODEM_FSK, FSK_BANDWIDTH,
                      FSK_DATARATE, 0, 
                      FSK_AFC_BANDWIDTH, FSK_PREAMBLE_LENGTH,
                      0, FSK_FIX_LENGTH_PAYLOAD_ON, 0, TRUE,
                      0, 0, FALSE, TRUE);

    Radio.SetTxConfig(MODEM_FSK, KETCUBE_TEST_RADIO_MIN_OUTPUT_POWER,
                      FSK_FDEV, 0, FSK_DATARATE, 0,
                      FSK_PREAMBLE_LENGTH, FSK_FIX_LENGTH_PAYLOAD_ON,
                      TRUE, 0, 0, 0, TX_TIMEOUT_VALUE);
    
    /* Init state */
    state = KETCUBE_TEST_RADIO_STATE_SLEEP;
    newState = state;
    Radio.Sleep();
    
    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Forces radio to sleep mode
 */
void ketCube_testRadio_cmd_GoSleep(void)
{
    newState = KETCUBE_TEST_RADIO_STATE_SLEEP;
}

/**
 * @brief Forces radio to CW mode
 */
void ketCube_testRadio_cmd_GoCW(void)
{
    newState = KETCUBE_TEST_RADIO_STATE_CW;
}

/**
 * @brief Get temperature of the radio chip
 */
void ketCube_testRadio_cmd_getTemp(void)
{
    commandIOParams.as_int32 = (int32_t) SX1276ReadTemperature();
}

/**
 * @brief This function is executed on Tx Done event
 */
static void ketCube_testRadio_OnTxDone(void)
{
    newState = KETCUBE_TEST_RADIO_STATE_TX_DONE;
}

/**
 * @brief This function is executed on Rx Done event
 */
static void ketCube_testRadio_OnRxDone(uint8_t * payload, uint16_t size,
                                       int16_t rssi, int8_t snr)
{
    newState = KETCUBE_TEST_RADIO_STATE_RX_DONE;
}

/**
 * @brief This function is executed on radio Tx Timeout event
 */
static void ketCube_testRadio_OnTxTimeout(void)
{
    newState = KETCUBE_TEST_RADIO_STATE_TX_TIMEOUT;
}

/**
 * @brief This function is executed on radio Rx Timeout event
 */
static void ketCube_testRadio_OnRxTimeout(void)
{
    newState = KETCUBE_TEST_RADIO_STATE_RX_TIMEOUT;
}

/**
 * @brief This function is executed on radio Rx Error event
 */
static void ketCube_testRadio_OnRxError(void)
{
    newState = KETCUBE_TEST_RADIO_STATE_RX_ERROR;
}


#endif                          /* KETCUBE_CFG_INC_MOD_TEST_RADIO */
