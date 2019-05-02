/**
 * @file    ketCube_lora_ext.c
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2019-01-30
 * @brief   This file contains the KETCube LoRa extension and helper functions interacting directly with LoRaWAN stack, while added for/by the KETCube project
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

#include "ketCube_terminal.h"
#include "ketCube_lora_ext.h"

#include "Region.h"

/**
 * @brief Print OTAA Join-related info
 * 
 */
void ketCube_lora_OTAAJoin(ApplyCFListParams_t * applyCFList)
{
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "OTAA Joined!");
    ketCube_lora_PrintCFList(applyCFList);
}

/**
 * @brief Print Cf list as received in a LoRaWAN Join response
 * 
 */
void ketCube_lora_PrintCFList(ApplyCFListParams_t * applyCFList)
{
    uint32_t freq;
    uint8_t i;

    // Size of the optional CF list
    if(applyCFList->Size != 16) {
        ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "CFLIST :: size too small (%d).", applyCFList->Size);
        return;
    }

    // Last byte is RFU, don't take it into account
    for(i = 0; i < 15; i += 3) {
        freq = (uint32_t) applyCFList->Payload[i];
        freq |= ((uint32_t) applyCFList->Payload[i + 1] << 8);
        freq |= ((uint32_t) applyCFList->Payload[i + 2] << 16);
        freq *= 100;

        if(freq != 0) {
            // print freq
            ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "CFList :: New Freq: %d Hz", freq);
        }
    }
}