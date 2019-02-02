/**
 * @file    ketCube_i2c.h
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2018-07-12
 * @brief   This file contains definitions for the ketCube I2C driver
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
#ifndef __KETCUBE_I2C_H
#define __KETCUBE_I2C_H

#include "ketCube_cfg.h"

/** @defgroup KETCube_I2C KETCube I2C
  * @brief KETCube I2C module
  * @ingroup KETCube_ModuleDrivers
  * @{
  */

/** @defgroup KETCube_I2C_defs Public Deffinitions
  * @brief Public defines
  * @{
  */

#define KETCUBE_I2C_NAME                      "i2c_drv"         ///< I2C driver name
#define KETCUBE_I2C_ADDRESS                   (uint8_t)0x33     ///< KETCube I2C address
#define KETCUBE_I2C_HANDLE                     I2C1
#define KETCUBE_I2C_TIMEOUT                    0x5000           ///< The value of the maximal timeout for BUS waiting loops

/**
* @brief I2C Speed selection
*/
typedef enum {
    KETCUBE_I2C_SPEED_100KHZ = 0x10A13E56,      /*<! Analog Filter ON, Rise Time 400ns, Fall Time 100ns */
    KETCUBE_I2C_SPEED_400KHZ = 0x00B1112E       /*<! Analog Filter ON, Rise Time 250ns, Fall Time 100ns */
} ketCube_I2C_SPEED_t;

#define KETCUBE_I2C_CLK_ENABLE()               __I2C1_CLK_ENABLE()
#define KETCUBE_I2C_SCL_SDA_GPIO_CLK_ENABLE()  __GPIOB_CLK_ENABLE()
#define KETCUBE_I2C_FORCE_RESET()              __I2C1_FORCE_RESET()
#define KETCUBE_I2C_RELEASE_RESET()            __I2C1_RELEASE_RESET()
#define KETCUBE_I2C_SCL_SDA_AF                 GPIO_AF4_I2C1
#define KETCUBE_I2C_SCL_SDA_GPIO_PORT          GPIOB
#define KETCUBE_I2C_SCL_PIN                    GPIO_PIN_8
#define KETCUBE_I2C_SDA_PIN                    GPIO_PIN_9
#define KETCUBE_I2C_EV_IRQn                    I2C1_IRQn

/**
* @}
*/

/** @defgroup KETCube_I2C_fn Public Functions
  * @brief Public functions
  * @{
  */

extern ketCube_cfg_ModError_t ketCube_I2C_Init(void);
extern ketCube_cfg_ModError_t ketCube_I2C_UnInit(void);
extern uint8_t ketCube_I2C_ReadData(uint8_t Addr, uint8_t Reg,
                                    uint8_t * pBuffer, uint16_t Size);
extern uint8_t ketCube_I2C_WriteData(uint8_t Addr, uint8_t Reg,
                                     uint8_t * pBuffer, uint16_t Size);
extern uint8_t ketCube_I2C_WriteRawData(uint8_t Addr, uint8_t * pBuffer,
                                        uint16_t Size);
extern uint8_t ketCube_I2C_ReadRawData(uint8_t Addr, uint8_t * pBuffer,
                                       uint16_t Size);

extern ketCube_cfg_ModError_t ketCube_I2C_TexasWriteReg(uint8_t devAddr,
                                                        uint8_t regAddr,
                                                        uint16_t * data);

extern ketCube_cfg_ModError_t ketCube_I2C_TexasReadReg(uint8_t devAddr,
                                                       uint8_t RegAddr,
                                                       uint16_t * data);

extern ketCube_cfg_ModError_t ketCube_I2C_STMWriteSingle(uint8_t devAddr,
                                                         uint8_t regAddr,
                                                         uint8_t * data,
                                                         uint8_t try);

extern ketCube_cfg_ModError_t ketCube_I2C_STMReadSingle(uint8_t devAddr,
                                                        uint8_t RegAddr,
                                                        uint8_t * data,
                                                        uint8_t try);

extern ketCube_cfg_ModError_t ketCube_I2C_STMReadBlock(uint8_t devAddr,
                                                       uint8_t regAddr,
                                                       uint8_t * data,
                                                       uint8_t len,
                                                       uint8_t try);

extern ketCube_cfg_ModError_t ketCube_I2C_AnalogWriteReg(uint8_t devAddr,
                                                         uint8_t regAddr,
                                                         uint8_t data);
/**
* @}
*/

/**
* @}
*/

#endif                          /* __KETCUBE_I2C_H */
