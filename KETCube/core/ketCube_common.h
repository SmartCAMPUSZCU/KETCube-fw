/**
  * @file    ketCube_common.h
  * @author  Jan Belohoubek
  * @author  Martin Ubl
  * @version 0.2
  * @date    2019-07-11
  * @brief   KETCube common definitions
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KETCUBE_COMMON_H
#define __KETCUBE_COMMON_H

/* Includes ------------------------------------------------------------------*/
#ifndef DESKTOP_BUILD
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_gpio.h"
#else
#include "stdint.h"
#endif
#include "stdlib.h"
#include "stdbool.h"

/** @defgroup  KETCube_common KETCube Common Deffinitions
  * @brief KETCube Common deffinitions nd inline functions
  *
  * This KETCube module incorporates common defs
  *
  * @ingroup KETCube_Core 
  * @{
  */

#define TRUE    1               ///< boolean TRUE
#define FALSE   0               ///< boolean FALSE

/*typedef enum {
  TRUE       = 1,
  FALSE      = 0
} bool;*/

typedef uint8_t byte;

extern void KETCube_ErrorHandler(void);

#define KETCUBE_COMMON_BUFFER_LEN    256
extern char ketCube_common_buffer[];    /*!< Shared buffer for common usage anywhere. */

char * ketCube_common_bytes2Str(uint8_t * byteArr, uint8_t len);

/**
  * @brief Convert a single Byte to HEX string (two bytes)
  *
  * @param byte a single byte to convert
  * @param str pointer to a two-byte string
  */
static inline void ketCube_common_Byte2hex(uint8_t byte, char *str)
{
    if ((byte & 0x0F) < 10) {
        str[1] = (byte & 0x0F) + '0';
    } else {
        str[1] = (byte & 0x0F) - 10 + 'A';
    }

    if (((byte & 0xF0) >> 4) < 10) {
        str[0] = ((byte & 0xF0) >> 4) + '0';
    } else {
        str[0] = ((byte & 0xF0) >> 4) - 10 + 'A';
    }
}

/**
  * @brief Convert an integer to DEC string (with defined length, counting zero terminator)
  *
  * @param number a number to be converted
  * @param str pointer to a string of given length
  * @param len length of given string
  */
static inline void ketCube_common_Int2dec(int32_t number, char *str,
                                          int len)
{
    int pos = 0, rem;
    char tmp;

    if (number < 0) {
        (*str++) = '-';         // a dirty trick to avoid reversing the minus sign
        len--;
        number *= -1;
    }

    while (number != 0 && pos < len) {
        rem = number % 10;
        number /= 10;

        str[pos++] = '0' + rem;
    }

    for (int rem = 0; rem < pos / 2; rem++) {
        tmp = str[rem];
        str[rem] = str[pos - 1 - rem];
        str[pos - 1 - rem] = tmp;
    }

    str[pos] = '\0';
}

/**
  * @brief Test if the string is valid HEX string
  *
  * @param str pointer to HEX string
  * @param len HEX string length
  *
  * @retval TRUE if it is a valid HEX string
  * @retval FALSE otherwise
  */
static inline bool ketCube_common_IsHexString(const char *str, uint8_t len)
{
    uint8_t i = 0;

    for (i = 0; i < len; i++) {
        if ((str[i] >= 'A') && (str[i] <= 'F')) {
            continue;
        } else if ((str[i] >= 'a') && (str[i] <= 'f')) {
            continue;
        } else if ((str[i] >= '0') && (str[i] <= '9')) {
            continue;
        } else {
            return FALSE;
        }
    }

    return TRUE;
}

/**
  * @brief Test if the string is valid DEC string
  *
  * @param str pointer to DEC string
  * @param len DEC string length
  *
  * @retval TRUE if it is a valid DEC string
  * @retval FALSE otherwise
  */
static inline bool ketCube_common_IsDecString(const char *str, uint8_t len)
{
    uint8_t i = 0;

    for (i = 0; i < len && str[i] != '\0'; i++) {
        if ((str[i] < '0') || (str[i] > '9'))
            return FALSE;
    }

    return TRUE;
}


/**
  * @brief Convert HEX char to Int (Byte)
  *
  * @param c hex character
  * 
  * @retval character value 0 - 15; in case of error 0xFF
  *
  * @note the function is safe -- ranges are checked strictly
  */
static inline uint8_t ketCube_common_Hex2Val(const char c) {
    
    if ((c >= '0') && (c <= '9')) {
        return (c - '0') & 0x0F;
    } else if ((c >= 'A') && (c <= 'F')) {
        return (c - 'A' + 10) & 0x0F;
    } else if ((c >= 'a') && (c <= 'f')) {
        return (c - 'a' + 10) & 0x0F;
    } else {
        return 0xFF;
    }
                                               
}

/**
  * @brief Convert HEX string to Byte array
  *
  * @param bytes pointer to byte array
  * @param str pointer to HEX string
  * @param len HEX string length
  *
  * @note in case of error (range), function exits immediately
  */
static inline void ketCube_common_Hex2Bytes(uint8_t * bytes, const char *str,
                                            uint8_t len)
{
    uint8_t i = 0, j = 0;
    uint8_t conv0, conv1;

    if (len == 0) {
        return;
    }

    for (i = 0; i < (len - 1); i += 2, j++) {
        conv0 = ketCube_common_Hex2Val(str[i]);
        conv1 = ketCube_common_Hex2Val(str[i + 1]);
        
        if ((conv0 < 0x10) && ((conv1 < 0x10))) {
            bytes[j] = (conv0 << 4) | conv1;
        } else {
            /* return earlier in case of error */
            return;
        }
    }

    if (i == (len - 1)) {
        conv0 = ketCube_common_Hex2Val(str[i]);
        
        if (conv0 < 0x10) {
            bytes[j] = conv0 << 4;
        } else {
            /* return earlier in case of error */
            return;
        }
    }
}

/**
  * @brief Convert DEC string to 4 byte signed integer
  *
  * @param output pointer to output integer
  * @param str pointer to HEX string
  * @param len HEX string length
  *
  */
static inline void ketCube_common_Dec2int(int32_t * output, const char *str,
                                          uint8_t len)
{
    uint8_t sign = FALSE;
    int pos;

    if (len == 0) {
        return;
    }

    *output = 0;

    if (str[0] == '-') {
        sign = TRUE;
        str++;
        len--;
    }

    for (pos = 0; pos < len && str[pos] != '\0'; pos++) {
        *output *= 10;
        *output += str[pos] - '0';
    }

    if (sign == TRUE)
        *output *= -1;
}


/**
  * @brief Return minimum of two bytes
  *
  * @param a a number
  * @param b a number
  * 
  * @retval min minimum
  *
  */
static inline uint8_t ketCube_common_Min(uint8_t a, uint8_t b)
{
    if (a < b)
        return a;
    else
        return b;
}

/**
  * @brief Return maximum of two bytes
  *
  * @param a a number
  * @param b a number
  * 
  * @retval max maximum
  *
  */
static inline uint8_t ketCube_common_Max(uint8_t a, uint8_t b)
{
    if (a > b)
        return a;
    else
        return b;
}

/**
  * @brief Return median from an array of bytes
  *
  * @param values shorts array
  * @param size array size
  * 
  * @retval med median
  *
  */
static inline uint8_t ketCube_common_Med8(uint8_t * values, uint16_t size)
{
    uint16_t i, j, tmp;

    // bubble-sort -- it's efficient for small arrays
    for (i = 0; i < size - 1; i++) {
        for (j = 0; j < size - 1 - i; j++) {
            if (values[j] < values[j + 1]) {
                tmp = values[j];
                values[j] = values[j + 1];
                values[j + 1] = tmp;
            }
        }
    }

    return (uint8_t) values[size / 2];
}

/**
  * @brief Return median from an array of shorts
  *
  * @param values shorts array
  * @param size array size
  * 
  * @retval med median
  *
  */
static inline uint16_t ketCube_common_Med(uint16_t * values, uint16_t size)
{
    uint16_t i, j, tmp;

    // bubble-sort -- it's efficient for small arrays
    for (i = 0; i < size - 1; i++) {
        for (j = 0; j < size - 1 - i; j++) {
            if (values[j] < values[j + 1]) {
                tmp = values[j];
                values[j] = values[j + 1];
                values[j + 1] = tmp;
            }
        }
    }

    return (uint16_t) values[size / 2];
}

/**
  * @brief Return median from an array of shorts
  *
  * @param values shorts array
  * @param size array size
  * 
  * @retval med median
  *
  */
static inline uint32_t ketCube_common_Med32(uint32_t * values, uint16_t size)
{
    uint32_t i, j, tmp;

    // bubble-sort -- it's efficient for small arrays
    for (i = 0; i < size - 1; i++) {
        for (j = 0; j < size - 1 - i; j++) {
            if (values[j] < values[j + 1]) {
                tmp = values[j];
                values[j] = values[j + 1];
                values[j + 1] = tmp;
            }
        }
    }

    return (uint32_t) values[size / 2];
}

/**
  * @brief Return max from an array of ints
  *
  * @param values shorts array
  * @param size array size
  * 
  * @retval max maximum
  *
  */
static inline uint32_t ketCube_common_Max32(uint32_t * values, uint32_t size)
{
    uint32_t i, max;
    
    max = values[0];

    for (i = 0; i < size; i++) {
        if (values[i] > max) {
            max = values[i];
        }
    }

    return max;
}

/**
  * @brief Return max from an array of ints
  *
  * @param values shorts array
  * @param size array size
  * 
  * @retval max maximum
  *
  */
static inline uint32_t ketCube_common_Min32(uint32_t * values, uint32_t size)
{
    uint32_t i, min;
    
    min = values[0];

    for (i = 0; i < size; i++) {
        if (values[i] < min) {
            min = values[i];
        }
    }

    return min;
}

/**
  * @brief Return median from an array of shorts
  *
  * @param values shorts array
  * @param size array size
  * 
  * @retval avg median
  *
  */
static inline uint16_t ketCube_common_Avg(uint16_t * values, uint16_t size)
{
    uint16_t i;
    uint32_t result = 0;

    // average
    for (i = 0; i < size; i++) {
        result += values[i];
    }

    result = result / size;

    return (uint16_t) result;
}


/**
  * @brief Swap word (16-bit) values
  * 
  * This is intended for conversion when different Endianity is used
  *
  * @param n a 16-bit number
  * 
  * @retval n with swapped bytes
  *
  */
static inline uint16_t ketCube_common_swapW(uint16_t n)
{
    return ((n >> 8) | (n << 8));
}

/**
  * @brief Basic Error Handler
  */
static inline void ketCube_common_BasicErrorHandler()
{
    while (TRUE) {
        // do nothing
    }
}


// Include project-specific common here
#ifndef DESKTOP_BUILD
#include "ketCube_projectCommon.h"
#endif


/**
  * @}
  */

#endif                          /* __KETCUBE_COMMON_H */
