/**
 * @file    ketCube_radio.c
 * @author  Jan Belohoubek
 * @version alpha
 * @date    2019-12-09
 * @brief   This file contains the ketCube Radio driver
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


#include "radio.h"

#include "ketCube_cfg.h"
#include "ketCube_radio.h"
#include "ketCube_gpio.h"
#include "ketCube_spi.h"

#include "ketCube_terminal.h"

#include "sx1276Regs-Fsk.h"

static volatile bool initialized = FALSE; /* disable concurent execution of the init function body */

/**
 * @brief Set-UP Radio before sleep enter
 * 
 * @note This function should be called by KETCube core
 * 
 */
ketCube_cfg_DrvError_t ketCube_Radio_InitDriver(void) {
    Radio.IoInit();
    ketCube_SPI_Init();
    SX1276SetXO(SET);
    Radio.Sleep();
    SX1276SetXO(RESET);
    ketCube_SPI_DeInit();
    
    return KETCUBE_CFG_DRV_OK;
}

/**
  * @brief Initializes the Radio hardware
  * 
  */
ketCube_cfg_DrvError_t ketCube_Radio_Init(void) {
    if (initialized == TRUE) {
        ketCube_terminal_DriverSeverityPrintln(KETCUBE_RADIO_NAME, KETCUBE_CFG_SEVERITY_ERROR, "Unable to initialize Radio!");
        return KETCUBE_CFG_MODULE_ERROR;
    }

    Radio.IoInit();
    
    initialized = TRUE;
    
    return KETCUBE_CFG_MODULE_OK;
}

/**
  * @brief Deinitializes the Radio hardware
  */
ketCube_cfg_DrvError_t ketCube_Radio_DeInit(void) {
    if (initialized == TRUE) {
        // UnInit here ...
        Radio.IoDeInit();
    }
    
    initialized = FALSE;
    
    return KETCUBE_CFG_MODULE_OK;
}


/**
 * @brief Set-UP Radio befere sleep enter
 * 
 * @note This function should be called by KETCube core
 * 
 */
ketCube_cfg_DrvError_t ketCube_Radio_SleepEnter(void) {
    if (initialized == TRUE) {
        //Radio.IoDeInit(); // Interrupts must continue to work
    }
    
    return KETCUBE_CFG_MODULE_OK;
}

/**
 * @brief Set-UP radio after sleep exit
 * 
 * @note This function should be called by KETCube core
 * 
 */
ketCube_cfg_DrvError_t ketCube_Radio_SleepExit(void) {
    if (initialized == TRUE) {
        Radio.IoInit();
    }
    
    return KETCUBE_CFG_MODULE_OK;
}
