/**
 * @file    ketCube_lis2hh12.h
 * @author  Krystof Vanek
 * @version pre-alpha
 * @date    2018-11-05
 * @brief   This file contains definitions for the LIS2HH12 module
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

#ifndef __KETCUBE_LIS2HH12_H_
#define __KETCUBE_LIS2HH12_H_

/** @defgroup KETCube_LIS2HH12 KETCube LIS2HH12
  * @brief KETCube LIS2HH12 module
  * @ingroup KETCube_SensMods
  * @{
  */

/**
* @brief  I2C slave (LIS2HH12) address.
* @note See LIS2HH12 datasheet for I2C address options
*/
#define KETCUBE_LIS2HH12_I2C_ADDRESS  (uint8_t) (0x1D << 1)

#define KETCUBE_LIS2HH12_WHO_AM_I          0x41

/** @defgroup KETCube_LIS2HH12_defs Public Defines
  * @brief Public defines
  * @{
  */

typedef enum {
    KETCUBE_LIS2HH12_INT1_SRC_XLOW = 0x00U,
    KETCUBE_LIS2HH12_INT1_SRC_XHIGH = 0x01U,
    KETCUBE_LIS2HH12_INT1_SRC_YLOW = 0x02U,
    KETCUBE_LIS2HH12_INT1_SRC_YHIGH = 0x04U,
    KETCUBE_LIS2HH12_INT1_SRC_ZLOW = 0x08U,
    KETCUBE_LIS2HH12_INT1_SRC_ZHIGH = 0x10U
} ketCube_lis2hh12_Int1_Src_t;

/**
* @addtogroup KETCube_LIS2HH12_Registers LIS2HH12 Registers
* @{
*/
#define KETCUBE_LIS2HH12_TEMP_L            0x0BU
#define KETCUBE_LIS2HH12_TEMP_H            0x0CU
#define KETCUBE_LIS2HH12_WHO_AM_I_REG      0x0FU
#define KETCUBE_LIS2HH12_ACT_TSH           0x1EU
#define KETCUBE_LIS2HH12_ACT_DUR           0x1FU
#define KETCUBE_LIS2HH12_CTRL_REG1         0x20U        //< Main CTRL Register
#define KETCUBE_LIS2HH12_CTRL_REG2         0x21U
#define KETCUBE_LIS2HH12_CTRL_REG3         0x22U        //< Interrupt control
#define KETCUBE_LIS2HH12_CTRL_REG4         0x23U
#define KETCUBE_LIS2HH12_CTRL_REG5         0x24U
#define KETCUBE_LIS2HH12_CTRL_REG6         0x25U
#define KETCUBE_LIS2HH12_CTRL_REG7         0x26U
#define KETCUBE_LIS2HH12_STATUS_REG        0x27U
#define KETCUBE_LIS2HH12_OUT_X_L           0x28U
#define KETCUBE_LIS2HH12_OUT_X_H           0x29U
#define KETCUBE_LIS2HH12_OUT_Y_L           0x2AU
#define KETCUBE_LIS2HH12_OUT_Y_H           0x2BU
#define KETCUBE_LIS2HH12_OUT_Z_L           0x2CU
#define KETCUBE_LIS2HH12_OUT_Z_H           0x2DU
#define KETCUBE_LIS2HH12_FIFO_CTRL         0x2EU
#define KETCUBE_LIS2HH12_FIFO_SRC          0x2FU
#define KETCUBE_LIS2HH12_INT1_CFG          0x30U
#define KETCUBE_LIS2HH12_INT1_SOURCE       0x31U
#define KETCUBE_LIS2HH12_INT1_TSH_X1       0x32U
#define KETCUBE_LIS2HH12_INT1_TSH_Y1       0x33U
#define KETCUBE_LIS2HH12_INT1_TSH_Z1       0x34U
#define KETCUBE_LIS2HH12_INT1_DURATION     0x35U
#define KETCUBE_LIS2HH12_INT2_CFG          0x36U
#define KETCUBE_LIS2HH12_INT2_SOURCE       0x37U
#define KETCUBE_LIS2HH12_INT2_TSH          0x38U
#define KETCUBE_LIS2HH12_INT2_DURATION     0x39U
#define KETCUBE_LIS2HH12_INT2_XL_REF       0x3AU
#define KETCUBE_LIS2HH12_INT2_XH_REF       0x3BU
#define KETCUBE_LIS2HH12_INT2_YL_REF       0x3CU
#define KETCUBE_LIS2HH12_INT2_YH_REF       0x3DU
#define KETCUBE_LIS2HH12_INT2_ZL_REF       0x3EU
#define KETCUBE_LIS2HH12_INT2_ZH_REF       0x3FU

/**
* @}
*/

/**
* @addtogroup KETCube_LIS2HH12_CTRL1 LIS2HH12 CTRL1 flags
* @{
*/
#define KETCUBE_LIS2HH12_RESOLUTION_HIGH   0x01U<<7
#define KETCUBE_LIS2HH12_RESOLUTION_NORMAL 0x00U
#define KETCUBE_LIS2HH12_ODR_0_PDN         0x00U
#define KETCUBE_LIS2HH12_ODR_10Hz          0x01U<<4
#define KETCUBE_LIS2HH12_ODR_50Hz          0x02U<<4
#define KETCUBE_LIS2HH12_ODR_100Hz         0x03U<<4
#define KETCUBE_LIS2HH12_ODR_200Hz         0x04U<<4
#define KETCUBE_LIS2HH12_ODR_400Hz         0x05U<<4
#define KETCUBE_LIS2HH12_ODR_800Hz         0x06U<<4
#define KETCUBE_LIS2HH12_DATA_LATCH        0x01U<<3
#define KETCUBE_LIS2HH12_DATA_UPDATE       0x00U
#define KETCUBE_LIS2HH12_X_ENABLE          0x01U
#define KETCUBE_LIS2HH12_Y_ENABLE          0x01U<<1
#define KETCUBE_LIS2HH12_Z_ENABLE          0x01U<<2
/**
* @}
*/

/**
* @addtogroup KETCube_LIS2HH12_CTRL3 LIS2HH12 CTRL3 flags
* @{
*/
#define KETCUBE_LIS2HH12_FIFO_EN           0x01U << 7
#define KETCUBE_LIS2HH12_FIFO_DIS          0x00U
#define KETCUBE_LIS2HH12_FIFO_TSH_EN       0x01U << 6
#define KETCUBE_LIS2HH12_FIFO_TSH_DIS      0x00U
#define KETCUBE_LIS2HH12_INT1_INACT_EN     0x01U << 5
#define KETCUBE_LIS2HH12_INT1_INACT_DIS    0x00U
#define KETCUBE_LIS2HH12_INT1_IG2_EN       0x01U << 4
#define KETCUBE_LIS2HH12_INT1_IG2_DIS      0x00U
#define KETCUBE_LIS2HH12_INT1_IG1_EN       0x01U << 3
#define KETCUBE_LIS2HH12_INT1_IG1_DIS      0x00U
#define KETCUBE_LIS2HH12_INT1_OVR_EN       0x01U << 2
#define KETCUBE_LIS2HH12_INT1_OVR_DIS      0x00U
#define KETCUBE_LIS2HH12_INT1_FTH_EN       0x01U << 1
#define KETCUBE_LIS2HH12_INT1_FTH_DIS      0x00U
#define KETCUBE_LIS2HH12_INT1_DRDY_EN      0x01U
#define KETCUBE_LIS2HH12_INT1_DRDY_DIS     0x00U
/**
* @}
*/

/**
* @addtogroup KETCube_LIS2HH12_IG_CFG1 LIS2HH12 IG_CFG1 flags
* @{
*/
#define KETCUBE_LIS2HH12_AOI_EN           0x01U << 7
#define KETCUBE_LIS2HH12_AOI_DIS          0x00U
#define KETCUBE_LIS2HH12_6D_EN            0x01U << 6
#define KETCUBE_LIS2HH12_6D_DIS           0x00U
#define KETCUBE_LIS2HH12_ZHIGH_EN         0x01U << 5
#define KETCUBE_LIS2HH12_ZHIGH_DIS        0x00U
#define KETCUBE_LIS2HH12_ZLOW_EN          0x01U << 4
#define KETCUBE_LIS2HH12_ZLOW_DIS         0x00U
#define KETCUBE_LIS2HH12_YHIGH_EN         0x01U << 3
#define KETCUBE_LIS2HH12_YHIGH_DIS        0x00U
#define KETCUBE_LIS2HH12_YLOW_EN          0x01U << 2
#define KETCUBE_LIS2HH12_YLOW_DIS         0x00U
#define KETCUBE_LIS2HH12_XHIGH_EN         0x01U << 1
#define KETCUBE_LIS2HH12_XHIGH_DIS        0x00U
#define KETCUBE_LIS2HH12_XLOW_EN          0x01U
#define KETCUBE_LIS2HH12_XLOW_DIS         0x00U
/**
* @}
*/

/**
* @}
*/

/** @defgroup KETCube_SCD30_fn Public Functions
  * @brief Public functions
  * @{
  */

extern ketCube_cfg_ModError_t ketCube_lis2hh12_Init(ketCube_InterModMsg_t
                                                    *** msg);
extern uint8_t ketCube_lis2h12_Get_Int(void);
extern ketCube_cfg_ModError_t ketCube_lis2hh12_UnInit(void);
extern ketCube_cfg_ModError_t ketCube_lis2hh12_ReadData(uint8_t * buffer,
                                                        uint8_t * len);

/**
* @}
*/

/**
* @}
*/

#endif                          /* __KETCUBE_LIS2HH12_H_ */
