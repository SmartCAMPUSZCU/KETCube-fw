/**
 * @file    ketCube_arduino_api.h
 * @author  Jan Bělohoubek
 * @version 0.2
 * @date    2020-11-30
 * @brief   This file contains API for the Arduino IDE
 * 
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 University of West Bohemia in Pilsen
 * All rights reserved.</center></h2>
 *
 * Developed by:
 * The SmartCampus Team
 * Department of Materials and Technology
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
#ifndef __KETCUBE_ARDUINO_API_H
#define __KETCUBE_ARDUINO_API_H

#include <stdint.h>


/** @defgroup KETCube_Arduino_API KETCube Arduino API
  * @brief KETCube Arduino API
  * @ingroup KETCube_SensMods
  * @{
  */

/** @defgroup KETCube_Arduino_API_Defines API Constants
  * @brief KETCube Arduino API Constants
  * 
  * @{
  */

#ifndef TRUE
/**
 * @brief digital PIN value
 * 
 */
typedef enum {
    TRUE  = 1,  ///< TRUE value
    FALSE = 0   ///< FALSE value
} bool_t;
#endif

/**
 * @brief Arduino PIN definition
 * 
 */
typedef enum {
    IO1 ,       ///< KETCube PIN IO1
    IO2 ,       ///< KETCube PIN IO2 
    IO3 ,       ///< KETCube PIN IO3 
    IO4 ,       ///< KETCube PIN IO4 
    AN  ,       ///< KETCube PIN AN; this pin has ADC capability; this pin has DAC capability
    RST ,       ///< KETCube PIN RST; this pin has ADC capability
    CS  ,       ///< KETCube PIN CS  
    SCK ,       ///< KETCube PIN SCK 
    MISO,       ///< KETCube PIN MISO
    MOSI,       ///< KETCube PIN MOSI
    PWM ,       ///< KETCube PIN PWM 
    INT ,       ///< KETCube PIN INT 
    RX  ,       ///< KETCube PIN RX; this pin has ADC capability  
    TX  ,       ///< KETCube PIN TX; this pin has ADC capability  
    SCL ,       ///< KETCube PIN SCL 
    SDA ,       ///< KETCube PIN SDA 
    LED1,       ///< KETCube PIN LED1
    LED2,       ///< KETCube PIN LED2
} ketCube_Arduino_pin_t;

/**
 * @brief digital PIN mode
 * 
 */
typedef enum {
    ANALOG         = 0x00,   ///< PIN mode ANALOG
    INPUT          = 0x01,   ///< PIN mode INPUT
    OUTPUT         = 0x02,   ///< PIN mode OUTPUT
    IT_RISING      = 0x03,   ///< trigger when the pin goes to high
    IT_FALLING     = 0x04,   ///< trigger when the pin goes to low
    IT_CHANGE      = 0x05,   ///< trigger the interrupt whenever the pin changes value
    INPUT_PULLUP   = 0x10,   ///< PIN mode INPUT with PULL UP
    INPUT_PULLDOWN = 0x20,   ///< PIN mode INPUT with PULL DOWN
} ketCube_Arduino_pinMode_t;

/**
 * @brief digital PIN value
 * 
 */
typedef enum {
    HIGH = TRUE,   ///< PIN value HIGH
    LOW = FALSE    ///< PIN value LOW
} ketCube_Arduino_pinValue_t;

/**
* @brief LED functions
*/
typedef enum {
    LED_OFF = 0,        ///< LED is permanently OFF  
    LED_ON,             ///< LED is permanently ON 
    LED_BLINK_SINGLE,   ///< LED BLINKs - single flash
    LED_BLINK_CONT      ///< LED BLINKs - continuous flashing
} ketCube_Arduino_ledFunction_t;

/**
* @brief LPWAN limits
*/
typedef enum {
    LPWAN_MAX_BYTES = 10   ///< LPWAN is limited to 10 bytes
} ketCube_Arduino_LPWANLimits_t;

/**
* @}
*/

typedef void (*voidPtr_t)(void);
typedef void (*rxHandler_t)(uint8_t *, uint8_t);

typedef void (*IO_pinMode_t)(ketCube_Arduino_pin_t, ketCube_Arduino_pinMode_t);
typedef void (*IO_digitalWrite_t)(ketCube_Arduino_pin_t, ketCube_Arduino_pinValue_t);
typedef ketCube_Arduino_pinValue_t (*IO_digitalRead_t)(ketCube_Arduino_pin_t);
typedef bool (*IO_attachInterrupt_t)(ketCube_Arduino_pin_t, voidPtr_t);

typedef void (*Analog_init_t)(void);
typedef uint16_t (*Analog_read_t)(ketCube_Arduino_pin_t);
typedef void (*Analog_write_t)(ketCube_Arduino_pin_t, uint16_t);

typedef void (*PWM_init_t)(void);
typedef void (*PWM_tone_t)(uint16_t, uint16_t, uint16_t);

typedef void (*LED_init_t)(ketCube_Arduino_pin_t, ketCube_Arduino_pinValue_t);
typedef void (*LED_set_t)(ketCube_Arduino_pin_t, ketCube_Arduino_ledFunction_t);

typedef void (*Terminal_print_t)(char *, ...);
typedef void (*Terminal_printError_t)(char *, ...);
typedef void (*Terminal_printInfo_t)(char *, ...);
typedef void (*Terminal_printDebug_t)(char *, ...);

typedef void (*Time_delay_t)(uint32_t);
typedef void (*Time_delayMicroseconds_t)(uint32_t);
typedef uint32_t (*Time_micros_t)(void);
typedef uint32_t (*Time_millis_t)(void);
typedef void (*Time_wakeAfter_t)(uint32_t);
typedef bool (*Time_wakeIntervalElapsed_t)(void);

typedef bool (*LPWAN_send_t)(uint8_t *, uint8_t);
typedef void (*LPWAN_regOnRxHandler_t)(rxHandler_t);

typedef bool (*I2C_init_t)(void);
typedef bool (*I2C_read_t)(uint8_t, uint8_t *, uint16_t);
typedef bool (*I2C_write_t)(uint8_t, uint8_t *, uint16_t);

typedef void (*Core_regSleepExitHandler_t)(voidPtr_t);

/**
 * @brief KETCube API object
 * 
 */
typedef struct ketCube_Arduino_API_t {   
   /**
    * @brief KETCube Arduino Digital IO API
    * 
    */
    struct IO {       
        /**
         * @brief Configures the specified pin to behave either as an input or an output
         * 
         * @param pin digital PIN, @ref ketCube_Arduino_pin_t
         * @param pinMode @ref ketCube_Arduino_pinMode_t
         * 
         */
        IO_pinMode_t pinMode;
        
        /**
         * @brief Write a value to a digital pin
         * 
         * @param pin digital PIN, @ref ketCube_Arduino_pin_t
         * @param pinValue HIGH or LOW value, @ref ketCube_Arduino_pinValue_t
         * 
         */
        IO_digitalWrite_t digitalWrite;
        
        /**
         * @brief Reads the value from a specified digital pin
         *
         * @param pin digital PIN, @ref ketCube_Arduino_pin_t
         * 
         */
        IO_digitalRead_t digitalRead;
        
        /**
         * @brief Attach interrupt to a PIN
         *
         * @param pin digital PIN, @ref ketCube_Arduino_pin_t
         * @param fn pointer to IRQ Handler function
         * 
         */
        IO_attachInterrupt_t attachInterrupt;
    } IO;
    
    /**
    * @brief KETCube Arduino Analog IO API
    * 
    */
    struct Analog {       
        /**
         * @brief Initialize Analog To Digital Converter (ADC)
         * 
         */
        Analog_init_t init;
        
        /**
         * @brief Read analog value from one of the analog pins (ADC capability)
         * 
         * @param pin digital PIN, @ref ketCube_Arduino_pin_t
         * @retval mv voltage level in millivolts, 0 is returned if PIN has not ADC capability
         * 
         */
        Analog_read_t read;
        
        /**
         * @brief Write analog value to the AN PIN
         * 
         * @param mv voltage level in millivolts
         * 
         * @note This function is not implemented yet
         * 
         */
        Analog_write_t write;
    } Analog;
    
    /**
    * @brief KETCube PWM API
    * 
    */
    struct PWM {  
        /**
         * @brief Inititalize PWM function
         * 
         */
        PWM_init_t init;
        
        /**
         * @brief Generates a square wave of the specified frequency
         * 
         * @param freq frequency in kHz
         * @param duration tone duration in ms
         * @param mute mute time following the tone in ms
         * 
         * @note tones are queued, you can call this function up to 8 times (until you fill the queue) - the tones will be produced sequentialy. If you call this function more than 8 times before queue is freed, (some) tones will be lost
         * 
         */
        PWM_tone_t tone;
    } PWM;
    
    /**
    * @brief KETCube Arduino LED Driver API
    * 
    */
    struct LED {       
        /**
         * @brief Configures the specified pin as the LED driver
         * 
         * @param pin digital PIN, @ref ketCube_Arduino_pin_t
         * @param polarity value to turn LED ON @ref ketCube_Arduino_pinValue_t
         * 
         */
        LED_init_t init;
        
        /**
         * @brief Configures the LED function
         * 
         * @param pin digital PIN, @ref ketCube_Arduino_pin_t
         * @param LEDFunction LED blink function @ref ketCube_Arduino_ledFunction_t
         * 
         */
        LED_set_t set;
        
    } LED;

    /**
    * @brief KETCube Terminal Arduino API
    * 
    */
    struct Terminal {
        /**
         * @brief Sends formatted output to KETCube terminal 
         * 
         * Behaves similarly to the C printf function
         * the String is prefixed by the module prefix string ("Arduino :: ") and terminated by the new line
         * 
         * @param format string that contains the text to be written to stdout. It can optionally contain embedded format tags that are replaced by the values specified in subsequent additional arguments -- @see https://sourceware.org/newlib/libc.html#sprintf
         * @param args additional arguments depending on the format string
         * 
         */
        Terminal_print_t      print;
        
        /**
         * @brief Sends formatted output to KETCube terminal 
         * 
         * Behaves similarly to the C printf function
         * the String is prefixed by the module prefix string ("Arduino :: ") and terminated by the new line
         * 
         * @note the string is produced only if Arduino module severity is set to 1 (ERROR)
         * 
         * @param format string that contains the text to be written to stdout. It can optionally contain embedded format tags that are replaced by the values specified in subsequent additional arguments -- @see https://sourceware.org/newlib/libc.html#sprintf
         * @param args additional arguments depending on the format string
         * 
         */
        Terminal_printError_t printError;
        
        /**
         * @brief Sends formatted output to KETCube terminal 
         * 
         * Behaves similarly to the C printf function
         * the String is prefixed by the module prefix string ("Arduino :: ") and terminated by the new line
         * 
         * @note the string is produced only if Arduino module severity is set to 2 (INFO)
         * 
         * @param format string that contains the text to be written to stdout. It can optionally contain embedded format tags that are replaced by the values specified in subsequent additional arguments -- @see https://sourceware.org/newlib/libc.html#sprintf
         * @param args additional arguments depending on the format string
         * 
         */
        Terminal_printInfo_t  printInfo;
        
        /**
         * @brief Sends formatted output to KETCube terminal 
         * 
         * Behaves similarly to the C printf function
         * the String is prefixed by the module prefix string ("Arduino :: ") and terminated by the new line
         * 
         * @note the string is produced only if Arduino module severity is set to 3 (DEBUG)
         * 
         * @param format string that contains the text to be written to stdout. It can optionally contain embedded format tags that are replaced by the values specified in subsequent additional arguments -- @see https://sourceware.org/newlib/libc.html#sprintf
         * @param args additional arguments depending on the format string
         * 
         */
        Terminal_printDebug_t printDebug;
    } Terminal;
    
    /**
    * @brief KETCube Arduino Time API
    * 
    */
    struct Time {
        /**
         * @brief Pauses the program for the amount of time (in milliseconds)
         *
         * @param ms the number of milliseconds to pause
         * 
         */
        Time_delay_t delay;
        
        /**
         * @brief Pauses the program for the amount of time (in microseconds)
         *
         * @param us the number of microseconds to pause
         * 
         */
        Time_delayMicroseconds_t delayMicroseconds;
        
        /**
         * @brief Get the number of microseconds since last KETCube PoR or overflow
         *
         * @retval ms the number of microseconds since last KETCube PoR or overflow
         * 
         */
        Time_micros_t micros;
        
        /**
         * @brief Get the number of milliseconds since last KETCube PoR
         *
         * @retval ms the number of milliseconds since last KETCube PoR
         * 
         */
        Time_millis_t millis;
        
        /**
         * @brief Wake KETCube after defined amount of time
         *
         * @retval ms the number of milliseconds from now to wake-up
         * 
         */
        Time_wakeAfter_t wakeAfter;
        
        /**
         * @brief Check if the wake-up was caused by timer preset by wakeAfter() function
         *
         * @retval flag TRUE in case, that wake-up interval elapsed, else return FALSE
         * 
         */
        Time_wakeIntervalElapsed_t wakeIntervalElapsed;
    } Time;
    
    /**
    * @brief Low-Power Wide Area Network API
    * 
    */
    struct LPWAN {
        /**
         * @brief Send data to LPWAN (e.g. LoRaWAN)
         *
         * @param buffer pointer to buffer
         * @param len data length
         * 
         * @retval result if maximum payload size exceeded, FALSE (only fraction of data will be send); else return TRUE
         * 
         */
        LPWAN_send_t send;
        
        /**
         * @brief Register onRxEventHandler
         *
         * @param fn pointer to the onRxHandler function
         * 
         */
        LPWAN_regOnRxHandler_t regOnRxHandler;
    } LPWAN;
    
    /**
    * @brief KETCube I2C API
    * 
    */
    struct I2C {
        /**
        * @brief Initialize I2C driver
        * 
        * @retval TRUE in case of success, else return FALSE
        * 
        */
        I2C_init_t init;
        
        /**
        * @brief Read from the I2C Slave
        * 
        * @param addr slave address
        * @param buffer pointer to buffer (of bytes)
        * @param buffLen buffer length
        * 
        * @retval TRUE in case of success, else return FALSE
        * 
        */
        I2C_read_t read;
        
        /**
        * @brief Write to the I2C Slave
        * 
        * @param addr slave address
        * @param buffer pointer to buffer (of bytes)
        * @param buffLen buffer length
        * 
        * @retval TRUE in case of success, else return FALSE
        * 
        */
        I2C_write_t write;
    } I2C;
    
    /**
    * @brief KETCube Core API
    * 
    */
    struct Core {
        /**
        * @brief Register sleepExit Handler
        * 
        * The Handler function is called when KETCube returns sleep mode
        * 
        * @note this function is intended for interrupt-invoked event processing in non-interrupt context
        * 
        */
        Core_regSleepExitHandler_t regSleepExitHandler;
    } Core;
} ketCube_Arduino_API_t;


extern ketCube_Arduino_API_t KETCube;

/**
* @}
*/

#endif                          /* __KETCUBE_ARDUINO_API_H */
