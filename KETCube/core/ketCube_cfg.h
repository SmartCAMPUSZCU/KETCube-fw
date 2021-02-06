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
#ifndef DESKTOP_BUILD
#include "ketCube_eeprom.h"
#endif
#include "ketCube_compilation.h"
#include "ketCube_module_id.h"

/** @defgroup  KETCube_cfg KETCube Configuration Manager
  * @brief KETCube Configuration Manager
  *
  * This KETCube module incorporates the static in-code and NVM (EEPROM) configuration management
  *
  * @ingroup KETCube_Core 
  * @{
  */

/**
* @brief  Allocated CFG EEPROM addreses
*/
typedef enum {
    KETCUBE_EEPROM_ALLOC_CORE    = 0,    /*!< KETCube core configuration base address -- core configuration is located in module configuration area */
    KETCUBE_EEPROM_ALLOC_MODULES = 0,    /*!< Module configuration base address */
    KETCUBE_EEPROM_ALLOC_LAST    = 2048, /*!< Module configuration base address */
} ketCube_cfg_AllocEEPROM_t;

/**
* @brief  Length of CFG EEPROM data
* 
* @deprecated This enum provides no real borders any more
* 
*/
typedef enum {
    KETCUBE_EEPROM_LEN_CORE    = 64,    /*!< Core max configuration len in bytes */
    KETCUBE_EEPROM_LEN_MODULES = 1024,  /*!< Modules max configuration len in bytes */
} ketCube_cfg_LenEEPROM_t;

/**
* @brief  Error code type.
*/
typedef enum {
    KETCUBE_CFG_OK = 0,         /*!< Operation OK */
    KETCUBE_CFG_ERROR = 1,      /*!< CFG ERROR */
    ketCube_cfg_Load_ERROR = 2, /*!< Load CFG */
    ketCube_cfg_Save_ERROR = 3, /*!< Save CFG */
    KETCUBE_CFG_BUFF_SMALL = 4, /*!< Buffer is small */
    KETCUBE_CFG_INV_HEX = 5     /*!< Invalid HEX  */
} ketCube_cfg_Error_t;

/**
* @brief  Error code type.
*/
typedef enum {
    KETCUBE_CFG_MODULE_OK = (uint8_t) 0,
    KETCUBE_CFG_MODULE_ERROR = !(KETCUBE_CFG_MODULE_OK)
} ketCube_cfg_ModError_t;

/**
* @brief  Error code type.
*/
typedef enum {
    KETCUBE_CFG_DRV_OK = (uint8_t) 0,
    KETCUBE_CFG_DRV_ERROR = !(KETCUBE_CFG_MODULE_OK)
} ketCube_cfg_DrvError_t;

/**
* @brief  KETCube inter-module message
*/
typedef struct ketCube_InterModMsg_t {
    uint8_t modID;              /*!< Module index */
    uint8_t msgLen;             /*!< Message length in bytes */
    uint8_t *msg;               /*!< Message body */
} ketCube_InterModMsg_t;

/**
* @brief  KETCube debug severity definition.
*/
typedef enum ketCube_severity_t {
    KETCUBE_CFG_SEVERITY_NONE  = 0x0,
    KETCUBE_CFG_SEVERITY_ERROR = 0x1,
    KETCUBE_CFG_SEVERITY_INFO  = 0x2,
    KETCUBE_CFG_SEVERITY_DEBUG = 0x3
} ketCube_severity_t;

extern const char * ketCube_severity_strAlias[4];

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
    bool enable:1;                      /*!< Module enable */
    ketCube_severity_t severity:2;      /*!< Module message severity level */
    uint8_t RFU:5;                      /*!< RFU */
} ketCube_cfg_ModuleCfgByte_t;

/**
* @brief  KETCube configuration variable descriptor
*/
typedef struct ketCube_cfg_varDescr_t {
    uint16_t moduleID;     ///< Module Index
    uint8_t offset;        ///< Variable offset in module configuration memory
    uint8_t size;          ///< Variable size in bytes
} ketCube_cfg_varDescr_t;

/**
* @brief  KETCube module definition.
*/
typedef struct ketCube_cfg_Module_t {
    char *name;                 /*!< Module name */
    char *descr;                /*!< Human-readable module description/help */
    ketCube_moduleID_t id;      /*!< Module global unique identifier */
    ketCube_cfg_ModInitFn_t fnInit;             /*!< Module init function */
    ketCube_cfg_ModVoidFn_t fnSleepEnter;       /*!< DeInitialize module when entering sleep mode */
    ketCube_cfg_ModVoidFn_t fnSleepExit;        /*!< Initialize module when returning from sleep mode */
    ketCube_cfg_ModDataFn_t fnGetSensorData;    /*!< Module function to get module data into buffer (sensors) */
    ketCube_cfg_ModDataFn_t fnSendData;         /*!< Module function to send data by communication module (the KETCube system period) */
    ketCube_cfg_ModVoidFn_t fnReceiveData;      /*!< Module function to initialize periodic data reception by using communication module */
    ketCube_cfg_ModDataPtrFn_t fnProcessMsg;    /*!< Module function to process data by this module */
    ketCube_cfg_ModuleCfgByte_t * cfgPtr;       /*!< Pointer to actual/running KETCube configuration */
    ketCube_cfg_LenEEPROM_t cfgLen;             /*!< # of module configuration bytes: min = 1; max = 255; note that the first configuration byte is always set to TRUE when module is enabled and to FALSE when disabled (all bits are cleared) */
    uint16_t EEpromBase;                        /*!< EEPROM base for module configuration */
} ketCube_cfg_Module_t;

/* Indicate pending events */
extern volatile bool KETCube_eventsProcessed;

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

extern ketCube_cfg_Error_t ketCube_cfg_SetDefaults(ketCube_cfg_moduleIDs_t id,
                                                   ketCube_cfg_AllocEEPROM_t addr,
                                                   ketCube_cfg_LenEEPROM_t len);

/**
* @}
*/

#endif                          /* __KETCUBE_CFG_H */
