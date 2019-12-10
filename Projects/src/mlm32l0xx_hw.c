/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Target board general functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
 /*******************************************************************************
  * @file    mlm32l0xx_hw.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    27-February-2017
  * @brief   system hardware driver
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
	*******************************************************************************
	*
	* @author  Jan Belohoubek
	* @author  Martin Ubl
  * @version 0.1
  * @date    2018-05-07
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
#include "hw.h"
#include "radio.h"
#include "debug.h"

#include "ketCube_cfg.h"
#include "ketCube_modules.h"
#include "ketCube_terminal.h"
#include "ketCube_uart.h"
#include "ketCube_rtc.h"


/*!
 * \brief ADC Vbat measurement constants
 */

 /* Internal voltage reference, parameter VREFINT_CAL*/
#define VREFINT_CAL       ((uint16_t*) ((uint32_t) 0x1FF80078))
#define LORAWAN_MAX_BAT   254


/* Internal temperature sensor: constants data used for indicative values in  */
/* this example. Refer to device datasheet for min/typ/max values.            */

/* Internal temperature sensor, parameter TS_CAL1: TS ADC raw data acquired at 
 *a temperature of 30 DegC (+-5 DegC), VDDA = 3.0 V (+-10 mV). */
#define TEMP30_CAL_ADDR   ((uint16_t*) ((uint32_t) 0x1FF8007A))

/* Internal temperature sensor, parameter TS_CAL2: TS ADC raw data acquired at 
 *a temperature of  130 DegC (+-5 DegC), VDDA = 3.0 V (+-10 mV). */
#define TEMP130_CAL_ADDR  ((uint16_t*) ((uint32_t) 0x1FF8007E))

/* Vdda value with which temperature sensor has been calibrated in production 
   (+-10 mV). */
#define VDDA_TEMP_CAL                  ((uint32_t) 3000)        


#define COMPUTE_TEMPERATURE(TS_ADC_DATA, VDDA_APPLI)                           \
  ((((( ((int32_t)((TS_ADC_DATA * VDDA_APPLI) / VDDA_TEMP_CAL)                  \
        - (int32_t) *TEMP30_CAL_ADDR)                                          \
     ) * (int32_t)(130 - 30)                                                   \
    )<<8) / (int32_t)(*TEMP130_CAL_ADDR - *TEMP30_CAL_ADDR)                        \
   ) + (30<<8)                                                                      \
  )

static ADC_HandleTypeDef hadc;
/*!
 * Flag to indicate if the ADC is Initialized
 */
static bool AdcInitialized = false;

/*!
 * Flag to indicate if the MCU is Initialized
 */
static bool McuInitialized = false;

/**
  * @brief This function initializes the hardware
  * @param None
  * @retval None
  */
void HW_Init( void )
{
  if( McuInitialized == false )
  {
#if defined( USE_BOOTLOADER )
    /* Set the Vector Table base location at 0x3000 */
    NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x3000 );
#endif

    HW_AdcInit( );

    //Radio.IoInit( );
    
    //HW_SPI_Init( );

    //HW_RTC_Init( );
    
    // ketCube_RTC_Init();

    McuInitialized = true;
  }
}


uint16_t HW_GetTemperatureLevel( void ) 
{
  uint16_t measuredLevel =0; 
  uint32_t batteryLevelmV;
  uint16_t temperatureDegreeC;

  measuredLevel = HW_AdcReadChannel( ADC_CHANNEL_VREFINT ); 

  if (measuredLevel ==0)
  {
    batteryLevelmV =0;
  }
  else
  {
    batteryLevelmV= (( (uint32_t) VDDA_VREFINT_CAL * (*VREFINT_CAL ) )/ measuredLevel);
  }
#if 0  
  PRINTF("VDDA= %d\n\r", batteryLevelmV);
#endif
  
  measuredLevel = HW_AdcReadChannel( ADC_CHANNEL_TEMPSENSOR ); 
  
  temperatureDegreeC = COMPUTE_TEMPERATURE( measuredLevel, batteryLevelmV);

#if 0 
  {
    uint16_t temperatureDegreeC_Int= (temperatureDegreeC)>>8;
    uint16_t temperatureDegreeC_Frac= ((temperatureDegreeC-(temperatureDegreeC_Int<<8))*100)>>8;  
    PRINTF("temp= %d, %d,%d\n\r", temperatureDegreeC, temperatureDegreeC_Int, temperatureDegreeC_Frac);
  }
#endif
  
  return (uint16_t) temperatureDegreeC;
}
/**
  * @brief This function return the battery level
  * @param none
  * @retval the battery level  1 (very low) to 254 (fully charged)
  */
uint8_t HW_GetBatteryLevel( void ) 
{
  uint8_t batteryLevel = 0;
  uint16_t measuredLevel = 0;
  uint32_t batteryLevelmV;

  measuredLevel = HW_AdcReadChannel( ADC_CHANNEL_VREFINT ); 

  if (measuredLevel == 0)
  {
    batteryLevelmV = 0;
  }
  else
  {
    batteryLevelmV= (( (uint32_t) VDDA_VREFINT_CAL * (*VREFINT_CAL ) )/ measuredLevel);
  }

  if (batteryLevelmV > VDD_BAT)
  {
    batteryLevel = LORAWAN_MAX_BAT;
  }
  else if (batteryLevelmV < VDD_MIN)
  {
    batteryLevel = 0;
  }
  else
  {
    batteryLevel = (( (uint32_t) (batteryLevelmV - VDD_MIN)*LORAWAN_MAX_BAT) /(VDD_BAT-VDD_MIN) ); 
  }
  return batteryLevel;
}

/**
  * @brief Get battery level
  *
  * @param buffer pointer to fuffer for storing the result of milivolt mesurement
  * @param len data len in bytes
  *
  * @retval KETCUBE_CFG_MODULE_OK in case of success
  * @retval KETCUBE_CFG_MODULE_ERROR in case of failure
  */
ketCube_cfg_ModError_t HW_ketCube_getBatLevel(uint8_t * buffer, uint8_t * len)
{
    // write to buffer
    *len = 1;
    buffer[0] = HW_GetBatteryLevel();
    
//     ketCube_terminal_DebugPrintln("batLevel :: %d", buffer[0]);
    
    return KETCUBE_CFG_MODULE_OK;
}

/**
  * @brief This function initializes the ADC
  * @param none
  * @retval none
  */
void HW_AdcInit( void )
{
  if( AdcInitialized == false )
  {
    AdcInitialized = true;
#if 0
    GPIO_InitTypeDef initStruct;
#endif
    
    hadc.Instance  = ADC1;
    
    hadc.Init.OversamplingMode      = DISABLE;
  
    hadc.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc.Init.LowPowerAutoPowerOff  = DISABLE;
    hadc.Init.LowPowerFrequencyMode = ENABLE;
    hadc.Init.LowPowerAutoWait      = DISABLE;
    
    hadc.Init.Resolution            = ADC_RESOLUTION_12B;
    hadc.Init.SamplingTime          = ADC_SAMPLETIME_160CYCLES_5;
    hadc.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;
    hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    hadc.Init.ContinuousConvMode    = DISABLE;
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
    hadc.Init.DMAContinuousRequests = DISABLE;

    ADCCLK_ENABLE();
    

    HAL_ADC_Init( &hadc );
#if 0
    initStruct.Mode =GPIO_MODE_ANALOG;
    initStruct.Pull = GPIO_NOPULL;
    initStruct.Speed = GPIO_SPEED_HIGH;

    HW_GPIO_Init( BAT_LEVEL_PORT, BAT_LEVEL_PIN, &initStruct );
#endif
  }
}
/**
  * @brief This function De-initializes the ADC
  * @param none
  * @retval none
  */
void HW_AdcDeInit( void )
{
  AdcInitialized = false;
}

/**
  * @brief This function De-initializes the ADC
  * @param Channel
  * @retval Value
  */
uint16_t HW_AdcReadChannel( uint32_t Channel )
{

  ADC_ChannelConfTypeDef adcConf;
  uint16_t adcData = 0;
  
  if( AdcInitialized == true )
  {
    /* wait the the Vrefint used by adc is set */
    while (__HAL_PWR_GET_FLAG(PWR_FLAG_VREFINTRDY) == RESET) {};
      
    ADCCLK_ENABLE();
    
    /*calibrate ADC if any calibraiton hardware*/
    HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED );
    
    /* Deselects all channels*/
    adcConf.Channel = ADC_CHANNEL_MASK;
    adcConf.Rank = ADC_RANK_NONE; 
    HAL_ADC_ConfigChannel( &hadc, &adcConf);
      
    /* configure adc channel */
    adcConf.Channel = Channel;
    adcConf.Rank = ADC_RANK_CHANNEL_NUMBER;
    HAL_ADC_ConfigChannel( &hadc, &adcConf);

    /* Start the conversion process */
    HAL_ADC_Start( &hadc);
      
    /* Wait for the end of conversion */
    HAL_ADC_PollForConversion( &hadc, HAL_MAX_DELAY );
      
    /* Get the converted value of regular channel */
    adcData = HAL_ADC_GetValue ( &hadc);

    __HAL_ADC_DISABLE( &hadc) ;

    ADCCLK_DISABLE();
  }
  return adcData;
}
