/**
 * @file    ketCube_compilation.h
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2018-10-31
 * @brief   This file contains defines for current KETCube compilation settings
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
#ifndef __KETCUBE_COMPILATION_H
#define __KETCUBE_COMPILATION_H

/** @defgroup  KETCube_compilation KETCube Compilation
  * @brief KETCube Compilation defines
  *
  * This File contains defines for current KETCube compilation settings
  *
  * @ingroup KETCube_Core 
  * @{
  */


#define KETCUBE_CFG_DEV_NAME           "KETCube"  //< Device name

/** @defgroup KETCube_inc_mod Included KETCube Modules
  * Define/undefine to include/exclude KETCube modules
  * @{
  */

#define KETCUBE_CFG_INC_MOD_LORA        //< Include LoRa module; undef to disable module
#define KETCUBE_CFG_INC_MOD_DEBUGDISPLAY        //< Include SerialDisplay module; undef to disable module
#define KETCUBE_CFG_INC_MOD_HDC1080     //< Include HDC1080 module; undef to disable module
#define KETCUBE_CFG_INC_MOD_BATMEAS     //< Include batMeas module; undef to disable module
#define KETCUBE_CFG_INC_MOD_ADC         //< Include batMeas module; undef to disable module
#define KETCUBE_CFG_INC_MOD_STARNET     //< Include StarNet module(s); undef to disable module
#define KETCUBE_CFG_INC_MOD_FDC2214     //< Include FDC2214 module; undef to disable module
#define KETCUBE_CFG_INC_MOD_RXDISPLAY   //< Include RxDisplay module; undef to disable module
#define KETCUBE_CFG_INC_MOD_ASYNCTX     //< Include RxDisplay module; undef to disable module

#define KETCUBE_CFG_INC_DRV_I2C         //< Include KET's I2C driver; undef to disable driver
#define KETCUBE_CFG_INC_DRV_MODBUS      //< Include KET's modBUS driver; undef to disable driver

/**
* @}
*/


/**
* @brief  List of module Indeces.
*/
typedef enum {
#ifdef KETCUBE_CFG_INC_MOD_LORA
    KETCUBE_LISTS_MODULEID_LORA,        /*<! Module LoRa */
#endif

#ifdef KETCUBE_CFG_INC_MOD_DEBUGDISPLAY
    KETCUBE_LISTS_MODULEID_DEBUGDISPLAY,        /*<! Module DebugDisplay */
#endif

#ifdef KETCUBE_CFG_INC_MOD_HDC1080
    KETCUBE_LISTS_MODULEID_HDC1080,     /*<! Module HDC1080 */
#endif

#ifdef KETCUBE_CFG_INC_MOD_BATMEAS
    KETCUBE_LISTS_MODULEID_BATMEAS,     /*<! Module MeasBattery */
#endif

#ifdef KETCUBE_CFG_INC_MOD_ADC
    KETCUBE_LISTS_MODULEID_ADC, /*<! Module ADC */
#endif

#ifdef KETCUBE_CFG_INC_MOD_STARNET
    KETCUBE_LISTS_MODULEID_STARNET_CONCENTRATOR,        /*<! Module starNet */
    KETCUBE_LISTS_MODULEID_STARNET_NODE,        /*<! Module starNet */
#endif

#ifdef KETCUBE_CFG_INC_MOD_FDC2214
    KETCUBE_LISTS_MODULEID_FDC2214,     /*<! Module FDC2214 */
#endif
    
#ifdef KETCUBE_CFG_INC_MOD_RXDISPLAY
    KETCUBE_LISTS_MODULEID_RXDISPLAY,   /*<! Module RxDisplay */
#endif

#ifdef KETCUBE_CFG_INC_MOD_ASYNCTX
    KETCUBE_LISTS_MODULEID_ASYNCTX,     /*<! Module AsyncTx */
#endif

    KETCUBE_LISTS_MODULEID_LAST /*<! Last module index - do not modify! */
} ketCube_cfg_moduleIDs_t;


/**
* @}
*/

#endif                          /* __KETCUBE_COMPILATION_H */