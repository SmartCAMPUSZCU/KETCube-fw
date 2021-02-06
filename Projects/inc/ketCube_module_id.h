/**
 * @file    ketCube_module_id.h
 * @author  Martin Ubl
 * @version 0.1
 * @date    2019-03-30
 * @brief   This file contains enumeration of all module IDs
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KETCUBE_MODULE_ID_H
#define __KETCUBE_MODULE_ID_H

/** @defgroup KETCube_mod_id Identifiers of all existing modules
  * @{
  */
  
/**
 * Module ID assignment rules
 *
 * There are 5 ID categories:
 *    1) 0          = reserved ID for KETCube core
 *    2) 1 - 127    = core modules, maintained by SmartCampus Team
 *    3) 128 - 1023 = upstream modlues, maintained by SmartCampusTeam
 *    4) 1024 - 65534 = 3rd party modules, empty in official upstream branch
 *    5) 65535      = invalid ID
 *
 * When the module is defined and implemented, it gets its ID assigned here.
 * The ID is a part of several core subsystems - e.g. terminal subsystem.
 * When the module gets a new major version released, and its interface changes,
 * a new ID needs to be assigned. When the interface definition does not change,
 * there's no need for new ID.
 *
 * It is typical to include both IDs amd module versions for a while to achieve
 * backwards compatibility. The old module goes to deprecated state and is to be
 * removed in upcoming firmware release
 */

/**
* @brief  List of global module identifiers
* These identifiers are used for e.g. remote terminal
*/
typedef enum {

    /* category 0 - reserved ID for core */

    KETCUBE_MODULEID_CORE = 0,              /*!< KETCube Core ID */

    /* category 1 - core modules - ID range 1 - 127 */

    // modules included as a part of core

    KETCUBE_MODULEID_DRIVERS_BEGIN          = 1,    /*!< All drivers */
    KETCUBE_MODULEID_LOCAL_TERMINAL         = 2,    /*!< Local terminal */
    KETCUBE_MODULEID_REMOTE_TERMINAL        = 3,    /*!< Remote terminal */
    KETCUBE_MODULEID_CORE_API               = 4,    /*!< Core API ID/version */

    /* category 2 - upstream modules - ID range 128 - 1023 */

    // modules included as a callable module (not a part of core)
    // these modules are maintained by the SmartCampus Team only

    KETCUBE_MODULEID_UPSTREAM_BEGIN         = 128,  /*!< Upstream modules BEGIN */
    KETCUBE_MODULEID_LORA                   = 128,  /*!< Module LoRa */
    KETCUBE_MODULEID_DEBUGDISPLAY           = 129,  /*!< Module DebugDisplay */
    KETCUBE_MODULEID_HDCX080                = 130,  /*!< Module HDCX080: HDC1080/HDC2080 share the common API! */
    KETCUBE_MODULEID_BATMEAS                = 131,  /*!< Module MeasBattery */
    KETCUBE_MODULEID_ADC                    = 132,  /*!< Module ADC */
    KETCUBE_MODULEID_STARNET_CONCENTRATOR   = 133,  /*!< Module starNet */
    KETCUBE_MODULEID_STARNET_NODE           = 134,  /*!< Module starNet */
    KETCUBE_MODULEID_RXDISPLAY              = 135,  /*!< Module RxDisplay */
    KETCUBE_MODULEID_ASYNCTX                = 136,  /*!< Module AsyncTx */
    KETCUBE_MODULEID_TXDISPLAY              = 137,  /*!< Module TxDisplay */
    KETCUBE_MODULEID_BMEX80                 = 138,  /*!< Module BMEx80 */
    KETCUBE_MODULEID_LIS2HH12               = 139,  /*!< Module LIS2HH12 */
    KETCUBE_MODULEID_ICS43432               = 140,  /*!< Module ICS43432 */
    KETCUBE_MODULEID_TEST_RADIO             = 141,  /*!< Module testRadio */
    KETCUBE_MODULEID_UART2WAN               = 142,  /*!< Module uart2WAN */
    KETCUBE_MODULEID_ARDUINO                = 143,  /*!< Module Arduino */

    /* category 3 - third party modules - ID range 1024 - 65534 */
    
    KETCUBE_MODULEID_3RDPARTY_BEGIN         = 1024, /*!< 3rd party modules BEGIN */

    // modules maintained by forks of original repository;
    // this category should remain empty in main repository

    //KETCUBE_MODULEID_MY_OWN_SENSOR        = 1024,

    // modules generated by helper scripts are placed here
    // please do not insert any ID's between BEGIN - END
    // it is recommended to move autogenerated IDs 
    // to other location and define ID explicitly
    KETCUBE_MODULEID_AUTOGENERATED_BEGIN    = 16384, /*!< 3rd party autogenerated; modules BEGIN */
    
    KETCUBE_MODULEID_AUTOGENERATED_END      = 20480, /*!< 3rd party autogenerated; modules END */
    
    /* category 4 - non-module IDs - ID 65535 */

    KETCUBE_MODULEID_INVALID                = 65535, /*!< Invalid module ID */

} ketCube_moduleID_t;

/**
* @}
*/

#endif

