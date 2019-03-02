/**
 * @file    ketCube_bmeX80.c
 * @author  Krystof Vanek
 * @version alpha
 * @date    2018-09-20
 * @brief   This file contains the BMEx80 (BME280 and BME680) driver
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

#include "ketCube_cfg.h"
#include "ketCube_terminal.h"
#include "ketCube_i2c.h"
#include "ketCube_bmeX80.h"

#ifdef KETCUBE_CFG_INC_MOD_BMEX80

ketCube_bmeX80_moduleCfg_t ketCube_bmeX80_moduleCfg; /*!< Module configuration storage */

extern void bench_StoreData_BME280(int16_t temperature, uint16_t humidity,
                                   uint16_t pressure);

static ketCube_cfg_ModError_t getCalibration(ketCube_bmeX80_Calib_t *
                                             calibration);
static ketCube_cfg_ModError_t getHumidity(uint32_t * value,
                                          ketCube_bmeX80_Calib_t *
                                          calibration);
static ketCube_cfg_ModError_t getTemperature(int16_t * value,
                                             ketCube_bmeX80_Calib_t *
                                             calibration);
static ketCube_cfg_ModError_t getPressure(uint32_t * value,
                                          ketCube_bmeX80_Calib_t *
                                          calibration);

/**
 * @brief Initialize the BMEx80 sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_bmeX80_Init(ketCube_InterModMsg_t *** msg)
{

    // Init drivers
    if (ketCube_I2C_Init() != KETCUBE_CFG_MODULE_OK) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_BMEX80,
                                      "I2C Driver initialisation failure!");
        return KETCUBE_CFG_MODULE_ERROR;
    }
    // Query compatible chip
    uint8_t chipID;
    if (ketCube_I2C_ReadData(KETCUBE_BMEX80_I2C_ADDRESS,
                             KETCUBE_BMEX80_CHIP_ID_REG, &chipID, 1)) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_BMEX80,
                                      "ChipID Readout failure!");
        return KETCUBE_CFG_MODULE_ERROR;
    }

    if (!(chipID == KETCUBE_BMEX80_CHIP_ID)) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_BMEX80,
                                      "Invalid ChipID!");
        return KETCUBE_CFG_MODULE_ERROR;
    }
    //Configure humidity oversampling
    uint8_t tempData = KETCUBE_BMEX80_OS_H_X1;
    if (ketCube_I2C_WriteData(KETCUBE_BMEX80_I2C_ADDRESS,
                              KETCUBE_BMEX80_CTRL_HUM_REG, &tempData, 1))
        return KETCUBE_CFG_MODULE_ERROR;

    //Configure pressure and temperature oversampling
    tempData = KETCUBE_BMEX80_OS_T_X1 << 5;
    tempData |= KETCUBE_BMEX80_OS_P_X1 << 2;
    tempData |= 1;
    if (ketCube_I2C_WriteData(KETCUBE_BMEX80_I2C_ADDRESS,
                              KETCUBE_BMEX80_CTRL_MEAS_REG, &tempData, 1))
        return KETCUBE_CFG_MODULE_ERROR;

    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Initialize the BMEx80 sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_bmeX80_UnInit(void)
{
    // UnInit drivers
    return ketCube_I2C_UnInit();
}

/**
 * @brief Reads and sorts BMEx80 calibration data
 * @param  calibration pointer to calibration data structure
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t getCalibration(ketCube_bmeX80_Calib_t * calibration)
{
#if defined(KETCUBE_BMEX80_SENSOR_TYPE_BME280)
    //Read calibration data part 1 from chip
    uint8_t coeff_array[26] = { 0 };
    if (ketCube_I2C_ReadData(KETCUBE_BMEX80_I2C_ADDRESS,
                             KETCUBE_BMEX80_CALIB_1_FIRST_REG,
                             coeff_array, KETCUBE_BMEX80_CALIB_1_LENGTH))
        return KETCUBE_CFG_MODULE_ERROR;

    //Asign data to calibration structure

    /* Temperature related coefficients */
    calibration->dig_T1 = BME_CONCAT_BYTES(coeff_array[1], coeff_array[0]);
    calibration->dig_T2 =
        (int16_t) BME_CONCAT_BYTES(coeff_array[3], coeff_array[2]);
    calibration->dig_T3 =
        (int16_t) BME_CONCAT_BYTES(coeff_array[5], coeff_array[4]);

    /* Pressure related coefficients */
    calibration->dig_P1 = BME_CONCAT_BYTES(coeff_array[7], coeff_array[6]);
    calibration->dig_P2 =
        (int16_t) BME_CONCAT_BYTES(coeff_array[9], coeff_array[8]);
    calibration->dig_P3 =
        (int16_t) BME_CONCAT_BYTES(coeff_array[11], coeff_array[10]);
    calibration->dig_P4 =
        (int16_t) BME_CONCAT_BYTES(coeff_array[13], coeff_array[12]);
    calibration->dig_P5 =
        (int16_t) BME_CONCAT_BYTES(coeff_array[15], coeff_array[14]);
    calibration->dig_P6 =
        (int16_t) BME_CONCAT_BYTES(coeff_array[17], coeff_array[16]);
    calibration->dig_P7 =
        (int16_t) BME_CONCAT_BYTES(coeff_array[19], coeff_array[18]);
    calibration->dig_P8 =
        (int16_t) BME_CONCAT_BYTES(coeff_array[21], coeff_array[20]);
    calibration->dig_P9 =
        (int16_t) BME_CONCAT_BYTES(coeff_array[23], coeff_array[22]);

    /* Humidity related coefficients */
    calibration->dig_H1 = coeff_array[25];
    int16_t dig_H4_lsb;
    int16_t dig_H4_msb;
    int16_t dig_H5_lsb;
    int16_t dig_H5_msb;

    //Read calibration data part 2 from chip
    if (ketCube_I2C_ReadData(KETCUBE_BMEX80_I2C_ADDRESS,
                             KETCUBE_BMEX80_CALIB_2_FIRST_REG,
                             coeff_array, KETCUBE_BMEX80_CALIB_2_LENGTH))
        return KETCUBE_CFG_MODULE_ERROR;

    calibration->dig_H2 =
        (int16_t) BME_CONCAT_BYTES(coeff_array[1], coeff_array[0]);
    calibration->dig_H3 = coeff_array[2];

    dig_H4_msb = (int16_t) (int8_t) coeff_array[3] * 16;
    dig_H4_lsb = (int16_t) (coeff_array[4] & 0x0F);
    calibration->dig_H4 = dig_H4_msb | dig_H4_lsb;

    dig_H5_msb = (int16_t) (int8_t) coeff_array[5] * 16;
    dig_H5_lsb = (int16_t) (coeff_array[4] >> 4);
    calibration->dig_H5 = dig_H5_msb | dig_H5_lsb;
    calibration->dig_H6 = (int8_t) coeff_array[6];

    return KETCUBE_CFG_MODULE_OK;

    /* KETCUBE_BMEX80_SENSOR_TYPE_BME280 */
#elif defined(KETCUBE_BMEX80_SENSOR_TYPE_BME680)

    //Read calibration data from chip
    uint8_t coeff_array[41] = { 0 };
    if (ketCube_I2C_ReadData(KETCUBE_BMEX80_I2C_ADDRESS,
                             KETCUBE_BMEX80_CALIB_1_FIRST_REG, coeff_array,
                             KETCUBE_BMEX80_CALIB_1_LENGTH))
        return KETCUBE_CFG_MODULE_ERROR;
    if (ketCube_I2C_ReadData(KETCUBE_BMEX80_I2C_ADDRESS,
                             KETCUBE_BMEX80_CALIB_2_FIRST_REG,
                             &coeff_array[KETCUBE_BMEX80_CALIB_1_LENGTH],
                             KETCUBE_BMEX80_CALIB_2_LENGTH))
        return KETCUBE_CFG_MODULE_ERROR;

    //Asign data to calibration structure
    /* Temperature related coefficients */
    calibration->par_t1 =
        (uint16_t) (BME_CONCAT_BYTES
                    (coeff_array[BME680_T1_MSB_REG],
                     coeff_array[BME680_T1_LSB_REG]));
    calibration->par_t2 =
        (int16_t) (BME_CONCAT_BYTES
                   (coeff_array[BME680_T2_MSB_REG],
                    coeff_array[BME680_T2_LSB_REG]));
    calibration->par_t3 = (int8_t) (coeff_array[BME680_T3_REG]);

    /* Pressure related coefficients */
    calibration->par_p1 =
        (uint16_t) (BME_CONCAT_BYTES
                    (coeff_array[BME680_P1_MSB_REG],
                     coeff_array[BME680_P1_LSB_REG]));
    calibration->par_p2 =
        (int16_t) (BME_CONCAT_BYTES
                   (coeff_array[BME680_P2_MSB_REG],
                    coeff_array[BME680_P2_LSB_REG]));
    calibration->par_p3 = (int8_t) coeff_array[BME680_P3_REG];
    calibration->par_p4 =
        (int16_t) (BME_CONCAT_BYTES
                   (coeff_array[BME680_P4_MSB_REG],
                    coeff_array[BME680_P4_LSB_REG]));
    calibration->par_p5 =
        (int16_t) (BME_CONCAT_BYTES
                   (coeff_array[BME680_P5_MSB_REG],
                    coeff_array[BME680_P5_LSB_REG]));
    calibration->par_p6 = (int8_t) (coeff_array[BME680_P6_REG]);
    calibration->par_p7 = (int8_t) (coeff_array[BME680_P7_REG]);
    calibration->par_p8 =
        (int16_t) (BME_CONCAT_BYTES
                   (coeff_array[BME680_P8_MSB_REG],
                    coeff_array[BME680_P8_LSB_REG]));
    calibration->par_p9 =
        (int16_t) (BME_CONCAT_BYTES
                   (coeff_array[BME680_P9_MSB_REG],
                    coeff_array[BME680_P9_LSB_REG]));
    calibration->par_p10 = (uint8_t) (coeff_array[BME680_P10_REG]);

    /* Humidity related coefficients */
    calibration->par_h1 =
        (uint16_t) (((uint16_t) coeff_array[BME680_H1_MSB_REG]
                     << 4) | (coeff_array[BME680_H1_LSB_REG] & 0x0F));
    calibration->par_h2 =
        (uint16_t) (((uint16_t) coeff_array[BME680_H2_MSB_REG]
                     << 4) | ((coeff_array[BME680_H2_LSB_REG]) >> 4));
    calibration->par_h3 = (int8_t) coeff_array[BME680_H3_REG];
    calibration->par_h4 = (int8_t) coeff_array[BME680_H4_REG];
    calibration->par_h5 = (int8_t) coeff_array[BME680_H5_REG];
    calibration->par_h6 = (uint8_t) coeff_array[BME680_H6_REG];
    calibration->par_h7 = (int8_t) coeff_array[BME680_H7_REG];

    /* Gas heater related coefficients */
    calibration->par_gh1 = (int8_t) coeff_array[BME680_GH1_REG];
    calibration->par_gh2 =
        (int16_t) (BME_CONCAT_BYTES
                   (coeff_array[BME680_GH2_MSB_REG],
                    coeff_array[BME680_GH2_LSB_REG]));
    calibration->par_gh3 = (int8_t) coeff_array[BME680_GH3_REG];

    return KETCUBE_CFG_MODULE_OK;
    /* KETCUBE_BMEX80_SENSOR_TYPE_BME680 */
#else
    return KETCUBE_CFG_MODULE_ERROR;
#endif
}

/**
 * @brief  Read BMEx80 Humidity output registers, and calculate humidity.
 * @param  value pointer to the returned humidity value that must be divided by 10 to get the value in [%].
 * @param  calibration pointer to calibration data structure
 *
 * @retval KETCUBE_CFG_MODULE_OK if success
 * @retval KETCUBE_CFG_MODULE_ERROR otherwise
 */
ketCube_cfg_ModError_t getHumidity(uint32_t * value,
                                   ketCube_bmeX80_Calib_t * calibration)
{
    //Read out data
    uint8_t rawH[2];
    if (ketCube_I2C_ReadData(KETCUBE_BMEX80_I2C_ADDRESS,
                             KETCUBE_BMEX80_HUMIDITY_REG, rawH, 2))
        return KETCUBE_CFG_MODULE_ERROR;

    uint16_t hum_adc = ((uint16_t) rawH[0] << 8) | rawH[1];

    //Calculate calibrated value
#if defined(KETCUBE_BMEX80_SENSOR_TYPE_BME280)
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    uint32_t humidity;

    var1 = calibration->t_fine - ((int32_t) 76800);
    var2 = (int32_t) (hum_adc * 16384);
    var3 = (int32_t) (((int32_t) calibration->dig_H4) * 1048576);
    var4 = ((int32_t) calibration->dig_H5) * var1;
    var5 = (((var2 - var3) - var4) + (int32_t) 16384) / 32768;
    var2 = (var1 * ((int32_t) calibration->dig_H6)) / 1024;
    var3 = (var1 * ((int32_t) calibration->dig_H3)) / 2048;
    var4 = ((var2 * (var3 + (int32_t) 32768)) / 1024) + (int32_t) 2097152;
    var2 = ((var4 * ((int32_t) calibration->dig_H2)) + 8192) / 16384;
    var3 = var5 * var2;
    var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
    var5 = var3 - ((var4 * ((int32_t) calibration->dig_H1)) / 16);
    var5 = (var5 < 0 ? 0 : var5);
    var5 = (var5 > 419430400 ? 419430400 : var5);
    humidity = (uint32_t) (var5 / 4096);

    if (humidity > 102400)
        humidity = 102400;

    *value = humidity;
    return KETCUBE_CFG_MODULE_OK;
#elif defined(KETCUBE_BMEX80_SENSOR_TYPE_BME680)
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    int32_t var6;
    int32_t temp_scaled;
    int32_t calc_hum;

    temp_scaled = (((int32_t) calibration->t_fine * 5) + 128) >> 8;
    var1 =
        (int32_t) (hum_adc
                   - ((int32_t) ((int32_t) calibration->par_h1 * 16)))
        - (((temp_scaled * (int32_t) calibration->par_h3)
            / ((int32_t) 100)) >> 1);
    var2 = ((int32_t) calibration->par_h2
            *
            (((temp_scaled * (int32_t) calibration->par_h4) /
              ((int32_t) 100))
             +
             (((temp_scaled *
                ((temp_scaled * (int32_t) calibration->par_h5)
                 / ((int32_t) 100))) >> 6) / ((int32_t) 100))
             + (int32_t) (1 << 14))) >> 10;
    var3 = var1 * var2;
    var4 = (int32_t) calibration->par_h6 << 7;
    var4 = ((var4)
            +
            ((temp_scaled * (int32_t) calibration->par_h7) /
             ((int32_t) 100)))
        >> 4;
    var5 = ((var3 >> 14) * (var3 >> 14)) >> 10;
    var6 = (var4 * var5) >> 1;
    calc_hum = (((var3 + var6) >> 10) * ((int32_t) 1000)) >> 12;

    if (calc_hum > 100000)      /* Cap at 100%rH */
        calc_hum = 100000;
    else if (calc_hum < 0)
        calc_hum = 0;

    *value = (uint32_t) calc_hum;
    return KETCUBE_CFG_MODULE_OK;
#else                           /* KETCUBE_BMEX80_SENSOR_TYPE_BME680 */
    return KETCUBE_CFG_MODULE_ERROR;
#endif
}

/**
 * @brief  Read BMEx80 temperature output registers, and calculate temperature.
 * @param  value a pointer to the returned temperature value that must be divided by 10 to get the value in ['C].
 * @param  calibration pointer to calibration data structure
 *
 * @retval KETCUBE_CFG_MODULE_OK if success
 * @retval KETCUBE_CFG_MODULE_ERROR otherwise
 */
ketCube_cfg_ModError_t getTemperature(int16_t * value,
                                      ketCube_bmeX80_Calib_t * calibration)
{

    //Read out data
    uint8_t rawT[3];
    if (ketCube_I2C_ReadData(KETCUBE_BMEX80_I2C_ADDRESS,
                             KETCUBE_BMEX80_TEMPERATURE_REG, rawT, 3))
        return KETCUBE_CFG_MODULE_ERROR;

    uint32_t temp_adc = (uint32_t) rawT[0] << 12 | (uint32_t) rawT[1] << 4
        | rawT[2] >> 4;

    //Calculate calibrated value
#if defined(KETCUBE_BMEX80_SENSOR_TYPE_BME280)
    int32_t var1;
    int32_t var2;
    int32_t temperature;
    int32_t temperature_min = -4000;
    int32_t temperature_max = 8500;

    var1 =
        (int32_t) ((temp_adc / 8) - ((int32_t) calibration->dig_T1 * 2));
    var1 = (var1 * ((int32_t) calibration->dig_T2)) / 2048;
    var2 = (int32_t) ((temp_adc / 16) - ((int32_t) calibration->dig_T1));
    var2 =
        (((var2 * var2) / 4096) * ((int32_t) calibration->dig_T3)) / 16384;
    calibration->t_fine = var1 + var2;
    temperature = (calibration->t_fine * 5 + 128) / 256;

    if (temperature < temperature_min)
        temperature = temperature_min;
    else if (temperature > temperature_max)
        temperature = temperature_max;

    *value = (int16_t) temperature;
    return KETCUBE_CFG_MODULE_OK;
#elif defined(KETCUBE_BMEX80_SENSOR_TYPE_BME680)
    int64_t var1;
    int64_t var2;
    int64_t var3;
    int16_t calc_temp;

    var1 =
        ((int32_t) temp_adc >> 3) - ((int32_t) calibration->par_t1 << 1);
    var2 = (var1 * (int32_t) calibration->par_t2) >> 11;
    var3 = ((var1 >> 1) * (var1 >> 1)) >> 12;
    var3 = ((var3) * ((int32_t) calibration->par_t3 << 4)) >> 14;
    calibration->t_fine = (int32_t) (var2 + var3);
    calc_temp = (int16_t) (((calibration->t_fine * 5) + 128) >> 8);

    *value = calc_temp;
    return KETCUBE_CFG_MODULE_OK;
#else                           /* KETCUBE_BMEX80_SENSOR_TYPE_BME680 */
    return KETCUBE_CFG_MODULE_ERROR;
#endif
}

/**
 * @brief  Read BMEx80 pressure output registers, and calculate pressure.
 * @param  value a pointer to the returned pressure value in [hPa].
 * @param  calibration pointer to calibration data structure
 *
 * @retval KETCUBE_CFG_MODULE_OK if success
 * @retval KETCUBE_CFG_MODULE_ERROR otherwise
 */
ketCube_cfg_ModError_t getPressure(uint32_t * value,
                                   ketCube_bmeX80_Calib_t * calibration)
{

    //Read out data
    uint8_t rawP[3] = { 0 };
    if (ketCube_I2C_ReadData(KETCUBE_BMEX80_I2C_ADDRESS,
                             KETCUBE_BMEX80_PRESSURE_REG, rawP, 3)) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    uint32_t pres_adc = (uint32_t) rawP[0] << 12 | (uint32_t) rawP[1] << 4
        | rawP[2] >> 4;

    //Calculate calibrated value
#if defined(KETCUBE_BMEX80_SENSOR_TYPE_BME280)
    int64_t var1;
    int64_t var2;
    int64_t var3;
    int64_t var4;
    uint32_t pressure;
    uint32_t pressure_min = 3000000;
    uint32_t pressure_max = 11000000;

    var1 = ((int64_t) calibration->t_fine) - 128000;
    var2 = var1 * var1 * (int64_t) calibration->dig_P6;
    var2 = var2 + ((var1 * (int64_t) calibration->dig_P5) * 131072);
    var2 = var2 + (((int64_t) calibration->dig_P4) * 34359738368);
    var1 = ((var1 * var1 * (int64_t) calibration->dig_P3) / 256)
        + ((var1 * ((int64_t) calibration->dig_P2) * 4096));
    var3 = ((int64_t) 1) * 140737488355328;
    var1 = (var3 + var1) * ((int64_t) calibration->dig_P1) / 8589934592;

    /* To avoid divide by zero exception */
    if (var1 != 0) {
        var4 = 1048576 - pres_adc;
        var4 = (((var4 * 2147483648) - var2) * 3125) / var1;
        var1 =
            (((int64_t) calibration->dig_P9) * (var4 / 8192) *
             (var4 / 8192))
            / 33554432;
        var2 = (((int64_t) calibration->dig_P8) * var4) / 524288;
        var4 = ((var4 + var1 + var2) / 256)
            + (((int64_t) calibration->dig_P7) * 16);
        pressure = (uint32_t) (((var4 / 2) * 100) / 128);

        if (pressure < pressure_min)
            pressure = pressure_min;
        else if (pressure > pressure_max)
            pressure = pressure_max;
    } else {
        pressure = pressure_min;
    }

    *value = pressure / 100;
    return KETCUBE_CFG_MODULE_OK;
#elif defined(KETCUBE_BMEX80_SENSOR_TYPE_BME680)
    int32_t var1 = 0;
    int32_t var2 = 0;
    int32_t var3 = 0;
    int32_t pressure_comp = 0;

    var1 = (((int32_t) calibration->t_fine) >> 1) - 64000;
    var2 =
        ((((var1 >> 2) * (var1 >> 2)) >> 11) *
         (int32_t) calibration->par_p6)
        >> 2;
    var2 = var2 + ((var1 * (int32_t) calibration->par_p5) << 1);
    var2 = (var2 >> 2) + ((int32_t) calibration->par_p4 << 16);
    var1 = (((((var1 >> 2) * (var1 >> 2)) >> 13)
             * ((int32_t) calibration->par_p3 << 5)) >> 3)
        + (((int32_t) calibration->par_p2 * var1) >> 1);
    var1 = var1 >> 18;
    var1 = ((32768 + var1) * (int32_t) calibration->par_p1) >> 15;
    pressure_comp = 1048576 - pres_adc;
    pressure_comp = (int32_t) ((pressure_comp - (var2 >> 12))
                               * ((uint32_t) 3125));
    if (pressure_comp >= 0x40000000)
        pressure_comp = ((pressure_comp / (uint32_t) var1) << 1);
    else
        pressure_comp = ((pressure_comp << 1) / (uint32_t) var1);
    var1 = ((int32_t) calibration->par_p9
            *
            (int32_t) (((pressure_comp >> 3) *
                        (pressure_comp >> 3)) >> 13))
        >> 12;
    var2 = ((int32_t) (pressure_comp >> 2) * (int32_t) calibration->par_p8)
        >> 13;
    var3 = ((int32_t) (pressure_comp >> 8) * (int32_t) (pressure_comp >> 8)
            * (int32_t) (pressure_comp >> 8) *
            (int32_t) calibration->par_p10)
        >> 17;

    pressure_comp = (int32_t) (pressure_comp)
        + ((var1 + var2 + var3
            + ((int32_t) calibration->par_p7 << 7)) >> 4);

    *value = (uint32_t) (pressure_comp);
    return KETCUBE_CFG_MODULE_OK;
#else                           /* KETCUBE_BMEX80_SENSOR_TYPE_BME680 */
    return KETCUBE_CFG_MODULE_ERROR;
#endif
}

/**
 * @brief Read data from BMEx80 sensor
 *
 * @param buffer pointer to buffer for storing the result of mesurement
 * @param len data len in bytes
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_bmeX80_ReadData(uint8_t * buffer,
                                               uint8_t * len)
{

    uint8_t i = 0;
    int16_t temperature = 0;
    uint32_t humidity = 0;
    uint32_t pressure = 0;

    // Query compatible chip
    uint8_t chipID;
    if (ketCube_I2C_ReadData(KETCUBE_BMEX80_I2C_ADDRESS,
                             KETCUBE_BMEX80_CHIP_ID_REG, &chipID, 1)) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    if (!(chipID == KETCUBE_BMEX80_CHIP_ID)) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_BMEX80,
                                      "Invalid ChipID!");
        buffer[i++] = 0xFF;
        buffer[i++] = 0xFF;
        buffer[i++] = 0xFF;
        buffer[i++] = 0xFF;
        *len = i;
        //bench_StoreData_BME280(INT16_MAX,UINT16_MAX,UINT16_MAX);
        return KETCUBE_CFG_MODULE_ERROR;
    }
    //Read measurement configuration
    uint8_t tempData = 0;
    if (ketCube_I2C_ReadData(KETCUBE_BMEX80_I2C_ADDRESS,
                             KETCUBE_BMEX80_CTRL_MEAS_REG, &tempData, 1)) {
        return KETCUBE_CFG_MODULE_ERROR;
    }
    //Switch mode to Forced - one shot measurement
    tempData |= 1;
    //Write back configuration data
    if (ketCube_I2C_WriteData(KETCUBE_BMEX80_I2C_ADDRESS,
                              KETCUBE_BMEX80_CTRL_MEAS_REG, &tempData,
                              1)) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    ketCube_bmeX80_Calib_t calibration = { 0 };
    getCalibration(&calibration);

    //Read status register - check data ready
    uint32_t tick = 0;
    do {
        if (ketCube_I2C_ReadData(KETCUBE_BMEX80_I2C_ADDRESS,
                                 KETCUBE_BMEX80_STATUS_REG, &tempData, 1))
            return KETCUBE_CFG_MODULE_ERROR;
        tick += 1;
    } while ((tempData & (1 << KETCUBE_BMEX80_MEASURING_SHIFT))
             && tick != 0);

    /* Process temperature FIRST! as it is needed to calculate calibration->t_fine */
    getTemperature(&temperature, &calibration); /* in °C * 100   */
    getHumidity(&humidity, &calibration);       /* in % * 1000   */
    getPressure(&pressure, &calibration);       /* in hPa* 100     */
    //temperature = (uint16_t) 10000 + ((int16_t) (temp * 10));   /*  x * 10 - 10000 in C */

    buffer[i++] = (uint8_t) (humidity / 500);   //Relative humidity in 0.5*%
    buffer[i++] = (uint8_t) (80 + temperature / 50);    //Temperature in 0.5*°C with 40°C offset
    buffer[i++] = ((pressure / 50) >> 8) & 0xFF;        //Pressure in 0.5*hPa
    buffer[i++] = ((pressure / 50) & 0xFF);

    *len = i;

#if defined(KETCUBE_BMEX80_SENSOR_TYPE_BME280)
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_BMEX80,
                                 "BME280 ::  Temperature: %.2f, RH: %.2f, Pressure: %.2f",
                                 temperature / 100.0, humidity / 1000.0,
                                 pressure / 100.0);

#elif defined(KETCUBE_BMEX80_SENSOR_TYPE_BME680)
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_BMEX80,
                                 "BME680 :: Temperature: %.2f, RH: %.2f, Pressure: %.2f",
                                 temperature / 100.0, humidity / 1000.0,
                                 pressure / 100.0);
#endif                          /* KETCUBE_BMEX80_SENSOR_TYPE_BME680 */

    return KETCUBE_CFG_MODULE_OK;

}

#endif                          /* KETCUBE_CFG_INC_MOD_BMEX80 */
