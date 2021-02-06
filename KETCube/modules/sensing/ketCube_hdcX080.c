/**
 * @file    ketCube_hdcx080.c
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2020-05-28
 * @brief   This file contains the HDCx080 driver
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

#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_i2c.h"
#include <math.h>

#include "ketCube_cfg.h"
#include "ketCube_terminal.h"
#include "ketCube_i2c.h"
#include "ketCube_hdcX080.h"

#ifdef KETCUBE_CFG_INC_MOD_HDCX080

ketCube_hdcX080_moduleCfg_t ketCube_hdcX080_moduleCfg; /*!< Module configuration storage */

/* Private function prototypes */
bool getHumidity(uint16_t * value);
bool getTemperature(int16_t * value);

/**
 * @brief  Write TexasInstruments I2C periph 16-bit register
 * @param  devAddr I2C Address
 * @param  regAddr register address
 * @param  data pointer to 16-bit value
 * 
 * @retval retval TRUE in case of success, else return FALSE
 */
static bool ketCube_I2C_HDC1080WriteReg(uint8_t devAddr,
                                        uint8_t regAddr,
                                        uint16_t * data)
{
    uint8_t buffer[2];
    buffer[0] = (uint8_t) ((*data >> 8) & 0x00FF);
    buffer[1] = (uint8_t) (*data & 0x00FF);

    /* no error by default */
    ketCube_hdcX080_moduleCfg.errno = KETCUBE_HDCX080_RESULT_OK;
    if (ketCube_I2C_WriteData(devAddr, regAddr, &(buffer[0]), 2) == KETCUBE_CFG_DRV_OK) {
        return TRUE;
    } else {
        ketCube_hdcX080_moduleCfg.errno = KETCUBE_HDCX080_RESULT_ERROR_I2CWRITE;
        return FALSE;
    }
}

/**
 * @brief  Read TexasInstruments I2C periph 16-bit register
 * @param  devAddr I2C Address
 * @param  regAddr register address
 * @param  data pointer to 16-bit value
 * 
 * @retval retval TRUE in case of success, else return FALSE
 */
static bool ketCube_I2C_HDC1080ReadReg(uint8_t devAddr,
                                       uint8_t regAddr,
                                       uint16_t * data)
{
    uint8_t buffer[2];
    uint8_t try;
    
    for (try = 0; try < KETCUBE_HDCX080_I2C_TRY; try++) {
        /* Write pointer address */
        if (ketCube_I2C_WriteRawData(devAddr, &regAddr, 1) == KETCUBE_CFG_DRV_ERROR) {
            ketCube_hdcX080_moduleCfg.errno = KETCUBE_HDCX080_RESULT_ERROR_I2CWRITEADDR;
            continue;
        }
        
        /* Wait conversion time */ 
        if ((regAddr == KETCUBE_HDC1080_HUMIDITY_REG) || (regAddr == KETCUBE_HDC1080_TEMPERATURE_REG)) {
            HAL_Delay(10);
        }
        
        /* Read data */
        if (ketCube_I2C_ReadRawData(devAddr, &(buffer[0]), 2) == KETCUBE_CFG_DRV_ERROR) {
            ketCube_hdcX080_moduleCfg.errno = KETCUBE_HDCX080_RESULT_ERROR_I2CREAD;
            continue;
        }
        
        /* do not repeat in cas of we are here */
        break;
    }
    
    if (try < KETCUBE_HDCX080_I2C_TRY) {
        *data = (((uint16_t) buffer[0]) << 8) | buffer[1];
        ketCube_hdcX080_moduleCfg.errno = KETCUBE_HDCX080_RESULT_OK;
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @brief  Write HDC2080 register
 * @param  devAddr I2C Address
 * @param  regAddr register address
 * @param  data 8-bit value
 * 
 * @retval retval TRUE in case of success, else return FALSE
 */
static bool ketCube_I2C_HDC2080WriteReg(uint8_t devAddr,
                                        uint8_t regAddr,
                                        uint8_t data)
{    
    /* no error by default */
    ketCube_hdcX080_moduleCfg.errno = KETCUBE_HDCX080_RESULT_OK;
    if (ketCube_I2C_WriteData(devAddr, regAddr, &(data), 1) == KETCUBE_CFG_DRV_OK) {
        return TRUE;
    } else {
        ketCube_hdcX080_moduleCfg.errno = KETCUBE_HDCX080_RESULT_ERROR_I2CWRITE;
        return FALSE;
    }
}

/**
 * @brief  Read HDC2080 register
 * @param  devAddr I2C Address
 * @param  regAddr register address
 * @param  data 8-bit variable pointer
 * 
 * @retval retval TRUE in case of success, else return FALSE
 */
static bool ketCube_I2C_HDC2080ReadReg(uint8_t devAddr,
                                       uint8_t regAddr,
                                       uint8_t * data)
{
    /* Write pointer address */
    if (ketCube_I2C_WriteRawData(devAddr, &regAddr, 1) == KETCUBE_CFG_DRV_ERROR) {
        ketCube_hdcX080_moduleCfg.errno = KETCUBE_HDCX080_RESULT_ERROR_I2CWRITEADDR;
        return FALSE;
    }
    
    /* Read data */
    if (ketCube_I2C_ReadRawData(devAddr, data, 1) == KETCUBE_CFG_DRV_ERROR) {
        ketCube_hdcX080_moduleCfg.errno = KETCUBE_HDCX080_RESULT_ERROR_I2CREAD;
        return FALSE;
    }
       
    ketCube_hdcX080_moduleCfg.errno = KETCUBE_HDCX080_RESULT_OK;
    return TRUE;
    
}


/**
 * @brief Initialize the HDC1080 sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_hdc1080_Init(void)
{
    ketCube_hdc1080_Init_t pxInit;
    
    pxInit.TemperatureMeasurementResolution = KETCUBE_HDC1080_TRES_14BIT;
    pxInit.HumidityMeasurementResolution = KETCUBE_HDCX080_HRES_14BIT;
    pxInit.ModeOfAcquisition = KETCUBE_HDC1080_AQ_SEPARATE;

    if (ketCube_I2C_HDC1080WriteReg
        (KETCUBE_HDC1080_I2C_ADDRESS, KETCUBE_HDC1080_CONFIGURATION_REG,
         (uint16_t *) & pxInit) == FALSE) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_HDCX080,
                                      "HDC1080 initialization failed (errno = %d)!", ketCube_hdcX080_moduleCfg.errno);
        return KETCUBE_CFG_MODULE_ERROR;
    }

    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Initialize the HDC2080 sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_hdc2080_Init(void)
{
    ketCube_hdc2080_Init_t pxInit;
    
    pxInit.TemperatureMeasurementResolution = KETCUBE_HDC2080_TRES_14BIT;
    pxInit.HumidityMeasurementResolution = KETCUBE_HDCX080_HRES_14BIT;
    pxInit.MeasCfg = KETCUBE_HDC2080_MEASCFG_RHT;
    pxInit.MeasTrig = KETCUBE_HDC2080_MEASTRIG_START;
    
    pxInit.SoftwareReset = KETCUBE_HDCX080_RST_NONE;
    pxInit.AutoMeasMode = KETCUBE_HDC2080_AMM_DIS;
    pxInit.Heater = KETCUBE_HDCX080_HTR_OFF;
    pxInit.IntEn = KETCUBE_HDC2080_INTEN_HZ;
    
    if (ketCube_I2C_HDC2080WriteReg
        (KETCUBE_HDC2080_I2C_ADDRESS, KETCUBE_HDC2080_CFG_REG,
         ((uint8_t *) &pxInit)[0] ) == FALSE) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_HDCX080,
                                      "HDC2080 initialization failed! (errno = %d)!", ketCube_hdcX080_moduleCfg.errno);
        return KETCUBE_CFG_MODULE_ERROR;
    }
    
    if (ketCube_I2C_HDC2080WriteReg
        (KETCUBE_HDC2080_I2C_ADDRESS, KETCUBE_HDC2080_MEASCFG_REG,
         ((uint8_t *) &pxInit)[1] ) == FALSE) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_HDCX080,
                                      "HDC2080 initialization failed! (errno = %d)!", ketCube_hdcX080_moduleCfg.errno);
        return KETCUBE_CFG_MODULE_ERROR;
    }
    
    
    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Initialize the HDCX080 sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_hdcX080_Init(ketCube_InterModMsg_t *** msg)
{

    // Init drivers
    if (ketCube_I2C_Init() != KETCUBE_CFG_DRV_OK) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_HDCX080,
                                      "I2C initialization failed!");
        return KETCUBE_CFG_MODULE_ERROR;
    }

    switch (ketCube_hdcX080_moduleCfg.sensType) {
        case KETCUBE_HDCX080_TYPE_AUTODETECT:
            if (ketCube_hdc1080_Init() == KETCUBE_CFG_MODULE_OK) {
                ketCube_hdcX080_moduleCfg.sensType = KETCUBE_HDCX080_TYPE_HDC1080;
                ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_HDCX080, "HDC1080 detected");
                return KETCUBE_CFG_MODULE_OK;
            } else {
                if (ketCube_hdc2080_Init() == KETCUBE_CFG_MODULE_OK) {
                    ketCube_hdcX080_moduleCfg.sensType = KETCUBE_HDCX080_TYPE_HDC2080;
                    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_HDCX080, "HDC2080 detected");
                    return KETCUBE_CFG_MODULE_OK;   
                }
            }
            break;
        case KETCUBE_HDCX080_TYPE_HDC1080:
            return ketCube_hdc1080_Init();
            break;
        case KETCUBE_HDCX080_TYPE_HDC2080:
            return ketCube_hdc2080_Init();
            break;
        default:
            break;
    }
    
    return KETCUBE_CFG_MODULE_ERROR;
}

/**
  * @brief Initialize the HDC1080 sensor
  *
  * @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_hdcX080_UnInit(void)
{
    // UnInit drivers
    return ketCube_I2C_UnInit();
}

/**
  * @brief  Read HDC1080 Humidity output registers, and calculate humidity.
  * @param  value pointer to the returned humidity value that must be divided by 10 to get the value in [%].
  * 
  * @retval retval TRUE in case of success, else return FALSE
  */
bool getHumidity(uint16_t * value)
{
    uint16_t rawH;
    
    switch (ketCube_hdcX080_moduleCfg.sensType) {
        case KETCUBE_HDCX080_TYPE_HDC1080:
            if (ketCube_I2C_HDC1080ReadReg
                (KETCUBE_HDC1080_I2C_ADDRESS, KETCUBE_HDC1080_HUMIDITY_REG,
                 &rawH) == FALSE) {
                ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_HDCX080,
                                              "Read humidity failed (errno = %d)!", ketCube_hdcX080_moduleCfg.errno);
                return FALSE;
            }
            break;
        case KETCUBE_HDCX080_TYPE_HDC2080:
            /* Read LSB first! */
            if (ketCube_I2C_HDC2080ReadReg
                (KETCUBE_HDC2080_I2C_ADDRESS, KETCUBE_HDC2080_HUMIDITY_REG_L,
                 &(((uint8_t *) &rawH)[0])) == FALSE) {
                ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_HDCX080,
                                              "Read humidity failed (errno = %d)!", ketCube_hdcX080_moduleCfg.errno);
                return FALSE;
            }
            if (ketCube_I2C_HDC2080ReadReg
                (KETCUBE_HDC2080_I2C_ADDRESS, KETCUBE_HDC2080_HUMIDITY_REG_H,
                 &(((uint8_t *) &rawH)[1])) == FALSE) {
                ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_HDCX080,
                                              "Read humidity failed (errno = %d)!", ketCube_hdcX080_moduleCfg.errno);
                return FALSE;
            }
            break;
        default:
            return FALSE;
    }    
    
    *value = (uint16_t) (((float) (((float) rawH) / pow(2, 16))) * 1000.0);

    return TRUE;
}

/**
  * @brief  Read HDC1080 temperature output registers, and calculate temperature.
  * @param  value a pointer to the returned temperature value that must be divided by 10 to get the value in ['C].
  * 
  * @retval retval TRUE in case of success, else return FALSE
  */
bool getTemperature(int16_t * value)
{
    uint16_t rawT;
    
    switch (ketCube_hdcX080_moduleCfg.sensType) {
        case KETCUBE_HDCX080_TYPE_HDC1080:
            if (ketCube_I2C_HDC1080ReadReg
                (KETCUBE_HDC1080_I2C_ADDRESS, KETCUBE_HDC1080_TEMPERATURE_REG,
                 &rawT) == FALSE) {
                ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_HDCX080,
                                              "Read temperature failed (errno = %d)!", ketCube_hdcX080_moduleCfg.errno);
                return FALSE;
            }
            break;
        case KETCUBE_HDCX080_TYPE_HDC2080:
            /* Read LSB first! */
            if (ketCube_I2C_HDC2080ReadReg
                (KETCUBE_HDC2080_I2C_ADDRESS, KETCUBE_HDC2080_TEMPERATURE_REG_L,
                 &(((uint8_t *) (&rawT))[0])) == FALSE) {
                ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_HDCX080,
                                              "Read temperature failed (errno = %d)!", ketCube_hdcX080_moduleCfg.errno);
                return FALSE;
            }
            if (ketCube_I2C_HDC2080ReadReg
                (KETCUBE_HDC2080_I2C_ADDRESS, KETCUBE_HDC2080_TEMPERATURE_REG_H,
                 &(((uint8_t *) &rawT)[1])) == FALSE) {
                ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_HDCX080,
                                              "Read temperature failed (errno = %d)!", ketCube_hdcX080_moduleCfg.errno);
                return FALSE;
            }
            break;
        default:
            return FALSE;
    }    
    
    *value =
        (int16_t) (10.0 *
                   (((((float) rawT) / pow(2, 16)) * 165.0) - 40.0));

    return TRUE;
}

/**
  * @brief Read data from HDCX080 sensor
  *
  * @param buffer pointer to fuffer for storing the result of milivolt mesurement
  * @param len data len in bytes
  *
  * @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_hdcX080_ReadData(uint8_t * buffer,
                                                uint8_t * len)
{
    uint8_t i = 0;
    int16_t temp = 0;
    uint16_t temperature = 0;
    uint16_t humidity = 0;
    
    ketCube_hdc2080_Init_t pxInit = { 0 };
    
    switch (ketCube_hdcX080_moduleCfg.sensType) {
        case KETCUBE_HDCX080_TYPE_HDC2080:
            /* Initiate measurement */
            pxInit.TemperatureMeasurementResolution = KETCUBE_HDC2080_TRES_14BIT;
            pxInit.HumidityMeasurementResolution = KETCUBE_HDCX080_HRES_14BIT;
            pxInit.MeasCfg = KETCUBE_HDC2080_MEASCFG_RHT;
            pxInit.MeasTrig = KETCUBE_HDC2080_MEASTRIG_START;
            
            ketCube_hdcX080_moduleCfg.errno = KETCUBE_HDCX080_RESULT_OK; /* init errno  */
            if (ketCube_I2C_HDC2080WriteReg
                (KETCUBE_HDC2080_I2C_ADDRESS, KETCUBE_HDC2080_MEASCFG_REG,
                 ((uint8_t *) &pxInit)[1] ) == FALSE) {
                ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_HDCX080,
                                              "HDC2080 measurement initialization failed (errno = %d)!", ketCube_hdcX080_moduleCfg.errno);
                return KETCUBE_CFG_MODULE_ERROR;
            }
            break;
            HAL_Delay(1); // wait for conversion
        case KETCUBE_HDCX080_TYPE_HDC1080:
            break;
        default:
        case KETCUBE_HDCX080_TYPE_AUTODETECT:
            ketCube_hdcX080_moduleCfg.errno = KETCUBE_HDCX080_RESULT_ERROR_SENSDET;
            ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_HDCX080,
                                              "Module configuration Error (errno = %d)!", ketCube_hdcX080_moduleCfg.errno);
            break;
    }

    /* in °C * 10 */
    if (getTemperature(&temp) == TRUE) {
        /* sign shift */
        temperature = (uint16_t) (10000 + ((int16_t) (temp)));  /*  x * 10 - 10000 in C */
    } else {
        temperature = ((0xFF << 8) | (ketCube_hdcX080_moduleCfg.errno));   // out-of the range value indicates error
    }
    
    /* in % * 10  */
    if (getHumidity(&humidity) == FALSE) {
        humidity = ((0xFF << 8) | (ketCube_hdcX080_moduleCfg.errno));      // out-of the range value indicates error
    }

    buffer[i++] = (temperature >> 8) & 0xFF;
    buffer[i++] = (temperature & 0xFF);
    buffer[i++] = (humidity >> 8) & 0xFF;
    buffer[i++] = (humidity & 0xFF);

    *len = i;

    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_HDCX080,
                                 "Temperature: %d °C, RH: %d %%",
                                 (((int) temperature - 10000) / 10),
                                 (humidity / 10));

    return KETCUBE_CFG_MODULE_OK;
}

#endif                          /* KETCUBE_CFG_INC_MOD_HDCX080 */
