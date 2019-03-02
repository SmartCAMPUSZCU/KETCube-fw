/**
 * @file    ketCube_bmeX80.h
 * @author  Krystof Vanek
 * @version alpha
 * @date    2018-09-20
 * @brief   This file contains definitions for the BMEx80 (BME280 and BME680) driver
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

#ifndef __KETCUBE_BMEX80_H
#define __KETCUBE_BMEX80_H

#include "ketCube_cfg.h"
#include "ketCube_common.h"

#define KETCUBE_BMEX80_SENSOR_TYPE_BME280
//#define KETCUBE_BMEX80_SENSOR_TYPE_BME680

/** @defgroup KETCube_BMEx80 KETCube BMEx80
  * @brief KETCube BMEx80 module
  * @ingroup KETCube_SensMods
  * @{
  */

/**
* @brief  KETCube module configuration
*/
typedef struct ketCube_bmeX80_moduleCfg_t {
    ketCube_cfg_ModuleCfgByte_t coreCfg;           /*<! KETCube core cfg byte */
} ketCube_bmeX80_moduleCfg_t;

extern ketCube_bmeX80_moduleCfg_t ketCube_bmeX80_moduleCfg;

/** @defgroup KETCube_BMEx80_defs Public Defines
  * @brief Public defines
  * @{
  */

/**
* @brief  Relative humidity oversampling.
*/
typedef enum {
    KETCUBE_BMEX80_OS_H_X0 = (uint8_t) 0x00,    /*!<  measurement skipped, output 0X8000 */
    KETCUBE_BMEX80_OS_H_X1 = (uint8_t) 0x01,    /*!<  16 bit resolution, 1 sample */
    KETCUBE_BMEX80_OS_H_X2 = (uint8_t) 0x02,    /*!<  16 bit resolution, 2 samples */
    KETCUBE_BMEX80_OS_H_X4 = (uint8_t) 0x03,    /*!<  16 bit resolution, 4 samples */
    KETCUBE_BMEX80_OS_H_X8 = (uint8_t) 0x04,    /*!<  16 bit resolution, 8 samples */
    KETCUBE_BMEX80_OS_H_X16 = (uint8_t) 0x05,   /*!<  16 bit resolution, 16 samples */
} ketCube_bmeX80_OS_H_t;

/**
* @brief  Temperature oversampling.
*/
typedef enum {
    KETCUBE_BMEX80_OS_T_X0 = (uint8_t) 0x00,    /*!<  measurement skipped, output 0X8000 */
    KETCUBE_BMEX80_OS_T_X1 = (uint8_t) 0x01,    /*!<  16 bit resolution, 1 sample */
    KETCUBE_BMEX80_OS_T_X2 = (uint8_t) 0x02,    /*!<  17 bit resolution, 2 samples */
    KETCUBE_BMEX80_OS_T_X4 = (uint8_t) 0x03,    /*!<  18 bit resolution, 4 samples */
    KETCUBE_BMEX80_OS_T_X8 = (uint8_t) 0x04,    /*!<  19 bit resolution, 8 samples */
    KETCUBE_BMEX80_OS_T_X16 = (uint8_t) 0x05,   /*!<  20 bit resolution, 16 samples */
} ketCube_bmeX80_OS_T_t;

/**
* @brief  Pressure oversampling.
*/
typedef enum {
    KETCUBE_BMEX80_OS_P_X0 = (uint8_t) 0x00,    /*!<  measurement skipped, output 0X8000 */
    KETCUBE_BMEX80_OS_P_X1 = (uint8_t) 0x01,    /*!<  16 bit resolution, 1 sample */
    KETCUBE_BMEX80_OS_P_X2 = (uint8_t) 0x02,    /*!<  17 bit resolution, 2 samples */
    KETCUBE_BMEX80_OS_P_X4 = (uint8_t) 0x03,    /*!<  18 bit resolution, 4 samples */
    KETCUBE_BMEX80_OS_P_X8 = (uint8_t) 0x04,    /*!<  19 bit resolution, 8 samples */
    KETCUBE_BMEX80_OS_P_X16 = (uint8_t) 0x05,   /*!<  20 bit resolution, 16 samples */
} ketCube_bmeX80_OS_P_t;

#ifdef KETCUBE_BMEX80_SENSOR_TYPE_BME280
/**
* @brief  Calibration data structure.
*/
typedef struct {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;
    int32_t t_fine;
} ketCube_bmeX80_Calib_t;
#endif                          /* KETCUBE_BMEX80_SENSOR_TYPE_BME280 */

#ifdef KETCUBE_BMEX80_SENSOR_TYPE_BME680
/**
* @brief  Calibration data structure.
*/
typedef struct {
    uint16_t par_h1;
    uint16_t par_h2;
    int8_t par_h3;
    int8_t par_h4;
    int8_t par_h5;
    uint8_t par_h6;
    int8_t par_h7;
    int8_t par_gh1;
    int16_t par_gh2;
    int8_t par_gh3;
    uint16_t par_t1;
    int16_t par_t2;
    int8_t par_t3;
    uint16_t par_p1;
    int16_t par_p2;
    int8_t par_p3;
    int16_t par_p4;
    int16_t par_p5;
    int8_t par_p6;
    int8_t par_p7;
    int16_t par_p8;
    int16_t par_p9;
    uint8_t par_p10;
    int32_t t_fine;
} ketCube_bmeX80_Calib_t;
#endif                          /* KETCUBE_BMEX80_SENSOR_TYPE_BME680 */

/**
* @brief  Macro to combine two 8 bit data's to form a 16 bit data
*/
#define BME_CONCAT_BYTES(msb, lsb)	(((uint16_t)msb << 8) | (uint16_t)lsb)

/**
* @brief  Array Index to Field data mapping for Calibration Data of BME680
*/
#define BME680_T2_LSB_REG	1
#define BME680_T2_MSB_REG	2
#define BME680_T3_REG		3
#define BME680_P1_LSB_REG	5
#define BME680_P1_MSB_REG	6
#define BME680_P2_LSB_REG	7
#define BME680_P2_MSB_REG	8
#define BME680_P3_REG		9
#define BME680_P4_LSB_REG	11
#define BME680_P4_MSB_REG	12
#define BME680_P5_LSB_REG	13
#define BME680_P5_MSB_REG	14
#define BME680_P7_REG		15
#define BME680_P6_REG		16
#define BME680_P8_LSB_REG	19
#define BME680_P8_MSB_REG	20
#define BME680_P9_LSB_REG	21
#define BME680_P9_MSB_REG	22
#define BME680_P10_REG		23
#define BME680_H2_MSB_REG	25
#define BME680_H2_LSB_REG	26
#define BME680_H1_LSB_REG	26
#define BME680_H1_MSB_REG	27
#define BME680_H3_REG		28
#define BME680_H4_REG		29
#define BME680_H5_REG		30
#define BME680_H6_REG		31
#define BME680_H7_REG		32
#define BME680_T1_LSB_REG	33
#define BME680_T1_MSB_REG	34
#define BME680_GH2_LSB_REG	35
#define BME680_GH2_MSB_REG	36
#define BME680_GH1_REG		37
#define BME680_GH3_REG		38

/**
* @brief  I2C address.
*/
//#define KETCUBE_BMEX80_I2C_ADDRESS  (uint8_t) (0x77 << 1)     /* SDO pin HIGH */
#define KETCUBE_BMEX80_I2C_ADDRESS  (uint8_t) (0x76 << 1)       /* SDO pin LOW  */

#define KETCUBE_BMEX80_CALIB_2_LENGTH	16

#ifdef KETCUBE_BMEX80_SENSOR_TYPE_BME280
#define KETCUBE_BMEX80_CHIP_ID			0x60
#define KETCUBE_BMEX80_CALIB_1_LENGTH	26
#define KETCUBE_BMEX80_MEASURING_SHIFT	3
#endif                          /* KETCUBE_BMEX80_SENSOR_TYPE_BME280 */

#ifdef KETCUBE_BMEX80_SENSOR_TYPE_BME680
#define KETCUBE_BMEX80_CHIP_ID			0x61
#define KETCUBE_BMEX80_CALIB_1_LENGTH	25
#define KETCUBE_BMEX80_MEASURING_SHIFT	5
#endif                          /* KETCUBE_BMEX80_SENSOR_TYPE_BME680 */
/**
* @}
*/

/**
* @addtogroup KETCUBE_BMEx80_Registers BMEx80 Registers
* @{
*/

#define KETCUBE_BMEX80_CHIP_ID_REG				0xD0
#define KETCUBE_BMEX80_CALIB_2_FIRST_REG		0xE1

#ifdef KETCUBE_BMEX80_SENSOR_TYPE_BME280
#define KETCUBE_BMEX80_HUMIDITY_REG			0XFD
#define KETCUBE_BMEX80_TEMPERATURE_REG		0XFA
#define KETCUBE_BMEX80_PRESSURE_REG			0XF7
#define KETCUBE_BMEX80_CTRL_MEAS_REG		0XF4
#define KETCUBE_BMEX80_STATUS_REG			0XF3
#define KETCUBE_BMEX80_CTRL_HUM_REG			0XF2
#define KETCUBE_BMEX80_CALIB_1_FIRST_REG	0x88
#endif                          /* KETCUBE_BMEX80_SENSOR_TYPE_BME280 */

#ifdef KETCUBE_BMEX80_SENSOR_TYPE_BME680
#define KETCUBE_BMEX80_HUMIDITY_REG			0X25
#define KETCUBE_BMEX80_TEMPERATURE_REG		0X22
#define KETCUBE_BMEX80_PRESSURE_REG			0X1F
#define KETCUBE_BMEX80_CTRL_MEAS_REG		0X74
#define KETCUBE_BMEX80_CTRL_HUM_REG			0X72
#define KETCUBE_BMEX80_CALIB_1_FIRST_REG	0x89
#define KETCUBE_BMEX80_STATUS_REG			0X1D
#endif                          /* KETCUBE_BMEX80_SENSOR_TYPE_BME680 */

/**
* @}
*/

/** @defgroup KETCube_BMEx80_fn Public Functions
  * @brief Public functions
  * @{
  */

extern ketCube_cfg_ModError_t ketCube_bmeX80_Init(ketCube_InterModMsg_t
                                                  *** msg);
extern ketCube_cfg_ModError_t ketCube_bmeX80_UnInit(void);
extern ketCube_cfg_ModError_t ketCube_bmeX80_ReadData(uint8_t * buffer,
                                                      uint8_t * len);

/**
* @}
*/

/**
* @}
*/



#endif                          /* __KETCUBE_BMEX80_H */
