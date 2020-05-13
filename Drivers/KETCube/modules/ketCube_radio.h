/**
 * @file    ketCube_radio.h
 * @author  Jan Belohoubek
 * @version alpha
 * @date    2019-12-09
 * @brief   This file contains definitions for the ketCube Radio driver
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
#ifndef __KETCUBE_RADIO_H
#define __KETCUBE_RADIO_H

#include "ketCube_cfg.h"
#include "ketCube_gpio.h"

/** @defgroup KETCube_Radio KETCube Radio driver
  * @brief KETCube Radio driver
  * @ingroup KETCube_ModuleDrivers
  * @{
  */

#define KETCUBE_RADIO_NAME               "radio_drv"         ///< Radio driver name

typedef enum {
    RADIO_RESET_PIN   =  KETCUBE_GPIO_PIN_0,
    RADIO_MOSI_PIN    =  KETCUBE_GPIO_PIN_7,
    RADIO_MISO_PIN    =  KETCUBE_GPIO_PIN_6,
    RADIO_SCLK_PIN    =  KETCUBE_GPIO_PIN_3,
    RADIO_NSS_PIN     =  KETCUBE_GPIO_PIN_15,
    RADIO_DIO_0_PIN   =  KETCUBE_GPIO_PIN_4,
    RADIO_DIO_1_PIN   =  KETCUBE_GPIO_PIN_1,
    RADIO_DIO_2_PIN   =  KETCUBE_GPIO_PIN_0,
    RADIO_DIO_3_PIN   =  KETCUBE_GPIO_PIN_13,
#ifdef RADIO_DIO_4 
    RADIO_DIO_4_PIN   =  KETCUBE_GPIO_PIN_5,
#endif
#ifdef RADIO_DIO_5 
    RADIO_DIO_5_PIN   =  KETCUBE_GPIO_PIN_4,
#endif
    RADIO_TCXO_VCC_PIN             =   KETCUBE_GPIO_PIN_12,
    RADIO_ANT_SWITCH_PIN_RX        =   KETCUBE_GPIO_PIN_1,
    RADIO_ANT_SWITCH_PIN_TX_BOOST  =   KETCUBE_GPIO_PIN_1,
    RADIO_ANT_SWITCH_PIN_TX_RFO    =   KETCUBE_GPIO_PIN_2
} ketCube_radio_pin_t;


typedef enum {
    RADIO_RESET_PORT  =  KETCUBE_GPIO_PC,
    RADIO_MOSI_PORT   =  KETCUBE_GPIO_PA,
    RADIO_MISO_PORT   =  KETCUBE_GPIO_PA,
    RADIO_SCLK_PORT   =  KETCUBE_GPIO_PB,
    RADIO_NSS_PORT    =  KETCUBE_GPIO_PA,
    RADIO_DIO_0_PORT  =  KETCUBE_GPIO_PB,
    RADIO_DIO_1_PORT  =  KETCUBE_GPIO_PB,
    RADIO_DIO_2_PORT  =  KETCUBE_GPIO_PB,
    RADIO_DIO_3_PORT  =  KETCUBE_GPIO_PC,
#ifdef RADIO_DIO_4 
    RADIO_DIO_4_PORT  =  KETCUBE_GPIO_PA,
#endif
#ifdef RADIO_DIO_5 
    RADIO_DIO_5_PORT  =  KETCUBE_GPIO_PA,
#endif
    RADIO_TCXO_VCC_PORT            =   KETCUBE_GPIO_PA,
    RADIO_ANT_SWITCH_PORT_RX       =   KETCUBE_GPIO_PA, /* CRF1 */
    RADIO_ANT_SWITCH_PORT_TX_BOOST =   KETCUBE_GPIO_PC, /* CRF3 */
    RADIO_ANT_SWITCH_PORT_TX_RFO   =   KETCUBE_GPIO_PC  /* CRF2 */
} ketCube_radio_port_t;


/** @defgroup KETCube_Radio_fn Public Functions
  * @brief Public functions
  * @{
  */

extern ketCube_cfg_DrvError_t ketCube_Radio_Init(void);
extern ketCube_cfg_DrvError_t ketCube_Radio_DeInit(void);

/**
* @}
*/

/**
* @}
*/

#endif       // __KETCUBE_RADIO_H
