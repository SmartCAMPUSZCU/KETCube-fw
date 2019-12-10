/**
 * @file    ketCube_spi.h
 * @author  Jan Belohoubek
 * @version alpha
 * @date    2019-12-09
 * @brief   This file contains definitions for the ketCube SPI driver
 *
 * @note This code is based on Semtech and STM SPI driver implementation. 
 * See the original file licenses in LICENSE_SEMTECH and LICENSE_STM respectively.
 * 
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 University of West Bohemia in Pilsen
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


#ifndef __KETCUBE_SPI_H__
#define __KETCUBE_SPI_H__

#include "ketCube_cfg.h"

/** @defgroup KETCube_SPI KETCube SPI driver
  * @brief KETCube SPI driver
  * @ingroup KETCube_ModuleDrivers
  * @{
  */

/** @defgroup KETCube_SPI_fn Public Functions
  * @brief Public functions
  * @{
  */


extern ketCube_cfg_DrvError_t ketCube_SPI_Init(void);
extern ketCube_cfg_DrvError_t ketCube_SPI_DeInit(void);
extern uint16_t ketCube_SPI_InOut(uint16_t txData);


/**
* @}
*/

/**
* @}
*/


#endif  /* __KETCUBE_SPI_H__ */









