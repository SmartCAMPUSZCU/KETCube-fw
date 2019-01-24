/**
 * @file    ketCube_lis2hh12.c
 * @author  Krystof Vanek
 * @version pre-alpha
 * @date    2018-11-05
 * @brief   This file contains the LIS2HH12 module
 *
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

#include "ketCube_cfg.h"
#include "ketCube_terminal.h"
#include "ketCube_i2c.h"
#include "ketCube_lis2hh12.h"

#ifdef KETCUBE_CFG_INC_MOD_LIS2HH12

/**
 * @brief Initialize the LIS2HH12 sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_lis2hh12_Init(ketCube_InterModMsg_t *** msg)
{

    // Init drivers
    if (ketCube_I2C_Init() != KETCUBE_CFG_MODULE_OK) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_LIS2HH12,
                                      "Initialisation failure! - I2C Driver");
        return KETCUBE_CFG_MODULE_ERROR;
    }

    uint8_t i2cByte;

    // Query compatible chip
    if (ketCube_I2C_ReadData(KETCUBE_LIS2HH12_I2C_ADDRESS,
                             KETCUBE_LIS2HH12_WHO_AM_I_REG, &i2cByte, 1)) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_LIS2HH12,
                                      "Initialisation failure! - WhoAmI Readout");
        return KETCUBE_CFG_MODULE_ERROR;
    }
    //Check LIS2HH12 identificator
    if (!(i2cByte == KETCUBE_LIS2HH12_WHO_AM_I)) {
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_LIS2HH12,
                                      "Invalid Who Am I! Got %X expected %X",
                                      i2cByte, KETCUBE_LIS2HH12_WHO_AM_I);
        return KETCUBE_CFG_MODULE_ERROR;
    }

    i2cByte = 0
        | KETCUBE_LIS2HH12_RESOLUTION_NORMAL
        | KETCUBE_LIS2HH12_ODR_10Hz
        | KETCUBE_LIS2HH12_DATA_UPDATE
        | KETCUBE_LIS2HH12_X_ENABLE
        | KETCUBE_LIS2HH12_Y_ENABLE | KETCUBE_LIS2HH12_Z_ENABLE;
    if (ketCube_I2C_WriteData(KETCUBE_LIS2HH12_I2C_ADDRESS,
                              KETCUBE_LIS2HH12_CTRL_REG1, &i2cByte, 1)) {
        return KETCUBE_CFG_MODULE_ERROR;
    }
    //Testing!-----------------------------------------------------------------
    i2cByte = 153;
    if (ketCube_I2C_WriteData(KETCUBE_LIS2HH12_I2C_ADDRESS,
                              KETCUBE_LIS2HH12_INT1_TSH_X1, &i2cByte, 1)) {
        return KETCUBE_CFG_MODULE_ERROR;
    }
    if (ketCube_I2C_WriteData(KETCUBE_LIS2HH12_I2C_ADDRESS,
                              KETCUBE_LIS2HH12_INT1_TSH_Y1, &i2cByte, 1)) {
        return KETCUBE_CFG_MODULE_ERROR;
    }
    if (ketCube_I2C_WriteData(KETCUBE_LIS2HH12_I2C_ADDRESS,
                              KETCUBE_LIS2HH12_INT1_TSH_Z1, &i2cByte, 1)) {
        return KETCUBE_CFG_MODULE_ERROR;
    }
    //Iterrupt source config
    i2cByte = 0
        | KETCUBE_LIS2HH12_AOI_DIS
        | KETCUBE_LIS2HH12_6D_DIS
        | KETCUBE_LIS2HH12_ZHIGH_DIS
        | KETCUBE_LIS2HH12_ZLOW_DIS
        | KETCUBE_LIS2HH12_YHIGH_EN
        | KETCUBE_LIS2HH12_YLOW_DIS
        | KETCUBE_LIS2HH12_XHIGH_DIS | KETCUBE_LIS2HH12_XLOW_DIS;
    if (ketCube_I2C_WriteData(KETCUBE_LIS2HH12_I2C_ADDRESS,
                              KETCUBE_LIS2HH12_INT1_CFG, &i2cByte, 1)) {
        return KETCUBE_CFG_MODULE_ERROR;
    }
    //Interrupt output config
    i2cByte = 0
        | KETCUBE_LIS2HH12_FIFO_DIS
        | KETCUBE_LIS2HH12_FIFO_TSH_DIS
        | KETCUBE_LIS2HH12_INT1_INACT_DIS
        | KETCUBE_LIS2HH12_INT1_IG2_DIS
        | KETCUBE_LIS2HH12_INT1_IG1_EN
        | KETCUBE_LIS2HH12_INT1_OVR_DIS
        | KETCUBE_LIS2HH12_INT1_FTH_DIS | KETCUBE_LIS2HH12_INT1_DRDY_DIS;
    if (ketCube_I2C_WriteData(KETCUBE_LIS2HH12_I2C_ADDRESS,
                              KETCUBE_LIS2HH12_CTRL_REG3, &i2cByte, 1)) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Initialise the LIS2HH12 sensor
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_lis2hh12_UnInit(void)
{
    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Read data from LIS2HH12 sensor
 *
 * @param buffer pointer to buffer for storing the result of mesurement
 * @param len data len in bytes
 *
 * @retval KETCUBE_CFG_MODULE_OK in case of success
 * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
 */
ketCube_cfg_ModError_t ketCube_lis2hh12_ReadData(uint8_t * buffer,
                                                 uint8_t * len)
{

    int16_t data[3] = { 0 };
    ketCube_I2C_ReadData(KETCUBE_LIS2HH12_I2C_ADDRESS,
                         KETCUBE_LIS2HH12_OUT_X_L, (uint8_t *) & data, 6);
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LIS2HH12,
                                 "X = %6.4f; Y = %6.4f; Z = %6.4f",
                                 (float) (data[0] / 16383.5),
                                 (float) (data[1] / 16383.5),
                                 (float) (data[2] / 16383.5));

    if (data[2] > 11000) {      //Up-facing
        buffer[0] = 1;
        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LIS2HH12,
                                     "Facing: Up");
    } else if (data[2] < -11000) {      //Down-facing
        buffer[0] = 2;
        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LIS2HH12,
                                     "Facing: Down");
    } else if (data[1] > 11000) {       //Left-facing
        buffer[0] = 3;
        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LIS2HH12,
                                     "Facing: Left");
    } else if (data[1] < -11000) {      //Right-facing
        buffer[0] = 4;
        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LIS2HH12,
                                     "Facing: Right");
    } else if (data[0] > 11000) {       //Back-facing
        buffer[0] = 5;
        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LIS2HH12,
                                     "Facing: Back");
    } else if (data[0] < -11000) {      //Front-facing
        buffer[0] = 6;
        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LIS2HH12,
                                     "Facing: Front");
    } else {                    //Unknown
        buffer[0] = 0;
        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LIS2HH12,
                                     "Facing: Unknown");
        ketCube_terminal_DebugPrintln("");
    }

    *len = 1;

    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Read Interrupt source from LIS2HH12 sensor
 *
 * @retval LIS2HH12 IG_SRC1 register contents
 */
uint8_t ketCube_lis2h12_Get_Int(void)
{
    uint8_t i2cByte;
    ketCube_I2C_ReadData(KETCUBE_LIS2HH12_I2C_ADDRESS,
                         KETCUBE_LIS2HH12_INT1_SOURCE, &i2cByte, 1);
    return i2cByte;
}

#endif                          // KETCUBE_CFG_INC_MOD_LIS2HH12
