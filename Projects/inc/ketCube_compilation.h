/**
 * @file    ketCube_compilation.h
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2018-10-31
 * @brief   This file contains defines for current KETCube compilation settings
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2018 - 2020 University of West Bohemia in Pilsen
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

/**
 * @brief LoRaWAN Protocol versions
 */
#define KETCUBE_LORA_LRWAN_VERSION_V10x    1     /*!< LoRaWAN Spec. 1.0.3 */
#define KETCUBE_LORA_LRWAN_VERSION_V11x    2     /*!< LoRaWAN Spec. 1.1.0 */

/** @defgroup  KETCube_compilation KETCube Compilation
  * @brief KETCube Compilation defines
  *
  * This File contains defines for current KETCube compilation settings
  *
  * @ingroup KETCube_Core 
  * @{
  */

#define KETCUBE_CFG_DEV_NAME           "KETCube"        //< Device name
#define KETCUBE_LORA_LRWAN_VERSION	   KETCUBE_LORA_LRWAN_VERSION_V10x

/**
 * @brief Predefined Build ID symbol
 * 
 */
#ifndef KETCUBE_BUILD_ID
#define KETCUBE_BUILD_ID    "unknownBuildID"
#endif

/**
 * @brief Predefined Version symbol
 * 
 */
#ifndef KETCUBE_VERSION
#define KETCUBE_VERSION    "unknownVersion"
#endif


/**
 * @brief Enable/Disable Fast weke up
 * 
 * when fast wake up is enabled, the mcu wakes up in ~20us  * and 
 * does not wait for the VREFINT to be settled. THis is ok for 
 * most of the case except when adc must be used in this case before 
 * starting the adc, you must make sure VREFINT is settled
 */
#define KETCUBE_ENABLE_FAST_WAKEUP

/**
 * @brief Enable/Disable programmable Voltage Detector (PVD)
 * 
 * @note If PVD is enabled, additional power consumption arises
 * @note PVD is disabled by default
 * 
 */
// #define KETCUBE_ENABLE_PVD

/**
 * @brief Enable/Disable watchdog
 * 
 * If enabled, watchdog checks the program flow. The watchdog reset is performed after every wake-up in the main loop.
 * 
 * @note the (max) wake-up period is shortened to less than 15s (@see KETCUBE_MCU_WD_SAFE_TIMER_CNT) if watchdog is used!
 * 
 */
#define KETCUBE_ENABLE_WD

/** @defgroup KETCube_inc_mod Included KETCube Modules
  * Define/undefine to include/exclude KETCube modules
  * 
  * @note the following defines are defined in the Makefile on the per-project basis, see the KETCUBE_USE_MAKEFILE_MOD_DEFINES macro
  * 
  * @{
  */

#ifndef KETCUBE_USE_MAKEFILE_MOD_DEFINES

#define KETCUBE_CFG_INC_MOD_LORA        ///< Include LoRa module; undef to disable module
//#define KETCUBE_CFG_INC_MOD_DEBUGDISPLAY        ///< Include SerialDisplay module; undef to disable module; this module is deprecated and will be (completely) removed in the next release
#define KETCUBE_CFG_INC_MOD_HDCX080     ///< Include HDCx080 module; undef to disable module
#define KETCUBE_CFG_INC_MOD_BATMEAS     ///< Include batMeas module; undef to disable module
#define KETCUBE_CFG_INC_MOD_ADC         ///< Include ADC module; undef to disable module
#define KETCUBE_CFG_INC_MOD_STARNET     ///< Include StarNet module(s); undef to disable module
#define KETCUBE_CFG_INC_MOD_RXDISPLAY   ///< Include RxDisplay module; undef to disable module
#define KETCUBE_CFG_INC_MOD_ASYNCTX     ///< Include AsyncTx module; undef to disable module
#define KETCUBE_CFG_INC_MOD_TXDISPLAY   ///< Include TxDisplay module; undef to disable module
#define KETCUBE_CFG_INC_MOD_BMEX80      ///< Include BMEx80 module; undef to disable module
#define KETCUBE_CFG_INC_MOD_LIS2HH12    ///< Include LIS2HH12 module; undef to disable module
#define KETCUBE_CFG_INC_MOD_ICS43432    ///< Include ICS43432 module; undef to disable module
#define KETCUBE_CFG_INC_MOD_TEST_RADIO  ///< Include testRadio module; undef to disable module
#define KETCUBE_CFG_INC_MOD_UART2WAN    ///< Include uart2WAN module; undef to disable module
#define KETCUBE_CFG_INC_MOD_ARDUINO     ///< Include Arduino module; undef to disable module

//#define KETCUBE_CFG_INC_MOD_DUMMY       ///< Autogenerated modules will be included here

#endif // KETCUBE_USE_MAKEFILE_MOD_DEFINES

#define KETCUBE_CFG_INC_DRV_AD          ///< Include KET's ADC driver; undef to disable driver
#define KETCUBE_CFG_INC_DRV_I2C         ///< Include KET's I2C driver; undef to disable driver
#define KETCUBE_CFG_INC_DRV_I2S         ///< Include KET's I2S driver; undef to disable driver
#define KETCUBE_CFG_INC_DRV_BUZZER      ///< Include buzzer driver; undef to disable driver

/**
* @}
*/


/**
* @brief  List of module Indeces.
*/
typedef enum {
    KETCUBE_LISTS_MODULEID_FIRST = 1,             /*!< First module index - do not modify! */
    KETCUBE_LISTS_ID_CORE = 0,                    /*!< KETCube Core ID - do not modify! */
    
    /* First module has ID 1 */
    
#ifdef KETCUBE_CFG_INC_MOD_LORA
    KETCUBE_LISTS_MODULEID_LORA,                  /*!< Module LoRa */
#endif

#ifdef KETCUBE_CFG_INC_MOD_DEBUGDISPLAY
    KETCUBE_LISTS_MODULEID_DEBUGDISPLAY,          /*!< Module DebugDisplay */
#endif

#ifdef KETCUBE_CFG_INC_MOD_HDCX080
    KETCUBE_LISTS_MODULEID_HDCX080,               /*!< Module HDCX080 */
#endif

#ifdef KETCUBE_CFG_INC_MOD_BATMEAS
    KETCUBE_LISTS_MODULEID_BATMEAS,               /*!< Module MeasBattery */
#endif

#ifdef KETCUBE_CFG_INC_MOD_ADC
    KETCUBE_LISTS_MODULEID_ADC,                   /*!< Module ADC */
#endif

#ifdef KETCUBE_CFG_INC_MOD_STARNET
    KETCUBE_LISTS_MODULEID_STARNET_CONCENTRATOR,  /*!< Module starNet */
    KETCUBE_LISTS_MODULEID_STARNET_NODE,          /*!< Module starNet */
#endif

#ifdef KETCUBE_CFG_INC_MOD_RXDISPLAY
    KETCUBE_LISTS_MODULEID_RXDISPLAY,             /*!< Module RxDisplay */
#endif

#ifdef KETCUBE_CFG_INC_MOD_ASYNCTX
    KETCUBE_LISTS_MODULEID_ASYNCTX,               /*!< Module AsyncTx */
#endif

#ifdef KETCUBE_CFG_INC_MOD_TXDISPLAY
    KETCUBE_LISTS_MODULEID_TXDISPLAY,             /*!< Module TxDisplay */
#endif

#ifdef KETCUBE_CFG_INC_MOD_BMEX80
    KETCUBE_LISTS_MODULEID_BMEX80,                /*!< Module BMEx80 */
#endif

#ifdef KETCUBE_CFG_INC_MOD_LIS2HH12
    KETCUBE_LISTS_MODULEID_LIS2HH12,              /*!< Module LIS2HH12 */
#endif

#ifdef KETCUBE_CFG_INC_MOD_ICS43432
    KETCUBE_LISTS_MODULEID_ICS43432,              /*!< Module ICS43432 */
#endif

#ifdef KETCUBE_CFG_INC_MOD_TEST_RADIO
    KETCUBE_LISTS_MODULEID_TEST_RADIO,            /*!< Module testRadio */
#endif
    
#ifdef KETCUBE_CFG_INC_MOD_UART2WAN
    KETCUBE_LISTS_MODULEID_UART2WAN,              /*!< Module uart2WAN */
#endif
    
#ifdef KETCUBE_CFG_INC_MOD_ARDUINO
    KETCUBE_LISTS_MODULEID_ARDUINO,              /*!< Module Arduino */
#endif

    KETCUBE_LISTS_MODULEID_LAST                   /*!< Last module index - do not modify! */
} ketCube_cfg_moduleIDs_t;


/**
* @}
*/

#endif                          /* __KETCUBE_COMPILATION_H */
