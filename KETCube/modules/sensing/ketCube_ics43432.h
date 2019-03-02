/**
 * @file    ketCube_ics43432.h
 * @author  Krystof Vanek
 * @version pre-alpha
 * @date    2018-11-18
 * @brief   This file contains definitions for the ICS43432 microphone module
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

#ifndef __KETCUBE_ICS43432_H_
#define __KETCUBE_ICS43432_H_

#include "ketCube_coreCfg.h"

/**
* @brief  KETCube module configuration
*/
typedef struct ketCube_ics43432_moduleCfg_t {
    ketCube_cfg_ModuleCfgByte_t coreCfg;           /*!< KETCube core cfg byte */
} ketCube_ics43432_moduleCfg_t;

extern ketCube_ics43432_moduleCfg_t ketCube_ics43432_moduleCfg;

#define KETCUBE_ICS43432_10_dBSPL        27U
#define KETCUBE_ICS43432_20_dBSPL        84U
#define KETCUBE_ICS43432_30_dBSPL       265U
#define KETCUBE_ICS43432_40_dBSPL       839U
#define KETCUBE_ICS43432_50_dBSPL      2653U
#define KETCUBE_ICS43432_60_dBSPL      8389U
#define KETCUBE_ICS43432_70_dBSPL     26527U
#define KETCUBE_ICS43432_80_dBSPL     83886U
#define KETCUBE_ICS43432_90_dBSPL    265271U
#define KETCUBE_ICS43432_100_dBSPL   838861U
#define KETCUBE_ICS43432_110_dBSPL  2652711U
#define KETCUBE_ICS43432_120_dBSPL  8388608U


#define KETCUBE_ICS43432_NOISE_LED_PORT       GPIOB
#define KETCUBE_ICS43432_NOISE_LED_PIN        GPIO_PIN_2

#define KETCUBE_ICS43432_MAX_SAMPLE_COUNT     KETCUBE_I2S_SAMPLE_RATE * KETCUBE_CORECFG_MIN_BASEPERIOD / 1000

extern ketCube_cfg_ModError_t ketCube_ics43432_Init(ketCube_InterModMsg_t
                                                    *** msg);
extern ketCube_cfg_ModError_t ketCube_ics43432_UnInit(void);
extern ketCube_cfg_ModError_t ketCube_ics43432_ReadData(uint8_t * buffer,
                                                        uint8_t * len);
extern void SPI2_IRQHandler(void);

#endif                          /* __KETCUBE_ICS43432_H_ */
