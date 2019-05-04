/**
 * @file    ketCube_eeprom.h
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-01-05
 * @brief   This file contains definitions for the ketCube EEPROM driver
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
#ifndef __KETCUBE_EEPROM_H
#define __KETCUBE_EEPROM_H

#include "stdint.h"

#define KETCUBE_EEPROM_NAME               "eeprom_drv"      ///< EEPROM driver name

/** @defgroup  KETCube_EEPROM KETCube EEPROM driver
  * @brief KETCube EEPROM driver for STM32L082
  *
  * @ingroup KETCube_CoreDrivers 
  * @{
  */

#define KETCUBE_EEPROM_BASE_ADDR  ((uint32_t)0x08080000)        /* Data EEPROM base address */
#define KETCUBE_EEPROM_END_ADDR   ((uint32_t)0x080807FF)        /* Data EEPROM end address */
#define KETCUBE_EEPROM_TIMEOUT    0x1000        /*<! Value of Timeout for EEPROM operations */

/**
* @brief  Error code type.
*/
typedef enum {
    KETCUBE_EEPROM_OK = 0,      /*<! Operation OK */
    KETCUBE_EEPROM_ERROR = 1,   /*<! EEPROM ERROR */
    ketCube_EEPROM_Error_tIMEOUT = 2,   /*<! EEPROM Timeout */
    KETCUBE_EEPROM_ERROR_MEMOVER = 3    /*<! EEPROM memory space overflow */
} ketCube_EEPROM_Error_t;

extern ketCube_EEPROM_Error_t ketCube_EEPROM_ReadBuffer(uint32_t addr,
                                                        uint8_t * data,
                                                        uint8_t len);
extern ketCube_EEPROM_Error_t ketCube_EEPROM_WriteBuffer(uint32_t addr,
                                                         uint8_t * data,
                                                         uint8_t len);
extern ketCube_EEPROM_Error_t ketCube_EEPROM_Erase(uint32_t addr,
                                                   uint8_t len);

/**
* @}
*/

#endif                          /* __KETCUBE_EEPROM_H */
