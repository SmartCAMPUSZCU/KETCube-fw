/**
 * @file    ketCube_hdc1080.h
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2020-05-28
 * @brief   This file contains definitions for the HDCx080 driver
 *
 * HDC1080 Datasheet:
 * www.ti.com/lit/ds/symlink/hdc1080.pdf
 * 
 * HDC2080 Datasheet:
 * www.ti.com/lit/ds/symlink/hdc2080.pdf
 * 
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2018 - 2020 University of West Bohemia in Pilsen
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
#ifndef __KETCUBE_HDCX080_H
#define __KETCUBE_HDCX080_H

#include "ketCube_cfg.h"
#include "ketCube_common.h"

/** @defgroup KETCube_HDCx080 KETCube HDCx080
  * @brief KETCube HDCx080 module
  * @ingroup KETCube_SensMods
  * @{
  */

/** @defgroup KETCube_HDCx080_defs Public Defines
  * @brief Public defines
  * @{
  */

/**
* @brief  List of sensors supported by this driver
*/
typedef enum {
    KETCUBE_HDCX080_RESULT_OK = 0x00,                  /*!< No error */
    
    KETCUBE_HDCX080_RESULT_ERROR_SENSDET = 0x01,       /*!< sensor detection failed */
    
    KETCUBE_HDCX080_RESULT_ERROR_I2CWRITE = 0x11,      /*!< generic I2C write error */
    KETCUBE_HDCX080_RESULT_ERROR_I2CWRITEADDR = 0x12,  /*!< I2C write address error */
    
    KETCUBE_HDCX080_RESULT_ERROR_I2CREAD = 0x21,       /*!< generic I2C read error */
} ketCube_hdcX080_result_t;

/**
* @brief  List of sensors supported by this driver
*/
typedef enum {
    KETCUBE_HDCX080_TYPE_AUTODETECT = 0, /*!< Sensor type autodetection */
    KETCUBE_HDCX080_TYPE_HDC1080    = 1, /*!< HDC1080 */
    KETCUBE_HDCX080_TYPE_HDC2080    = 2, /*!< HDC2080 */
} ketCube_hdcX080_sensType_t;

/**
* @brief  KETCube module configuration
* 
* @note anonymous union is used; the Keil C compiler requires pragma
*/
#ifdef __ARMCC_VERSION
#pragma anon_unions
#endif
typedef struct ketCube_hdcX080_moduleCfg_t {
    union {
        struct {
            ketCube_cfg_ModuleCfgByte_t coreCfg;           /*!< KETCube core cfg byte */
            ketCube_hdcX080_sensType_t sensType;           /*!< Used sensor type */
            ketCube_hdcX080_result_t errno;                /*!< Error indication; meaningful is only the RAM value */
        };
        uint8_t raw[8];                                    /*!< 8 bytes are reserved for HDCX080 configuration for future configuration extension */
    };
} ketCube_hdcX080_moduleCfg_t;

extern ketCube_hdcX080_moduleCfg_t ketCube_hdcX080_moduleCfg;

/* ============================================ */

/**
* @brief  I2C Timeout.
*/
#define KETCUBE_HDCX080_I2C_TIMEOUT 0x1000      /*!< Value of Timeout when I2C communication fails */
#define KETCUBE_HDCX080_I2C_TRY          3      /*!< # repeats if error */


/* ============================================ */

/**
* @brief  Heater On/Off
*/
typedef enum {
    KETCUBE_HDCX080_HTR_ON = (uint8_t) 0x01,    /*!< HTR ON */
    KETCUBE_HDCX080_HTR_OFF = (uint8_t) 0x00    /*!< HTR OFF */
} ketCube_hdcX080_Htr_t;

/**
* @brief Reset
*/
typedef enum {
    KETCUBE_HDCX080_RST_RESET = (uint8_t) 0x01, /*!< Invoke SW reset */
    KETCUBE_HDCX080_RST_NONE = (uint8_t) 0x00   /*!< Normal operation */
} ketCube_hdcX080_Rst_t;

/**
* @brief  RH resolution configuration.
*/
typedef enum {
    KETCUBE_HDCX080_HRES_8BIT = (uint8_t) 0x02,         /*!<  8 bit resolution for HDC1080, 9 bits for HDC2080 */
    KETCUBE_HDCX080_HRES_11BIT = (uint8_t) 0x01,        /*!< 11 bit resolution */
    KETCUBE_HDCX080_HRES_14BIT = (uint8_t) 0x00         /*!< 14 bit resolution */
} ketCube_hdcX080_HRes_t;

/* ============================================ */


/** @defgroup KETCube_HDC1080_defs Public Defines
  * @brief Public defines for HDC1080 only
  * @{
  */


/**
* @brief  HDC1080 I2C address.
*/
#define KETCUBE_HDC1080_I2C_ADDRESS  (uint8_t) (0x40 << 1)

/**
* @brief HDC1080 register File
*/
typedef enum {
    KETCUBE_HDC1080_TEMPERATURE_REG      = 0x00,
    KETCUBE_HDC1080_HUMIDITY_REG         = 0x01,
    KETCUBE_HDC1080_CONFIGURATION_REG    = 0x02,
    KETCUBE_HDC1080_MANUFACTURER_ID_REG  = 0xFE,
    KETCUBE_HDC1080_DEVICE_ID_REG        = 0xFF,
    KETCUBE_HDC1080_SERIAL_ID_FIRST_REG  = 0xFB,
    KETCUBE_HDC1080_SERIAL_ID_MID_REG    = 0xFC,
    KETCUBE_HDC1080_SERIAL_ID_LAST_REG   = 0xFD
} ketCube_hdc1080_RegMap_t;
    
/**
* @brief  Temperature resolution configuration.
*/
typedef enum {
    KETCUBE_HDC1080_TRES_11BIT = (uint8_t) 0x01,      /*!< 11 bit resolution */
    KETCUBE_HDC1080_TRES_14BIT = (uint8_t) 0x00       /*!< 14 bit resolution */
} ketCube_hdc1080_TRes_t;

/**
* @brief AQ mode
*/
typedef enum {
    KETCUBE_HDC1080_AQ_SEPARATE = (uint8_t) 0x00,       /*!< Get temperature/Humidity separately */
    KETCUBE_HDC1080_AQ_SEQ = (uint8_t) 0x01     /*!< get Temperature/Humidity in sequence; temperature first */
} ketCube_hdc1080_Aq_t;

/**
* @brief  Battery voltage
*/
typedef enum {
    KETCUBE_HDC1080_BAT_HIGH = (uint8_t) 0x00,  /*!< Bat above 2.8 V */
    KETCUBE_HDC1080_BAT_LOW = (uint8_t) 0x01    /*!< Bat below 2.8 V */
} ketCube_hdc1080_Bat_t;

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
* @brief  HDC1080 Init structure definition.
*/
#ifdef __ARMCC_VERSION
#pragma anon_unions
#endif
typedef struct {
    uint8_t rawData;
    struct {
        ketCube_hdcX080_HRes_t HumidityMeasurementResolution:2;
        ketCube_hdc1080_TRes_t TemperatureMeasurementResolution:1;
        ketCube_hdc1080_Bat_t BatteryStatus:1;  //< read-only field
        ketCube_hdc1080_Aq_t ModeOfAcquisition:1;
        ketCube_hdcX080_Htr_t Heater:1;
        uint8_t RFU:1;
        ketCube_hdcX080_Rst_t SoftwareReset:1;
    };
} ketCube_hdc1080_Init_t;

/**
* @}
*/

/* ============================================ */

/** @defgroup KETCube_HDC2080_defs Public Defines
  * @brief Public defines for HDC2080 only
  * @{
  */

/**
* @brief  Default HDC2080 I2C address.
*/
#define KETCUBE_HDC2080_I2C_ADDRESS  (uint8_t) (0x40 << 1)


/**
* @brief HDC2080 Register File
*/
typedef enum {
    KETCUBE_HDC2080_TEMPERATURE_REG_L    = 0x00,
    KETCUBE_HDC2080_TEMPERATURE_REG_H    = 0x01,
    KETCUBE_HDC2080_HUMIDITY_REG_L       = 0x02,
    KETCUBE_HDC2080_HUMIDITY_REG_H       = 0x03,
    
    /* Registers UNUSED in this module */
    
    KETCUBE_HDC2080_CFG_REG              = 0x0E,
    KETCUBE_HDC2080_MEASCFG_REG          = 0x0F,
    
    /* Registers UNUSED in this module */
    
    KETCUBE_HDC2080_MANUFACTURER_ID_REG_L  = 0xFC,
    KETCUBE_HDC2080_MANUFACTURER_ID_REG_H  = 0xFD,
    KETCUBE_HDC2080_DEVICE_ID_REG_L        = 0xFE,
    KETCUBE_HDC2080_DEVICE_ID_REG_H        = 0xFF,
} ketCube_hdc2080_RegMap_t;

/**
* @brief  Temperature resolution configuration.
*/
typedef enum {
    KETCUBE_HDC2080_TRES_9BIT = (uint8_t) 0x02,       /*!<  9 bits for HDC2080 */
    KETCUBE_HDC2080_TRES_11BIT = (uint8_t) 0x01,      /*!< 11 bit resolution */
    KETCUBE_HDC2080_TRES_14BIT = (uint8_t) 0x00       /*!< 14 bit resolution */
} ketCube_hdc2080_TRes_t;

/**
* @brief  HDC2080 Auto Measurement Mode (AMM)
* 
*/
typedef enum {
    KETCUBE_HDC2080_AMM_DIS   = 0,    //< disabled
    KETCUBE_HDC2080_AMM_0_008 = 1,    //< 1/120 Hz
    KETCUBE_HDC2080_AMM_0_016 = 2,    //< 1/60 Hz
    KETCUBE_HDC2080_AMM_0_1   = 3,    //< 0.1 Hz
    KETCUBE_HDC2080_AMM_0_2   = 4,    //< 0.2 Hz
    KETCUBE_HDC2080_AMM_1_0   = 5,    //< 1 Hz
    KETCUBE_HDC2080_AMM_2_0   = 6,    //< 2 Hz
    KETCUBE_HDC2080_AMM_5_0   = 7,    //< 5 Hz
} ketCube_hdc2080_AMM_t;

/**
* @brief DRDY/INT_EN mode
*/
typedef enum {
    KETCUBE_HDC2080_INTEN_HZ = (uint8_t) 0x0,    /*!< High Z */
    KETCUBE_HDC2080_INTEN_EN = (uint8_t) 0x1     /*!< Enabled */
} ketCube_hdc2080_IntEn_t;

/**
* @brief Interrupt polarity
*/
typedef enum {
    KETCUBE_HDC2080_INTPOL_LOW  = (uint8_t) 0x0,    /*!< Active Low */
    KETCUBE_HDC2080_INTPOL_HIGH = (uint8_t) 0x1     /*!< Active High */
} ketCube_hdc2080_IntPol_t;

/**
* @brief Interrupt mode
*/
typedef enum {
    KETCUBE_HDC2080_INTPOL_LS  = (uint8_t) 0x0,    /*!< Level Sensitive */
    KETCUBE_HDC2080_INTPOL_CMP = (uint8_t) 0x1     /*!< Comparator mode */
} ketCube_hdc2080_IntMode_t;

/**
* @brief Measurement Configuration
*/
typedef enum {
    KETCUBE_HDC2080_MEASCFG_RHT  = (uint8_t) 0x0,    /*!< RH + T Measurement */
    KETCUBE_HDC2080_MEASCFG_T    = (uint8_t) 0x1     /*!< T-only Measurement */
} ketCube_hdc2080_MeasCfg_t;

/**
* @brief Measurement Trigger
*/
typedef enum {
    KETCUBE_HDC2080_MEASTRIG_NONE     = (uint8_t) 0x0,    /*!< No Action */
    KETCUBE_HDC2080_MEASTRIG_START    = (uint8_t) 0x1     /*!< Start Measurement */
} ketCube_hdc2080_MeasTrig_t;

/**
* @brief  HDC2080 Init structure definition.
*/
#ifdef __ARMCC_VERSION
#pragma anon_unions
#endif
typedef struct {
    struct {
        ketCube_hdc2080_IntMode_t IntMode:1;
        ketCube_hdc2080_IntPol_t IntPol:1;
        ketCube_hdc2080_IntEn_t IntEn:1;
        ketCube_hdcX080_Htr_t Heater:1;
        ketCube_hdc2080_AMM_t AutoMeasMode:3;
        ketCube_hdcX080_Rst_t SoftwareReset:1;
    };
    struct {
        ketCube_hdc2080_MeasTrig_t MeasTrig:1;
        ketCube_hdc2080_MeasCfg_t MeasCfg:2;
        uint8_t RFU:1;
        ketCube_hdcX080_HRes_t HumidityMeasurementResolution:2;
        ketCube_hdc2080_TRes_t TemperatureMeasurementResolution:2;
    };
} ketCube_hdc2080_Init_t;

/**
* @}
*/


/**
* @}
*/



/** @defgroup KETCube_HDCX080_fn Public Functions
  * @brief Public functions
  * @{
  */

extern ketCube_cfg_ModError_t ketCube_hdcX080_Init(ketCube_InterModMsg_t
                                                   *** msg);
extern ketCube_cfg_ModError_t ketCube_hdcX080_UnInit(void);
extern ketCube_cfg_ModError_t ketCube_hdcX080_ReadData(uint8_t * buffer,
                                                       uint8_t * len);

/**
* @}
*/

/**
* @}
*/

#endif                          /* __KETCUBE_HDCX080_H */
