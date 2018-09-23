/**
 * @file    ketCube_modules.h
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-01-05
 * @brief   This file aggregates the KETCube module management
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


 /* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __KETCUBE_MODULES_H
#define __KETCUBE_MODULES_H

#include "ketCube_cfg.h"

/** @defgroup  KETCube_Modules KETCube Modules
  * @brief KETCube Modules
  *
  * This KETCube modules implements the module support for KETCube
  *
  * @ingroup KETCube_Core 
  * @{
  */

#define KETCUBE_MODULES_SENSOR_BYTES  512       //< Max number of bytes which can be read from all sensors
#define ketCube_modules_CNT  (KETCUBE_LISTS_MODULEID_LAST)



extern ketCube_cfg_Module_t ketCube_modules_List[ketCube_modules_CNT];
extern ketCube_cfg_Error_t ketCube_modules_Init(void);
extern ketCube_cfg_Error_t ketCube_modules_ExecutePeriodic(void);
extern ketCube_cfg_Error_t ketCube_modules_ProcessMsgs(void);
extern ketCube_cfg_Error_t ketCube_modules_SleepEnter(void);
extern ketCube_cfg_Error_t ketCube_modules_SleepExit(void);

/**
* @}
*/

#endif                          /* KETCUBE_MODULES_H */
