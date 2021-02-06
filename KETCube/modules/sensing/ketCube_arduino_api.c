/**
 * @file    ketCube_arduino_API.c
 * @author  Jan Bělohoubek
 * @version 0.2
 * @date    2020-11-30
 * @brief   This file contains definitions for the Arduino API
 * 
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 University of West Bohemia in Pilsen
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

#include <stdarg.h>
#include <stdlib.h>

#include "ketCube_cfg.h"
#include "ketCube_mainBoard.h"
#include "ketCube_arduino_api.h"
#include "ketCube_buzzer.h"

#include "ketCube_gpio.h"
#include "ketCube_ad.h"
#include "ketCube_i2c.h"
#include "ketCube_terminal.h"
#include "ketCube_rtc.h"
#include "timeServer.h"

#ifdef KETCUBE_CFG_INC_MOD_ARDUINO

static TimerEvent_t ketCube_Arduino_API_wakeAfterTimer;
bool ketCube_Arduino_API_wakeAfterTimerFlag = FALSE;

uint8_t * ketCube_Arduino_API_LPWANsendBuff;
uint8_t ketCube_Arduino_API_LPWANsendBuffLen;

volatile rxHandler_t rxDataCallback = NULL;
volatile voidPtr_t regSleepExitHandler = NULL;

void ketCube_Arduino_API_wakeAfterTimerCallback(void* context) {
    ketCube_Arduino_API_wakeAfterTimerFlag = TRUE;
}

/* *********** API - module interface ************** */

/**
 * @brief This function must be called during Arduino module initialization
 *  
 */
void ketCube_Arduino_API_Init() {
    ketCube_Arduino_API_wakeAfterTimerFlag = FALSE;
    TimerInit(&ketCube_Arduino_API_wakeAfterTimer, ketCube_Arduino_API_wakeAfterTimerCallback);
    ketCube_Arduino_API_LPWANsendBuffLen = 0;
}

/**
 * @brief This function is used to get data by arduino module
 *  
 */
void ketCube_Arduino_API_getBuffer(uint8_t *buffer, uint8_t *len) {
    if (ketCube_Arduino_API_LPWANsendBuffLen != 0) {
        *len = ketCube_Arduino_API_LPWANsendBuffLen;
        if (*len > LPWAN_MAX_BYTES) {
            *len = LPWAN_MAX_BYTES;
        }
        memcpy(buffer, ketCube_Arduino_API_LPWANsendBuff, *len);
    }
    ketCube_Arduino_API_LPWANsendBuffLen = 0;
}

/**
 * @brief This function must be called by the Arduino module sleepExit function
 *  
 */
void ketCube_Arduino_API_SleepExitHandler() {
    if (regSleepExitHandler != NULL) {
        regSleepExitHandler();
    }
}

/**
 * @brief Process LPWAN received data
 *  
 */
void ketCube_Arduino_API_NewLPWANData(uint8_t * buffer, uint8_t len) {
    if (rxDataCallback != NULL) {
        rxDataCallback(buffer, len);
    }
}

/* *********** ********************* ************** */

/**
 * @brief Arduino PIN definition
 * 
 */
uint32_t ketCube_Arduino_ADCPinChannels[] = {
    /* IO1  = */ (uint32_t) NULL,
    /* IO2  = */ (uint32_t) NULL,
    /* IO3  = */ (uint32_t) NULL,
    /* IO4  = */ ADC_CHANNEL_5,
    /* AN   = */ ADC_CHANNEL_4,
    /* RST  = */ ADC_CHANNEL_0,
    /* CS   = */ (uint32_t) NULL,
    /* SCK  = */ (uint32_t) NULL,
    /* MISO = */ (uint32_t) NULL,
    /* MOSI = */ (uint32_t) NULL,
    /* PWM  = */ (uint32_t) NULL,
    /* INT  = */ (uint32_t) NULL,
    /* RX   = */ ADC_CHANNEL_3,
    /* TX   = */ ADC_CHANNEL_2,
    /* SCL  = */ (uint32_t) NULL,
    /* SDA  = */ (uint32_t) NULL,
    /* LED1 = */ (uint32_t) NULL,
    /* LED2 = */ (uint32_t) NULL
};

/**
 * @brief Arduino PIN to KETCube PIN mapping
 *  
 */
uint32_t ketCube_Arduino_API_GPIO_ArduinoPin2KETCubePin(uint8_t pin) {
    switch (pin) {
        case IO1  : return KETCUBE_MAIN_BOARD_PIN_IO1_PIN;
        case IO2  : return KETCUBE_MAIN_BOARD_PIN_IO2_PIN;
        case IO3  : return KETCUBE_MAIN_BOARD_PIN_IO3_PIN;
        case IO4  : return KETCUBE_MAIN_BOARD_PIN_IO4_PIN;
        case AN   : return KETCUBE_MAIN_BOARD_PIN_AN_PIN;
        case RST  : return KETCUBE_MAIN_BOARD_PIN_RST_PIN;
        case CS   : return KETCUBE_MAIN_BOARD_PIN_CS_PIN;
        case SCK  : return KETCUBE_MAIN_BOARD_PIN_SCK_PIN;
        case MISO : return KETCUBE_MAIN_BOARD_PIN_MISO_PIN;
        case MOSI : return KETCUBE_MAIN_BOARD_PIN_MOSI_PIN;
        case PWM  : return KETCUBE_MAIN_BOARD_PIN_PWM_PIN;
        case INT  : return KETCUBE_MAIN_BOARD_PIN_INT_PIN;
        case RX   : return KETCUBE_MAIN_BOARD_PIN_RX_PIN;
        case TX   : return KETCUBE_MAIN_BOARD_PIN_TX_PIN;
        case SCL  : return KETCUBE_MAIN_BOARD_PIN_SCL_PIN;
        case SDA  : return KETCUBE_MAIN_BOARD_PIN_SDA_PIN;
        case LED1 : return KETCUBE_MAIN_BOARD_LED1_PIN;
        case LED2 : return KETCUBE_MAIN_BOARD_LED2_PIN;
        default:
            return (uint32_t) NULL;
    }
}

/**
 * @brief Arduino PIN to KETCube PORT mapping
 *  
 */
uint32_t ketCube_Arduino_API_GPIO_ArduinoPin2KETCubePort(ketCube_Arduino_pin_t pin) {
    switch (pin) {
        case IO1  : return KETCUBE_MAIN_BOARD_PIN_IO1_PORT;
        case IO2  : return KETCUBE_MAIN_BOARD_PIN_IO2_PORT;
        case IO3  : return KETCUBE_MAIN_BOARD_PIN_IO3_PORT;
        case IO4  : return KETCUBE_MAIN_BOARD_PIN_IO4_PORT;
        case AN   : return KETCUBE_MAIN_BOARD_PIN_AN_PORT;
        case RST  : return KETCUBE_MAIN_BOARD_PIN_RST_PORT;
        case CS   : return KETCUBE_MAIN_BOARD_PIN_CS_PORT;
        case SCK  : return KETCUBE_MAIN_BOARD_PIN_SCK_PORT;
        case MISO : return KETCUBE_MAIN_BOARD_PIN_MISO_PORT;
        case MOSI : return KETCUBE_MAIN_BOARD_PIN_MOSI_PORT;
        case PWM  : return KETCUBE_MAIN_BOARD_PIN_PWM_PORT;
        case INT  : return KETCUBE_MAIN_BOARD_PIN_INT_PORT;
        case RX   : return KETCUBE_MAIN_BOARD_PIN_RX_PORT;
        case TX   : return KETCUBE_MAIN_BOARD_PIN_TX_PORT;
        case SCL  : return KETCUBE_MAIN_BOARD_PIN_SCL_PORT;
        case SDA  : return KETCUBE_MAIN_BOARD_PIN_SDA_PORT;
        case LED1 : return KETCUBE_MAIN_BOARD_LED1_PORT;
        case LED2 : return KETCUBE_MAIN_BOARD_LED2_PORT;
        default:
            return KETCUBE_GPIO_NOPIN;
    }
}

/**
 * @brief Initialize digital PIN in Arduino
 */
void ketCube_Arduino_API_GPIO_pinMode(ketCube_Arduino_pin_t pin, ketCube_Arduino_pinMode_t pinMode) {
    GPIO_InitTypeDef GPIO_InitStruct;

    switch(pinMode & 0xF0) {
        case INPUT_PULLUP: 
            GPIO_InitStruct.Pull = GPIO_PULLUP;
            break;
        case INPUT_PULLDOWN: 
            GPIO_InitStruct.Pull = GPIO_PULLDOWN;
            break;
        default:
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            break;
    }
    
    switch(pinMode & 0x0F) {
        case OUTPUT: 
            GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStruct.Pull = GPIO_NOPULL; // force no pull
            break;
        case IT_RISING: 
            GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
            break;
        case IT_FALLING: 
            GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
            break;
        case IT_CHANGE: 
            GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
            break;
        case INPUT: 
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            break;
        case ANALOG:
        default:
            GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
            GPIO_InitStruct.Pull = GPIO_NOPULL; // force no pull
            break;
    }
    
    GPIO_InitStruct.Pin  = ketCube_Arduino_API_GPIO_ArduinoPin2KETCubePin(pin);
        
    ketCube_GPIO_Init(ketCube_Arduino_API_GPIO_ArduinoPin2KETCubePort(pin),
    ketCube_Arduino_API_GPIO_ArduinoPin2KETCubePin(pin), &GPIO_InitStruct);
}

/**
 * @brief Write digital PIN in Arduino
 */
void ketCube_Arduino_API_GPIO_digitalWrite(ketCube_Arduino_pin_t pin, ketCube_Arduino_pinValue_t pinValue) {
    ketCube_GPIO_Write(ketCube_Arduino_API_GPIO_ArduinoPin2KETCubePort(pin),
    ketCube_Arduino_API_GPIO_ArduinoPin2KETCubePin(pin), pinValue);
}

/**
 * @brief Read digital PIN in Arduino
 */
ketCube_Arduino_pinValue_t ketCube_Arduino_API_GPIO_digitalRead(ketCube_Arduino_pin_t pin) {
    return ketCube_GPIO_Read(ketCube_Arduino_API_GPIO_ArduinoPin2KETCubePort(pin),
    ketCube_Arduino_API_GPIO_ArduinoPin2KETCubePin(pin));
}

/**
 * @brief Attach interrupt to a digital pin
 */
bool ketCube_Arduino_API_GPIO_attachInterrupt(ketCube_Arduino_pin_t pin, voidPtr_t fn) {
    if (ketCube_GPIO_SetIrq(ketCube_Arduino_API_GPIO_ArduinoPin2KETCubePort(pin),
    ketCube_Arduino_API_GPIO_ArduinoPin2KETCubePin(pin), 0, (ketCube_GPIO_VoidFn_t) fn) == KETCUBE_CFG_DRV_OK) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @brief Initialize ADC
 */
void ketCube_Arduino_API_Analog_init(void) {
    ketCube_AD_Init();
}

/**
 * @brief Read analog value in mV from PIN
 */
uint16_t ketCube_Arduino_API_Analog_read(ketCube_Arduino_pin_t pin) {
    if (ketCube_Arduino_ADCPinChannels[pin] != (uint32_t) NULL) {
        return  ketCube_AD_ReadChannelmV(ketCube_Arduino_ADCPinChannels[pin]);
    }
    
    return 0;
}

/**
 * @brief Write analog value in mV to PIN
 */
void ketCube_Arduino_API_Analog_write(ketCube_Arduino_pin_t pin, uint16_t value) {
    if (pin != AN) {
        return;
    }
    
    // TODO not implemented yet
}

/**
 * @brief Initialize PWM
 */
void ketCube_Arduino_API_PWM_init(void) {
    ketCube_buzzer_Init();
}

/**
 * @brief generate PWM tone
 */
void ketCube_Arduino_API_PWM_tone(uint16_t freq, uint16_t dur, uint16_t mute) {
    ketCube_buzzer_Beep(freq, dur, mute);
}

/**
 * @brief Initialize digital PIN as LED driver in Arduino
 */
void ketCube_Arduino_API_LED_init(ketCube_Arduino_pin_t pin, ketCube_Arduino_pinValue_t polarity) {       
    ketCube_GPIO_InitLED(ketCube_Arduino_API_GPIO_ArduinoPin2KETCubePort(pin),
    ketCube_Arduino_API_GPIO_ArduinoPin2KETCubePin(pin), polarity);
}

/**
 * @brief Configure the LED driver in Arduino
 */
void ketCube_Arduino_API_LED_set(ketCube_Arduino_pin_t pin, ketCube_Arduino_ledFunction_t function) {       
    ketCube_GPIO_SetLED(ketCube_Arduino_API_GPIO_ArduinoPin2KETCubePort(pin),
    ketCube_Arduino_API_GPIO_ArduinoPin2KETCubePin(pin), function);
}

/**
 * @brief KETCube Terminal PrintInfo
 */
void ketCube_Arduino_API_Terminal_printInfo(char *format, ...) {
    va_list args;
    va_start(args, format);
    ketCube_terminal_ModSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO, KETCUBE_LISTS_MODULEID_ARDUINO,
                                        format, args);
    va_end(args);
}

/**
 * @brief KETCube Terminal PrintError
 */
void ketCube_Arduino_API_Terminal_printError(char *format, ...) {
    va_list args;
    va_start(args, format);
    ketCube_terminal_ModSeverityPrintln(KETCUBE_CFG_SEVERITY_ERROR, KETCUBE_LISTS_MODULEID_ARDUINO,
                                        format, args);
    va_end(args);
}

/**
 * @brief KETCube Terminal PrintDebug
 */
void ketCube_Arduino_API_Terminal_printDebug(char *format, ...) {
    va_list args;
    va_start(args, format);
    ketCube_terminal_ModSeverityPrintln(KETCUBE_CFG_SEVERITY_DEBUG, KETCUBE_LISTS_MODULEID_ARDUINO,
                                        format, args);
    va_end(args);
}

/**
 * @brief KETCube Terminal Print - print always
 */
void ketCube_Arduino_API_Terminal_print(char *format, ...) {
    va_list args;
    va_start(args, format);
    ketCube_terminal_ModSeverityPrintln(KETCUBE_CFG_SEVERITY_NONE, KETCUBE_LISTS_MODULEID_ARDUINO,
                                        format, args);
    va_end(args);
}


/**
 * @brief Delay for specified amount of miliseconds
 */
void ketCube_Arduino_API_Time_delay(uint32_t ms) {
    HAL_Delay(ms);
}

/**
 * @brief Delay for specified amount of miliseconds
 */
void ketCube_Arduino_API_Time_delayMicroseconds(uint32_t us) {
    ketCube_RTC_DelayUs(us);
}

/**
 * @brief Get current time in microseconds
 */
uint32_t ketCube_Arduino_API_Time_micros(void) {
    return HAL_GetUs();
}

/**
 * @brief Get current time in miliseconds
 */
uint32_t ketCube_Arduino_API_Time_millis(void) {
    return HAL_GetTick();
}

/**
 * @brief Wake up after specified amount of miliseconds
 */
void ketCube_Arduino_API_Time_wakeAfter(uint32_t ms) {
    TimerStop(&ketCube_Arduino_API_wakeAfterTimer);
    TimerSetValue(&ketCube_Arduino_API_wakeAfterTimer, ms);
    TimerStart(&ketCube_Arduino_API_wakeAfterTimer);
}

/**
 * @brief Wake-up time elapsed or not yet
 */
bool ketCube_Arduino_API_Time_wakeIntervalElapsed(void) {
    return ketCube_Arduino_API_wakeAfterTimerFlag;
}

/**
 * @brief Send data to LPWAN
 */
bool ketCube_Arduino_API_LPWAN_send(uint8_t * buffer, uint8_t len) {
    ketCube_Arduino_API_LPWANsendBuff = buffer;
    ketCube_Arduino_API_LPWANsendBuffLen = len;
    
    if (len > LPWAN_MAX_BYTES) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief register onRxHandler
 * 
 */
void ketCube_Arduino_API_LPWAN_regOnRxHandler(rxHandler_t fn) {
    rxDataCallback = fn;
}

/**
 * @brief register onSleepHandler
 * 
 */
void ketCube_Arduino_API_Core_regSleepExitHandler(voidPtr_t fn)  {
    regSleepExitHandler = fn;
}

/**
 * @brief Send data to I2C slave
 */
bool ketCube_Arduino_API_I2C_init(void) {
    if (ketCube_I2C_Init() == KETCUBE_CFG_DRV_OK) {
        return TRUE;
    }
    
    return FALSE;
}

/**
 * @brief Send data to I2C slave
 */
bool ketCube_Arduino_API_I2C_write(uint8_t addr, uint8_t * buffer, uint16_t len) {
    if (ketCube_I2C_WriteRawData(addr, buffer, len) == KETCUBE_CFG_DRV_OK) {
        return TRUE;
    }
    
    return FALSE;
}

/**
 * @brief Read data from I2C slave
 */
bool ketCube_Arduino_API_I2C_read(uint8_t addr, uint8_t * buffer, uint16_t len) {
    if (ketCube_I2C_ReadRawData(addr, buffer, len) == KETCUBE_CFG_DRV_OK) {
        return TRUE;
    }
    
    return FALSE;
}

/* KETCube Arduino API object */
ketCube_Arduino_API_t KETCube = {
    .IO.pinMode = &ketCube_Arduino_API_GPIO_pinMode,
    .IO.digitalWrite = &ketCube_Arduino_API_GPIO_digitalWrite,
    .IO.digitalRead = &ketCube_Arduino_API_GPIO_digitalRead,
    .IO.attachInterrupt = &ketCube_Arduino_API_GPIO_attachInterrupt,
    
    .Analog.init = &ketCube_Arduino_API_Analog_init,
    .Analog.read = &ketCube_Arduino_API_Analog_read,
    .Analog.write = &ketCube_Arduino_API_Analog_write,
    
    .PWM.init = &ketCube_Arduino_API_PWM_init,
    .PWM.tone = &ketCube_Arduino_API_PWM_tone,
    
    .LED.init = &ketCube_Arduino_API_LED_init,
    .LED.set = &ketCube_Arduino_API_LED_set,
    
    .Terminal.print = &ketCube_Arduino_API_Terminal_print,
    .Terminal.printError = &ketCube_Arduino_API_Terminal_printError,
    .Terminal.printInfo = &ketCube_Arduino_API_Terminal_printInfo,
    .Terminal.printDebug = &ketCube_Arduino_API_Terminal_printDebug,
    
    .Time.delay = &ketCube_Arduino_API_Time_delay,
    .Time.delayMicroseconds = &ketCube_Arduino_API_Time_delayMicroseconds,
    .Time.micros = &ketCube_Arduino_API_Time_micros,
    .Time.millis = &ketCube_Arduino_API_Time_millis,
    .Time.wakeAfter = &ketCube_Arduino_API_Time_wakeAfter,
    .Time.wakeIntervalElapsed = &ketCube_Arduino_API_Time_wakeIntervalElapsed,
    
    .LPWAN.send = &ketCube_Arduino_API_LPWAN_send,
    .LPWAN.regOnRxHandler = &ketCube_Arduino_API_LPWAN_regOnRxHandler,
    
    .I2C.init = ketCube_Arduino_API_I2C_init,
    .I2C.read = &ketCube_Arduino_API_I2C_read,
    .I2C.write = &ketCube_Arduino_API_I2C_write,
    
    .Core.regSleepExitHandler = &ketCube_Arduino_API_Core_regSleepExitHandler,
};


#endif                          /* KETCUBE_CFG_INC_MOD_ARDUINO */
