/**
 * @file    ketCube_hdc1080.c
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-01-04
 * @brief   This file contains the HDC1080 driver
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

#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_i2c.h"
#include <math.h>

#include "ketCube_cfg.h"
#include "ketCube_terminal.h"
#include "ketCube_i2c.h"
#include "ketCube_hdc1080.h"

#ifdef KETCUBE_CFG_INC_MOD_HDC1080

ketCube_cfg_ModError_t getHumidity(uint16_t * value);
ketCube_cfg_ModError_t getTemperature(int16_t * value);
ketCube_cfg_ModError_t setHeaterState(ketCube_hdc1080_Htr_t status);
ketCube_cfg_ModError_t getHeaterState(ketCube_hdc1080_Htr_t * status);

/**
 * @brief Initialize the HDC1080 sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_hdc1080_Init(ketCube_InterModMsg_t *** msg)
{

    // Init drivers
    if (ketCube_I2C_Init() != KETCUBE_CFG_MODULE_OK) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    ketCube_hdc1080_Init_t pxInit;
    pxInit.TemperatureMeasurementResolution = KETCUBE_HDC1080_TRES_14BIT;
    pxInit.HumidityMeasurementResolution = KETCUBE_HDC1080_HRES_14BIT;
    pxInit.ModeOfAcquisition = KETCUBE_HDC1080_AQ_SEPARATE;

    if (ketCube_I2C_TexasWriteReg
        (KETCUBE_HDC1080_I2C_ADDRESS, KETCUBE_HDC1080_CONFIGURATION_REG,
         (uint16_t *) & pxInit)) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Initialize the HDC1080 sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_hdc1080_UnInit(void)
{
    // UnInit drivers
    return ketCube_I2C_UnInit();
}

/**
* @brief  Read HDC1080 Humidity output registers, and calculate humidity.
* @param  value pointer to the returned humidity value that must be divided by 10 to get the value in [%].
* 
* @retval KETCUBE_CFG_MODULE_OK if success
* @retval KETCUBE_CFG_MODULE_ERROR otherwise
*/
ketCube_cfg_ModError_t getHumidity(uint16_t * value)
{
    uint16_t rawH;

    if (ketCube_I2C_TexasReadReg
        (KETCUBE_HDC1080_I2C_ADDRESS, KETCUBE_HDC1080_HUMIDITY_REG,
         &rawH)) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    *value = (uint16_t) (((float) (((float) rawH) / pow(2, 16))) * 1000.0);

    return KETCUBE_CFG_MODULE_OK;
}

/**
* @brief  Read HDC1080 temperature output registers, and calculate temperature.
* @param  value a pointer to the returned temperature value that must be divided by 10 to get the value in ['C].
* 
* @retval KETCUBE_CFG_MODULE_OK if success
* @retval KETCUBE_CFG_MODULE_ERROR otherwise
*/
ketCube_cfg_ModError_t getTemperature(int16_t * value)
{
    uint16_t rawT;

    if (ketCube_I2C_TexasReadReg
        (KETCUBE_HDC1080_I2C_ADDRESS, KETCUBE_HDC1080_TEMPERATURE_REG,
         &rawT)) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    *value = (int16_t) (((((float) rawT) / pow(2, 16)) * 165.0) - 40.0);

    return KETCUBE_CFG_MODULE_OK;
}

/**
* @brief  Configure the internal heater.
* @param  status The status of the internal heater
*
* @retval Error code [KETCUBE_CFG_MODULE_OK, KETCUBE_CFG_MODULE_ERROR]
*/
ketCube_cfg_ModError_t setHeaterState(ketCube_hdc1080_Htr_t status)
{
    ketCube_hdc1080_Init_t reg;
    reg.Heater = status;

    if (ketCube_I2C_TexasWriteReg
        (KETCUBE_HDC1080_I2C_ADDRESS, KETCUBE_HDC1080_CONFIGURATION_REG,
         (uint16_t *) & reg)) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    return KETCUBE_CFG_MODULE_OK;
}

/**
* @brief  Get the internal heater.
* @param  status pointer to the returned status of the internal heater
*
* @retval KETCUBE_CFG_MODULE_OK if success
* @retval KETCUBE_CFG_MODULE_ERROR otherwise
*/
ketCube_cfg_ModError_t getHeaterState(ketCube_hdc1080_Htr_t * status)
{
    ketCube_hdc1080_Init_t reg;

    if (ketCube_I2C_TexasReadReg
        (KETCUBE_HDC1080_I2C_ADDRESS, KETCUBE_HDC1080_CONFIGURATION_REG,
         (uint16_t *) & reg)) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    *status = reg.Heater;

    return KETCUBE_CFG_MODULE_OK;
}

/**
  * @brief Read data from HDC1080 sensor
  *
  * @param buffer pointer to fuffer for storing the result of milivolt mesurement
  * @param len data len in bytes
  *
  * @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t ketCube_hdc1080_ReadData(uint8_t * buffer,
                                                uint8_t * len)
{
    uint8_t i = 0;
    int16_t temp = 0;
    uint16_t temperature = 0;
    uint16_t humidity = 0;

    getHumidity(&humidity);     /* in % * 10   */
    getTemperature(&temp);
    temperature = (uint16_t) 10000 + ((int16_t) (temp * 10));   /*  x * 10 - 10000 in C */

    buffer[i++] = (temperature >> 8) & 0xFF;
    buffer[i++] = (temperature & 0xFF);
    buffer[i++] = (humidity >> 8) & 0xFF;
    buffer[i++] = (humidity & 0xFF);

    *len = i;

    ketCube_terminal_DebugPrintln("HDC1080 :: Temperature: %d, RH: %d",
                                  (((int) temperature - 10000) / 10),
                                  (humidity / 10));

    return KETCUBE_CFG_MODULE_OK;
}

#endif                          /* KETCUBE_CFG_INC_MOD_HDC1080 */
