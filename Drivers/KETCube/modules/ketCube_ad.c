/**
 * @file    ketCube_ad.c
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-01-04
 * @brief   This file contains the ketCube ADC driver
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

#include "ketCube_cfg.h"

#ifdef KETCUBE_CFG_INC_DRV_AD

#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_i2c.h"

#include "ketCube_ad.h"
#include "ketCube_rtc.h"
#include "ketCube_terminal.h"

#define KETCUBE_AD_VDDA_VREFINT_CAL       ((uint32_t) 3000)       /*!< Internal voltage reference was calibrated at 3V */

#define KETCUBE_AD_VREFINT_CAL            ((uint16_t*) ((uint32_t) 0x1FF80078))   /*!< Internal voltage reference calibration value VREFINT_CAL */

#define KETCUBE_AD_TS_CAL1   ((uint16_t*) ((uint32_t) 0x1FF8007A))  /*!< Internal temperature sensor, parameter TS_CAL1: TS ADC raw data acquired at a temperature of 30 DegC (+-5 DegC), VDDA = 3.0 V (+-10 mV). */

#define KETCUBE_AD_TS_CAL2  ((uint16_t*) ((uint32_t) 0x1FF8007E))  /*!< Internal temperature sensor, parameter TS_CAL2: TS ADC raw data acquired at a temperature of  130 DegC (+-5 DegC), VDDA = 3.0 V (+-10 mV). */

#define KETCUBE_AD_VREFANALOG_TEMPSENSOR_CAL  ((uint32_t) 3000)    /* Analog voltage reference (Vref+) voltage with which temperature sensor has been calibrated in production (+-10 mV) (unit: mV). */

#define KETCUBE_AD_MAX                    ((uint16_t) 4095)       /*!< ADC Resolution */

static ADC_HandleTypeDef hadc;

static uint8_t initRuns = 0;    ///< This driver can be initialized in number of modules. If 0 == not initialized, else initialized

/**
 * @brief  initializes ADC
 *
 * @retval KETCUBE_CFG_DRV_OK in case of success
 * @retval KETCUBE_CFG_DRV_ERROR in case of failure
 */
ketCube_cfg_DrvError_t ketCube_AD_Init(void) {
    initRuns += 1;

    if (initRuns > 1) {
        return KETCUBE_CFG_DRV_OK;
    }
    
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
    
    __HAL_RCC_ADC1_CLK_ENABLE();
    
    HAL_ADC_Init(&hadc);
    
    __HAL_RCC_ADC1_CLK_DISABLE();
    
    return KETCUBE_CFG_DRV_OK;
}

/**
 * @brief  DeInitializes ADC
 *
 * @retval KETCUBE_CFG_DRV_OK in case of success
 * @retval KETCUBE_CFG_DRV_ERROR in case of failure
 */
ketCube_cfg_DrvError_t ketCube_AD_UnInit(void) {
    if (initRuns > 1) {
        initRuns -= 1;
        return KETCUBE_CFG_DRV_OK;
    } else if (initRuns == 1) {
        // Run UnInit body once only: (initRuns == 1)
        initRuns = 0;
        // UnInit here ...
        HAL_ADC_DeInit(&hadc);
    }

    return KETCUBE_CFG_DRV_OK;
}

/**
  * @brief Read ADC channel single-conversion result
  
  * @param channel ADC channel
  * 
  * @retval value connversion result
  *
  */
uint16_t ketCube_AD_ReadChannel(uint32_t channel) {
    ADC_ChannelConfTypeDef adcConf;
    uint16_t adcData = 0;
    TimerTime_t timeout = 0;
    
    if (initRuns > 0) {
        /* wait the Vrefint used by adc is set */
        timeout = ketCube_RTC_GetTimerValue( );
        while (__HAL_PWR_GET_FLAG(PWR_FLAG_VREFINTRDY) == RESET) {
            if( ((ketCube_RTC_GetTimerValue( ) - timeout)) < ketCube_RTC_ms2Tick(KETCUBE_AD_VREFINT_MAX_TIMEOUT_MS) ) {
                break;
            }
        }
          
        __HAL_RCC_ADC1_CLK_ENABLE();
        
        /*calibrate ADC if any calibraiton hardware*/
        HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
        
        /* Deselects all channels*/
        adcConf.Channel = ADC_CHANNEL_MASK;
        adcConf.Rank = ADC_RANK_NONE; 
        HAL_ADC_ConfigChannel(&hadc, &adcConf);
          
        /* configure adc channel */
        adcConf.Channel = channel;
        adcConf.Rank = ADC_RANK_CHANNEL_NUMBER;
        HAL_ADC_ConfigChannel(&hadc, &adcConf);
        
        /* Start the conversion process */
        HAL_ADC_Start(&hadc);
          
        /* Wait for the end of conversion */
        HAL_ADC_PollForConversion( &hadc, HAL_MAX_DELAY );
        
        /* Get the converted value of regular channel */
        adcData = HAL_ADC_GetValue(&hadc);
        
        __HAL_ADC_DISABLE(&hadc) ;
        
        __HAL_RCC_ADC1_CLK_DISABLE();
    }
    
    return adcData;
}


/**
  * @brief Read ADC channel single-conversion value in mV
  
  * @param channel ADC channel
  * 
  * @retval value connversion value in mV
  *
  */
uint16_t ketCube_AD_ReadChannelmV(uint32_t channel)
{
    uint32_t vddmv;
    uint16_t vdd;
    uint16_t vin;
    uint16_t mv;

    vdd = ketCube_AD_ReadChannel(ADC_CHANNEL_VREFINT);

    if (vdd == 0) {
        return KETCUBE_CFG_MODULE_ERROR;
    }

    vddmv =
        (((uint32_t) KETCUBE_AD_VDDA_VREFINT_CAL *
          (*KETCUBE_AD_VREFINT_CAL)) / vdd);

    vin = ketCube_AD_ReadChannel(channel);

    // for 12 bit resolution ...
    mv = (uint16_t) ((vddmv * vin) / KETCUBE_AD_MAX);
    ketCube_terminal_DriverSeverityPrintln(KETCUBE_AD_NAME, KETCUBE_CFG_SEVERITY_DEBUG, "Voltage: %d", mv);
    
    return mv;
}

/**
  * @brief This function returns the battery level in mV
  * @param none
  
  * @retval battery level in mV
  */
uint32_t ketCube_AD_GetBatLevelmV(void) {
    uint16_t measuredLevel = 0;
    uint32_t batteryLevelmV;

    measuredLevel = ketCube_AD_ReadChannel(ADC_CHANNEL_VREFINT);

    if (measuredLevel == 0) {
        batteryLevelmV = 0;
    } else {
        batteryLevelmV =
            (((uint32_t) KETCUBE_AD_VDDA_VREFINT_CAL *
              (*KETCUBE_AD_VREFINT_CAL)) / measuredLevel);
    }
    
    return batteryLevelmV;
}

/**
 * @brief Compute core temperature
 * 
 * Compute temperature based on RAW ADC data and battery voltage
 * 
 * @param tempRAW temperature sensor RAW data
 * @param bat battery level in mV
 * 
 * @retval temperature in degrees of Celsius
 * 
 */
static inline int32_t compute_temperature(uint16_t tempRAW, uint16_t bat) {
  int32_t deg = 0;
  
  deg = (int32_t) ((tempRAW * bat) / KETCUBE_AD_VREFANALOG_TEMPSENSOR_CAL) - (int32_t) *KETCUBE_AD_TS_CAL1;
  deg = deg * (int32_t)(130 - 30);
  deg = deg << 8;
  deg = deg / ((int32_t) (*KETCUBE_AD_TS_CAL2 - *KETCUBE_AD_TS_CAL1));
  deg = deg + (30 << 8);
  
  return deg;
}

/**
  * @brief This function returns the core temperature
  * 
  * @retval temperature in degrees of Celsius
  * 
  */
uint16_t ketCube_AD_GetTemperature(void) {
    return (uint16_t) compute_temperature(ketCube_AD_ReadChannel(ADC_CHANNEL_TEMPSENSOR), ketCube_AD_GetBatLevelmV());
}

#endif // KETCUBE_CFG_INC_DRV_AD
