/**
 * @file    ketCube_cfg.h
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-01-05
 * @brief   This file contains definitions for the KETCube configuration
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
#ifndef __KETCUBE_CFG_H
#define __KETCUBE_CFG_H

#include "ketCube_common.h"
#include "ketCube_eeprom.h"

/** @defgroup  KETCube_cfg KETCube Configuration Manager
  * @brief KETCube Configuration Manager
  *
  * This KETCube module incorporates the static in-code and NVM (EEPROM) configuration management
  *
  * @ingroup KETCube_Core 
  * @{
  */


/** @defgroup KETCube_inc_mod Included KETCube Modules
  * Define/undefine to include/exclude KETCube modules
  * @{
  */

#define KETCUBE_CFG_INC_MOD_LORA            //< Include LoRa module; undef to disable module
#define KETCUBE_CFG_INC_MOD_DEBUGDISPLAY    //< Include SerialDisplay module; undef to disable module
#define KETCUBE_CFG_INC_MOD_HDC1080         //< Include HDC1080 module; undef to disable module
#define KETCUBE_CFG_INC_MOD_BATMEAS         //< Include batMeas module; undef to disable module
#define KETCUBE_CFG_INC_MOD_ADC             //< Include batMeas module; undef to disable module
#define KETCUBE_CFG_INC_MOD_STARNET         //< Include StarNet module(s); undef to disable module
#define KETCUBE_CFG_INC_MOD_FDC2214         //< Include FDC2214 module; undef to disable module
#define KETCUBE_CFG_INC_MOD_RXDISPLAY       //< Include RxDisplay module; undef to disable module
#define KETCUBE_CFG_INC_MOD_LEDBEACON       //< Include LED beacon module; undef to disable module
#define KETCUBE_CFG_INC_MOD_BUZZER          //< Include Buzzer module; undef to disable module
#define KETCUBE_CFG_INC_MOD_MAX1501         //< Include MAX1501 module; undef to disable module
#define KETCUBE_CFG_INC_MOD_LIS331HH        //< Include LIS331HH module; undef to disable module
#define KETCUBE_CFG_INC_MOD_MICRO_DOCK      //< Include KET's microDock module; undef to disable module

#define KETCUBE_CFG_INC_DRV_I2C             //< Include KET's I2C driver; undef to disable driver
#define KETCUBE_CFG_INC_DRV_MODBUS          //< Include KET's modBUS driver; undef to disable driver

/**
* @}
*/


/**
* @brief  List of module Indeces.
*/
typedef enum {
#ifdef KETCUBE_CFG_INC_MOD_LORA
    KETCUBE_LISTS_MODULEID_LORA,                        /*<! Module LoRa */
#endif

#ifdef KETCUBE_CFG_INC_MOD_DEBUGDISPLAY
    KETCUBE_LISTS_MODULEID_DEBUGDISPLAY,                /*<! Module DebugDisplay */
#endif

#ifdef KETCUBE_CFG_INC_MOD_HDC1080
    KETCUBE_LISTS_MODULEID_HDC1080,                     /*<! Module HDC1080 */
#endif

#ifdef KETCUBE_CFG_INC_MOD_BATMEAS
    KETCUBE_LISTS_MODULEID_BATMEAS,                     /*<! Module MeasBattery */
#endif

#ifdef KETCUBE_CFG_INC_MOD_ADC
    KETCUBE_LISTS_MODULEID_ADC,                         /*<! Module ADC */
#endif

#ifdef KETCUBE_CFG_INC_MOD_STARNET
    KETCUBE_LISTS_MODULEID_STARNET_CONCENTRATOR,        /*<! Module starNet */
    KETCUBE_LISTS_MODULEID_STARNET_NODE,                /*<! Module starNet */
#endif

#ifdef KETCUBE_CFG_INC_MOD_FDC2214
    KETCUBE_LISTS_MODULEID_FDC2214,         /*<! Module FDC2214 */
#endif

#ifdef KETCUBE_CFG_INC_MOD_RXDISPLAY
    KETCUBE_LISTS_MODULEID_RXDISPLAY,       /*<! Module RxDisplay */
#endif

#ifdef KETCUBE_CFG_INC_MOD_LEDBEACON
    KETCUBE_LISTS_MODULEID_LEDBEACON,       /*<! Module LEDBeacon */
#endif

#ifdef KETCUBE_CFG_INC_MOD_BUZZER
    KETCUBE_LISTS_MODULEID_BUZZER,          /*<! Module Buzzer */
#endif
    
#ifdef KETCUBE_CFG_INC_MOD_MAX1501
    KETCUBE_LISTS_MODULEID_MAX1501,         /*<! Module MAX1501 */
#endif
    
#ifdef KETCUBE_CFG_INC_MOD_LIS331HH
    KETCUBE_LISTS_MODULEID_LIS331HH,        /*<! Module LIS331HH */
#endif
    
#ifdef KETCUBE_CFG_INC_MOD_MICRO_DOCK
    KETCUBE_LISTS_MODULEID_MICRO_DOCK,      /*<! Module microDock */
#endif

    KETCUBE_LISTS_MODULEID_LAST             /*<! Last module index - do not modify! */
} ketCube_cfg_moduleIDs_t;

/**
* @brief  Allocated CFG EEPROM addreses
*/
typedef enum {
    KETCUBE_EEPROM_ALLOC_CORE = 0,      /*<! KETCube core configuration base address -- 64 bytes is reseved for core */
    KETCUBE_EEPROM_ALLOC_MODULES = 64,  /*<! Module configuration base address */
} ketCube_cfg_AllocEEPROM_t;

/**
* @brief  Length of CFG EEPROM data
*/
typedef enum {
    KETCUBE_EEPROM_LEN_MODULES = 1024,  /*<! Modules max configuration len in bytes */
} ketCube_cfg_LenEEPROM_t;

/**
* @brief  Error code type.
*/
typedef enum {
    KETCUBE_CFG_OK = 0,         /*<! Operation OK */
    KETCUBE_CFG_ERROR = 1,      /*<! CFG ERROR */
    ketCube_cfg_Load_ERROR = 2, /*<! Load CFG */
    ketCube_cfg_Save_ERROR = 3, /*<! Save CFG */
    KETCUBE_CFG_BUFF_SMALL = 4, /*<! Buffer is small */
    KETCUBE_CFG_INV_HEX = 5     /*<! Invalid HEX  */
} ketCube_cfg_Error_t;

/**
* @brief  Error code type.
*/
typedef enum {
    KETCUBE_CFG_MODULE_OK = (uint8_t) 0,
    KETCUBE_CFG_MODULE_ERROR = !(KETCUBE_CFG_MODULE_OK)
} ketCube_cfg_ModError_t;

/**
* @brief  KETCube inter-module message
*/
typedef struct ketCube_InterModMsg_t {
    uint8_t modID;              /*<! Module index */
    uint8_t msgLen;             /*<! Message length in bytes */
    uint8_t *msg;               /*<! Message body */
} ketCube_InterModMsg_t;

/**
* @brief Pointer to function returning ketCube_cfg_ModError_t
*/
typedef ketCube_cfg_ModError_t(*ketCube_cfg_ModVoidFn_t) (void);
typedef
ketCube_cfg_ModError_t(*ketCube_cfg_ModInitFn_t) (ketCube_InterModMsg_t ***
                                                  msg);
typedef ketCube_cfg_ModError_t(*ketCube_cfg_ModDataFn_t) (uint8_t * buffer, uint8_t * len);     //< Pointer to a function processing data of spec. length 
typedef ketCube_cfg_ModError_t(*ketCube_cfg_ModDataPtrFn_t) (ketCube_InterModMsg_t * msg);      //< Pointer to a function processing data of spec. length 

/**
* @brief  KETCube module configuration byte.
*/
typedef struct ketCube_cfg_ModuleCfgByte_t {
    bool enable:1;              /*<! Module enable */
    uint8_t RfMC:7;             /*<! Reserverd for Module Configuration */
} ketCube_cfg_ModuleCfgByte_t;

/**
* @brief  KETCube module definition.
*/
typedef struct ketCube_cfg_Module_t {
    char *name;                 /*<! Module name */
    char *descr;                /*<! Human-readable module description/help */
    ketCube_cfg_ModInitFn_t fnInit;     /*<! Module init function */
    ketCube_cfg_ModVoidFn_t fnSleepEnter;       /*<! DeInitialize module when entering sleep mode */
    ketCube_cfg_ModVoidFn_t fnSleepExit;        /*<! Initialize module when returning from sleep mode */
    ketCube_cfg_ModDataFn_t fnGetSensorData;    /*<! Module function to get module data into buffer (sensors) */
    ketCube_cfg_ModDataFn_t fnSendData; /*<! Module function to send data by communication module (the KETCube system period) */
    ketCube_cfg_ModVoidFn_t fnReceiveData;      /*<! Module function to initialize periodic data reception by using communication module */
    ketCube_cfg_ModDataPtrFn_t fnProcessMsg;    /*<! Module function to process data by this module */
    uint16_t cfgBase;           /*<! # Base address of module configuration */
    uint8_t cfgLen;             /*<! # of module configuration bytes: min = 1; max = 255; note that the first configuration byte is always set to TRUE when module is enabled and to FALSE when disabled (all bits are cleared) */
    ketCube_cfg_ModuleCfgByte_t cfgByte;        /*<! Module CFG byte */
} ketCube_cfg_Module_t;

extern ketCube_cfg_Error_t ketCube_cfg_Load(uint8_t * data,
                                            ketCube_cfg_moduleIDs_t id,
                                            ketCube_cfg_AllocEEPROM_t addr,
                                            ketCube_cfg_LenEEPROM_t len);
extern ketCube_cfg_Error_t ketCube_cfg_LoadStr(char *data, uint8_t buffLen,
                                               ketCube_cfg_moduleIDs_t id,
                                               ketCube_cfg_AllocEEPROM_t
                                               addr,
                                               ketCube_cfg_LenEEPROM_t
                                               len);
extern ketCube_cfg_Error_t ketCube_cfg_Save(uint8_t * data,
                                            ketCube_cfg_moduleIDs_t id,
                                            ketCube_cfg_AllocEEPROM_t addr,
                                            ketCube_cfg_LenEEPROM_t len);
extern ketCube_cfg_Error_t ketCube_cfg_SaveStr(char *data,
                                               ketCube_cfg_moduleIDs_t id,
                                               ketCube_cfg_AllocEEPROM_t
                                               addr,
                                               ketCube_cfg_LenEEPROM_t
                                               len);

/**
* @}
*/

#endif                          /* __KETCUBE_CFG_H */
