/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: contains hardaware configuration Macros and Constants

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
 /******************************************************************************
  * @file    hw_conf.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    27-February-2017
  * @brief   contains hardware configuration Macros and Constants
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
	*
	* @author  Jan Belohoubek
  * @version 0.1
  * @date    2018-01-03
	* @brief   This file has been modified to fit into the KETCube platform
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
	*
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONF_H__
#define __HW_CONF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#ifdef USE_STM32F0XX_NUCLEO
  #include "stm32f0xx_hal.h"
  #include "stm32f0xx_nucleo.h"
  #include "stm32f0xx_hal_conf.h"
  #error "create stm32f0xx_hw_conf.h "
#endif

#ifdef USE_STM32F1XX_NUCLEO
  #include "stm32f1xx_hal.h"
  #include "stm32f1xx_nucleo.h"
  #include "stm32f1xx_hal_conf.h"
  #error "create stm32f1xx_hw_conf.h "
#endif

#ifdef USE_STM32F3XX_NUCLEO
  #include "stm32f3xx_hal.h"
  #include "stm32f3xx_nucleo.h"
  #include "stm32f3xx_hal_conf.h"
  #error "create stm32f3xx_hw_conf.h "
#endif

#ifdef USE_STM32F4XX_NUCLEO
  #include "stm32f4xx_hal.h"
  #include "stm32f4xx_nucleo.h"
  #include "stm32f4xx_hal_conf.h"
  #error "create stm32f4xx_hw_conf.h "
#endif

#ifdef USE_STM32L0XX_NUCLEO
  #include "stm32l0xx_hal.h"
  #include "stm32l0xx_nucleo.h"
  #include "stm32l0xx_hal_conf.h"
  #include "stm32l0xx_hw_conf.h"
#endif

#ifdef USE_STM32L1XX_NUCLEO
  #include "stm32l1xx_hal.h"
  #include "stm32l1xx_nucleo.h"
  #include "stm32l1xx_hal_conf.h"
  #include "stm32l1xx_hw_conf.h"
#endif

#ifdef USE_STM32L4XX_NUCLEO
  #include "stm32l4xx_hal.h"
  #include "stm32l4xx_nucleo.h"
  #include "stm32l4xx_hal_conf.h"
  #include "stm32l4xx_hw_conf.h"
#endif

#ifdef USE_B_L072Z_LRWAN1
  #include "stm32l0xx_hal.h"
  #include "b-l072z-lrwan1.h"
  #include "stm32l0xx_hal_conf.h"
  #include "mlm32l0xx_hw_conf.h"
#endif

#ifdef USE_B_L082Z_KETCube
  #include "stm32l0xx_hal.h"
	#include "ketCube_mainBoard.h"
  #include "stm32l0xx_hal_conf.h"
	#include "mlm32l0xx_hw_conf.h"
#endif

/* --------Preprocessor compile swicth------------ */
/* debug swicthes in debug.h */
//#define DEBUG
//#define TRACE
   
/* uncomment below line to never enter lowpower modes in main.c*/
//#define LOW_POWER_DISABLE

/* debug swicthes in bsp.c */
//#define SENSOR_ENABLED
   
   
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __HW_CONF_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
