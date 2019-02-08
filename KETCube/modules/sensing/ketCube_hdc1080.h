/**
 * @file    ketCube_hdc1080.h
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-01-04
 * @brief   This file contains definitions for the HDC1080 driver
 *
 * HDC1080 Datasheet:
 * www.ti.com/lit/ds/symlink/hdc1080.pdf
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KETCUBE_HDC1080_H
#define __KETCUBE_HDC1080_H

#include "ketCube_cfg.h"
#include "ketCube_common.h"

/** @defgroup KETCube_HDC1080 KETCube HDC1080
  * @brief KETCube HDC1080 module
  * @ingroup KETCube_SensMods
  * @{
  */

/** @defgroup KETCube_HDC1080_defs Public Defines
  * @brief Public defines
  * @{
  */

/**
* @brief  RH resolution configuration.
*/
typedef enum {
    KETCUBE_HDC1080_HRES_8BIT = (uint8_t) 0x02, /*!<  8 bit resolution */
    KETCUBE_HDC1080_HRES_11BIT = (uint8_t) 0x01,        /*!< 11 bit resolution */
    KETCUBE_HDC1080_HRES_14BIT = (uint8_t) 0x00 /*!< 14 bit resolution */
} ketCube_hdc1080_HRes_t;

/**
* @brief  temperature resolution configuration.
*/
typedef enum {
    KETCUBE_HDC1080_TRES_11BIT = (uint8_t) 0x01,        /*!< 11 bit resolution */
    KETCUBE_HDC1080_TRES_14BIT = (uint8_t) 0x00 /*!< 14 bit resolution */
} ketCube_hdc1080_TRes_t;

/**
* @brief  Battery voltage
*/
typedef enum {
    KETCUBE_HDC1080_BAT_HIGH = (uint8_t) 0x00,  /*!< Bat above 2.8 V */
    KETCUBE_HDC1080_BAT_LOW = (uint8_t) 0x01    /*!< Bat below 2.8 V */
} ketCube_hdc1080_Bat_t;

/**
* @brief  Heater On/Off
*/
typedef enum {
    KETCUBE_HDC1080_HTR_ON = (uint8_t) 0x01,    /*!< HTR ON */
    KETCUBE_HDC1080_HTR_OFF = (uint8_t) 0x00    /*!< HTR OFF */
} ketCube_hdc1080_Htr_t;

/**
* @brief Reset
*/
typedef enum {
    KETCUBE_HDC1080_RST_RESET = (uint8_t) 0x01, /*!< Invoke SW reset */
    KETCUBE_HDC1080_RST_NONE = (uint8_t) 0x00   /*!< Normal operation */
} ketCube_hdc1080_Rst_t;

/**
* @brief AQ mode
*/
typedef enum {
    KETCUBE_HDC1080_AQ_SEPARATE = (uint8_t) 0x00,       /*!< Get temperature/Humidity separately */
    KETCUBE_HDC1080_AQ_SEQ = (uint8_t) 0x01     /*!< get Temperature/Humidity in sequence; temperature first */
} ketCube_hdc1080_Aq_t;

/**
* @brief  Push-pull/Open Drain selection on DRDY pin.
*/
typedef enum {
    KETCUBE_HDC1080_PUSHPULL = (uint8_t) 0x00,  /*!< DRDY pin in push pull */
    KETCUBE_HDC1080_OPENDRAIN = (uint8_t) 0x40  /*!< DRDY pin in open drain */
} ketCube_hdc1080_OutputType_t;

#define IS_KETCUBE_HDC1080_OutputType(MODE) ((MODE == KETCUBE_HDC1080_PUSHPULL) || (MODE == KETCUBE_HDC1080_OPENDRAIN))

/**
* @brief  Active level of DRDY pin.
*/
typedef enum {
    KETCUBE_HDC1080_HIGH_LVL = (uint8_t) 0x00,  /*!< HIGH state level for DRDY pin */
    KETCUBE_HDC1080_LOW_LVL = (uint8_t) 0x80    /*!< LOW state level for DRDY pin */
} ketCube_hdc1080_DrdyLevel_t;
#define IS_KETCUBE_HDC1080_DrdyLevelType(MODE) ((MODE == KETCUBE_HDC1080_HIGH_LVL) || (MODE == KETCUBE_HDC1080_LOW_LVL))

/**
* @brief  KETCube module configuration
*/
typedef struct ketCube_hdc1080_moduleCfg_t {
    ketCube_cfg_ModuleCfgByte_t coreCfg;           /*<! KETCube core cfg byte */
} ketCube_hdc1080_moduleCfg_t;

extern ketCube_hdc1080_moduleCfg_t ketCube_hdc1080_moduleCfg;

/**
* @brief  HDC1080 Init structure definition.
*/
#ifdef __ARMCC_VERSION
#pragma anon_unions
#endif
typedef struct {
    uint8_t rawData;
    struct {
        ketCube_hdc1080_HRes_t HumidityMeasurementResolution:2;
        ketCube_hdc1080_TRes_t TemperatureMeasurementResolution:1;
        ketCube_hdc1080_Bat_t BatteryStatus:1;  //< read-only field
        ketCube_hdc1080_Aq_t ModeOfAcquisition:1;
        ketCube_hdc1080_Htr_t Heater:1;
        uint8_t ReservedAgain:1;
        ketCube_hdc1080_Rst_t SoftwareReset:1;
    };
} ketCube_hdc1080_Init_t;

/**
* @brief  I2C address.
*/
#define KETCUBE_HDC1080_I2C_ADDRESS  (uint8_t) (0x40 << 1)

/**
* @brief  I2C Timeout.
*/
#define KETCUBE_HDC1080_I2C_TIMEOUT 0x1000      /*<! Value of Timeout when I2C communication fails */

/**
* @addtogroup KETCUBE_HDC1080_Registers HDC1080 Registers
* @{
*/

#define KETCUBE_HDC1080_TEMPERATURE_REG		  0x00
#define KETCUBE_HDC1080_HUMIDITY_REG		    0x01
#define KETCUBE_HDC1080_CONFIGURATION_REG	  0x02
#define KETCUBE_HDC1080_MANUFACTURER_ID_REG 0xFE
#define KETCUBE_HDC1080_DEVICE_ID_REG		    0xFF
#define KETCUBE_HDC1080_SERIAL_ID_FIRST_REG	0xFB
#define KETCUBE_HDC1080_SERIAL_ID_MID_REG	  0xFC
#define KETCUBE_HDC1080_SERIAL_ID_LAST_REG	0xFD

/**
* @}
*/


/**
* @}
*/


/** @defgroup KETCube_HDC1080_fn Public Functions
  * @brief Public functions
  * @{
  */

extern ketCube_cfg_ModError_t ketCube_hdc1080_Init(ketCube_InterModMsg_t
                                                   *** msg);
extern ketCube_cfg_ModError_t ketCube_hdc1080_UnInit(void);
extern ketCube_cfg_ModError_t ketCube_hdc1080_ReadData(uint8_t * buffer,
                                                       uint8_t * len);

/**
* @}
*/

/**
* @}
*/

#endif                          /* __KETCUBE_HDC1080_H */
