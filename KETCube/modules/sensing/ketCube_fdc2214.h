/**
 * @file    ketCube_fdc2214.h
 * @author  Jan Belohoubek
 * @version alpha
 * @date    2019-01-01
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
#include "ketCube_mainBoard.h"
#include "ketCube_gpio.h"

/** @defgroup KETCube_FDC2214 KETCube FDC2214
  * @brief KETCube FDC2214 module
  * @ingroup KETCube_SensMods
  * @{
  */

/**
 * @brief FDC2214 Channels
 * 
 */
typedef enum {
    FDC2214_CH0  = 0x0,          ///< Channel 0
    FDC2214_CH1  = 0x1,          ///< Channel 1
    FDC2214_CH2  = 0x2,          ///< Channel 2
    FDC2214_CH3  = 0x3           ///< Channel 3
} ketCube_fdc2214_chan_t;

/**
 * @brief Channel Sequencing
 * 
 * @note the values are used to fdc2214 register sequencing. Do not modify!
 */
typedef enum {
    FDC2214_SINGLE_CH0  = 0x0,          ///< Enable single channel 0
    FDC2214_SINGLE_CH1  = 0x1,          ///< Enable single channel 1
    FDC2214_SINGLE_CH2  = 0x2,          ///< Enable single channel 2
    FDC2214_SINGLE_CH3  = 0x3,          ///< Enable single channel 3
    FDC2214_SEQ_CH_01   = (0x1 << 4),   ///< Sequence channels 0 and 2
    FDC2214_SEQ_CH_012  = (0x2 << 4),   ///< Sequence channels 0 and 1 and 2
    FDC2214_SEQ_CH_0123 = (0x3 << 4)    ///< Sequence channels 0 and 1 and 2 and 3
} ketCube_fdc2214_chanSeq_t;

/**
 * @brief Deglitch Filter
 */
typedef enum {
    FDC2214_DGLF_1   = 0x1,          ///< 1 MHz
    FDC2214_DGLF_3   = 0x4,          ///< 3.3 MHz
    FDC2214_DGLF_10  = 0x5,          ///< 10 MHz
    FDC2214_DGLF_33  = 0x7,          ///< 33 MHz
} ketCube_fdc2214_dglFilter_t;

/**
 * @brief Sensor type
 */
typedef enum {
    FDC2214_SENSTYPE_DIFFERENTIAL,      ///< Differential sensor
    FDC2214_SENSTYPE_SINGLE_ENDED       ///< Single-ended sensor
} ketCube_fdc2214_sensType_t;

/**
* @addtogroup KETCUBE_FDC2214_Registers
* @{
*/

#define KETCUBE_FDC2214_DATA_CH0            0x00   ///< Channel 0 MSB Conversion Result and status
#define KETCUBE_FDC2214_DATA_LSB_CH0        0x01   ///< Channel 0 LSB Conversion Result and status
#define KETCUBE_FDC2214_DATA_CH1            0x02
#define KETCUBE_FDC2214_DATA_LSB_CH1        0x03
#define KETCUBE_FDC2214_DATA_CH2            0x04
#define KETCUBE_FDC2214_DATA_LSB_CH2        0x05
#define KETCUBE_FDC2214_DATA_CH3            0x06
#define KETCUBE_FDC2214_DATA_LSB_CH3        0x07

#define KETCUBE_FDC2214_RCOUNT_CH0          0x08   ///< Reference Count setting for Channel 0
#define KETCUBE_FDC2214_RCOUNT_CH1          0x09
#define KETCUBE_FDC2214_RCOUNT_CH2          0x0A
#define KETCUBE_FDC2214_RCOUNT_CH3          0x0B

#define KETCUBE_FDC2214_OFFSET_CH0          0x0C   ///< Reference Count setting for Channel 0
#define KETCUBE_FDC2214_OFFSET_CH1          0x0D
#define KETCUBE_FDC2214_OFFSET_CH2          0x0E
#define KETCUBE_FDC2214_OFFSET_CH3          0x0F

#define KETCUBE_FDC2214_SETTLECOUNT_CH0     0x10   ///< Channel 0 Settling Reference Count
#define KETCUBE_FDC2214_SETTLECOUNT_CH1     0x11
#define KETCUBE_FDC2214_SETTLECOUNT_CH2     0x12
#define KETCUBE_FDC2214_SETTLECOUNT_CH3     0x13

#define KETCUBE_FDC2214_CLOCK_DIVIDERS_CH0  0x14   ///< Reference divider settings for Channel 0
#define KETCUBE_FDC2214_CLOCK_DIVIDERS_CH1  0x15
#define KETCUBE_FDC2214_CLOCK_DIVIDERS_CH2  0x16
#define KETCUBE_FDC2214_CLOCK_DIVIDERS_CH3  0x17

#define KETCUBE_FDC2214_STATUS              0x18   //< Device status
#define KETCUBE_FDC2214_STATUS_CONFIG       0x19   //< Device status reporting config
#define KETCUBE_FDC2214_CONFIG              0x1A   //< Conversion config
#define KETCUBE_FDC2214_MUX_CONFIG          0x1B   //< Channelmultiplexing config
#define KETCUBE_FDC2214_RESET_DEV           0x1C   //< Reset device

#define KETCUBE_FDC2214_DRIVE_CURRENT_CH0   0x1E   //< Channel 0 sensor current drive configuration
#define KETCUBE_FDC2214_DRIVE_CURRENT_CH1   0x1F
#define KETCUBE_FDC2214_DRIVE_CURRENT_CH2   0x20
#define KETCUBE_FDC2214_DRIVE_CURRENT_CH3   0x21

#define KETCUBE_FDC2214_MANUFACTURER_ID     0x7E   ///< Manufacturer ID. For FDC2214: 0x5449
#define KETCUBE_FDC2214_DEVICE_ID           0x7F   ///< Device ID. For FDC2214: 0x3055

/**
* @}
*/


/** @defgroup KETCUBE_FDC2214_settingsn FDC2214 Settings
* @{
*/

/**
* @brief  I2C slave (FDC2214) address.
* @note See FDC2214 datasheet for I2C address options
*/
#define KETCUBE_FDC2214_I2C_ADDRESS  (uint8_t) (0x2A << 1)

#define KETCUBE_FDC2214_I2C_TIMEOUT         1000    ///< Timeout value for I2C communication [ms]
#define KETCUBE_FDC2214_I2C_WU              100     ///< Wake-up time [ms]
#define KETCUBE_FDC2214_I2C_TRY              50     ///< I2C try when not successfull 

#define FDC2214_SD_PORT                     KETCUBE_MAIN_BOARD_PIN_PWM_PORT   ///< FDC2214 Shut-Down
#define FDC2214_SD_PIN                      KETCUBE_MAIN_BOARD_PIN_PWM_PIN    ///< FDC2214 Shut-Down
#define FDC2214_INT_PORT                    KETCUBE_MAIN_BOARD_PIN_INT_PORT   ///< FDC2214 INT
#define FDC2214_INT_PIN                     KETCUBE_MAIN_BOARD_PIN_INT_PIN    ///< FDC2214 INT
#define FDC2214_LED_PORT                    KETCUBE_MAIN_BOARD_PIN_MOSI_PORT  ///< Indication LED
#define FDC2214_LED_PIN                     KETCUBE_MAIN_BOARD_PIN_MOSI_PIN   ///< Indication LED

#define FDC2214_USE_EXTERNAL_OSC            TRUE                              ///< Enable/Disable external oscillator
#define FDC2214_ENABLE_INT                  FALSE                             ///< Enable FDC2214 interrupt
#define FDC2214_ENABLE_SLEEP                TRUE                              ///< Enter sleep mode instead of shut-down when KETCube goes low-power mode

#define FDC2214_LED_INDICATION              TRUE                              ///< Enable LED indication
#define FDC2214_LED_THRESHOLD               1000                              ///< FDC2214 raw value difference used to indicate significant capacity difference by using LED
#define FDC2214_LED_CHAN                    FDC2214_CH0                       ///< FDC2214 channel used for LED indication, @see ketCube_fdc2214_chan_t

#define FDC2214_RCOUNT                      0xFFFF //0x0080                   ///< RCount - to set the meas time
#define FDC2214_SETTLECOUNT                 0x0400                            ///< SettleCount - to stabilize oscilaltions

#define FDC2214_CHAN_SEQ                    FDC2214_SINGLE_CH0                ///< Selected/active channel(s), @see ketCube_fdc2214_chanSeq_t
#define FDC2214_DGL_FILTER                  FDC2214_DGLF_10                   ///< Selected Deglitch filter, @see ketCube_fdc2214_dglFilter_t
#define FDC2214_SENSTYPE                    FDC2214_SENSTYPE_SINGLE_ENDED     ///< Selected sensor type, @see ketCube_fdc2214_sensType_t

/**
* @}
*/

/** @defgroup KETCUBE_FDC2214_fn Public Functions
* @{
*/

extern ketCube_cfg_ModError_t ketCube_fdc2214_Init(ketCube_InterModMsg_t
                                                   *** msg);
extern ketCube_cfg_ModError_t ketCube_fdc2214_SleepEnter(void);
extern ketCube_cfg_ModError_t ketCube_fdc2214_SleepExit(void);
extern ketCube_cfg_ModError_t ketCube_fdc2214_ReadData(uint8_t * buffer,
                                                       uint8_t * len);

/**
* @}
*/

/**
* @}
*/

#endif                          /* __KETCUBE_FDC2214_H */
