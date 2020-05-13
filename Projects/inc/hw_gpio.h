/**
 * @file    hw_gpio.h
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2018-10-30
 * @brief   This is the wrapper for compatibility with the Semtech code
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



#ifndef __HW_GPIO_H__
#define __HW_GPIO_H__

#include "ketCube_cfg.h"
#include "ketCube_gpio.h"
   
   
typedef void( GpioIrqHandler )( void* context );

static inline void HW_GPIO_Init( ketCube_gpio_port_t GPIOx, uint16_t GPIO_Pin, GPIO_InitTypeDef* initStruct) {
     ketCube_GPIO_ReInit((ketCube_gpio_port_t) GPIOx, (ketCube_gpio_pin_t) GPIO_Pin, initStruct);
}

static inline void HW_GPIO_SetIrq( ketCube_gpio_port_t GPIOx, uint16_t GPIO_Pin, uint32_t prio,  GpioIrqHandler *irqHandler ) {
    ketCube_GPIO_SetIrq((ketCube_gpio_port_t) GPIOx, (ketCube_gpio_pin_t) GPIO_Pin, prio, irqHandler);
}

static inline void HW_GPIO_Write( ketCube_gpio_port_t GPIOx, uint16_t GPIO_Pin,  uint32_t value ) {
    ketCube_GPIO_Write((ketCube_gpio_port_t) GPIOx, (ketCube_gpio_pin_t) GPIO_Pin, (bool) value);
}

static inline uint32_t HW_GPIO_Read( ketCube_gpio_port_t GPIOx, uint16_t GPIO_Pin ) {
    return (uint32_t) ketCube_GPIO_Read((ketCube_gpio_port_t) GPIOx, (ketCube_gpio_pin_t) GPIO_Pin);
}


#endif /* __HW_GPIO_H__ */
