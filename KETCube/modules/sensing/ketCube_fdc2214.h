/**
 * @file    ketCube_fdc2214.h
 * @author  Jan Belohoubek
 * @version pre-alpha
 * @date    2018-03-18
 * @brief   This file contains definitions for the FDC2214 driver
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
#ifndef __KETCUBE_FDC2214_H
#define __KETCUBE_FDC2214_H


#include "ketCube_cfg.h"
#include "ketCube_common.h"

/** @defgroup KETCube_FDC2214 KETCube FDC2214
  * @brief KETCube FDC2214 module
  * @ingroup KETCube_SensMods
  * @{
  */

/**
* @brief  I2C slave (FDC2214) address.
* @note Watch FDC2214 datasheet for I2C address options
*/
#define KETCUBE_FDC2214_I2C_ADDRESS  (uint8_t) (0x2A << 1)


#define KETCUBE_FDC2214_I2C_TIMEOUT 0x1000      /*<! Timeout value for I2C communication */
#define KETCUBE_FDC2214_I2C_TRY        200      /*<! I2C try when not successfull */

/**
* @addtogroup KETCUBE_FDC2214_Registers
* @{
*/

#define KETCUBE_FDC2214_DATA_CH0      		  0x00  //< Channel 0 MSB Conversion Result and status
#define KETCUBE_FDC2214_DATA_LSB_CH0		    0x01        //< Channel 0 LSB Conversion Result and status
#define KETCUBE_FDC2214_DATA_CH1      		  0x02
#define KETCUBE_FDC2214_DATA_LSB_CH1		    0x03
#define KETCUBE_FDC2214_DATA_CH2      		  0x04
#define KETCUBE_FDC2214_DATA_LSB_CH2		    0x05
#define KETCUBE_FDC2214_DATA_CH3      		  0x06
#define KETCUBE_FDC2214_DATA_LSB_CH3		    0x07

#define KETCUBE_FDC2214_RCOUNT_CH0  		    0x08        //< Reference Count setting for Channel 0
#define KETCUBE_FDC2214_RCOUNT_CH1  		    0x09
#define KETCUBE_FDC2214_RCOUNT_CH2  		    0x0A
#define KETCUBE_FDC2214_RCOUNT_CH3  		    0x0B

#define KETCUBE_FDC2214_OFFSET_CH0  		    0x0C        //< Reference Count setting for Channel 0
#define KETCUBE_FDC2214_OFFSET_CH1  		    0x0D
#define KETCUBE_FDC2214_OFFSET_CH2  		    0x0E
#define KETCUBE_FDC2214_OFFSET_CH3  		    0x0F

#define KETCUBE_FDC2214_SETTLECOUNT_CH0     0x10        //< Channel 0 Settling Reference Count
#define KETCUBE_FDC2214_SETTLECOUNT_CH1     0x11
#define KETCUBE_FDC2214_SETTLECOUNT_CH2     0x12
#define KETCUBE_FDC2214_SETTLECOUNT_CH3     0x13

#define KETCUBE_FDC2214_CLOCK_DIVIDERS_CH0  0x14        //< Reference divider settings for Channel 0
#define KETCUBE_FDC2214_CLOCK_DIVIDERS_CH1  0x15
#define KETCUBE_FDC2214_CLOCK_DIVIDERS_CH2  0x16
#define KETCUBE_FDC2214_CLOCK_DIVIDERS_CH3  0x17

#define KETCUBE_FDC2214_STATUS              0x18        // Device status
#define KETCUBE_FDC2214_STATUS_CONFIG       0x19        // Device status reporting config
#define KETCUBE_FDC2214_CONFIG              0x1A        // Conversion config
#define KETCUBE_FDC2214_MUX_CONFIG          0x1B        // Channelmultiplexing config
#define KETCUBE_FDC2214_RESET_DEV           0x1C        // Reset device

#define KETCUBE_FDC2214_DRIVE_CURRENT_CH0   0x1E        // Channel 0 sensor current drive configuration
#define KETCUBE_FDC2214_DRIVE_CURRENT_CH1   0x1F
#define KETCUBE_FDC2214_DRIVE_CURRENT_CH2   0x20
#define KETCUBE_FDC2214_DRIVE_CURRENT_CH3   0x21

#define KETCUBE_FDC2214_MANUFACTURER_ID     0x7E        //< Manufacturer ID. For FDC2214: 0x5449
#define KETCUBE_FDC2214_DEVICE_ID           0x7F        //< Device ID. For FDC2214: 0x3055

/**
* @}
*/


/** @defgroup KETCUBE_FDC2214_fn Public Functions
* @{
*/

extern ketCube_cfg_ModError_t ketCube_fdc2214_Init(ketCube_InterModMsg_t
                                                   *** msg);
extern ketCube_cfg_ModError_t ketCube_fdc2214_UnInit(void);
extern ketCube_cfg_ModError_t ketCube_fdc2214_ReadData(uint8_t * buffer,
                                                       uint8_t * len);

/**
* @}
*/

/**
* @}
*/

#endif                          /* __KETCUBE_FDC2214_H */
