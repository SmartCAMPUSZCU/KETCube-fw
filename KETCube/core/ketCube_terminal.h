/**
 * @file    ketCube_terminal.h
 * @author  Jan Belohoubek
 * @version 0.1
 * @date    2018-01-05
 * @brief   This file contains definitions for the terminal
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
#ifndef __KETCUBE_TERMINAL_H
#define __KETCUBE_TERMINAL_H

#include <stdarg.h>
#include <stdlib.h>

#include "ketCube_terminal_common.h"
#include "vcom.h"

/** @defgroup  KETCube_Terminal KETCube Terminal
  * @brief KETCube serial terminal
  *
  * This KETCube module implements the serial terminal interface
  *
  * @ingroup KETCube_Core 
  * @{
  */

#define KETCUBE_TERMINAL_DEBUG           FALSE  /*!< Turn ON/OFF low-level debug messages */
#define KETCUBE_TERMINAL_MODDEBUG        TRUE   /*!< Turn ON/OFF structured debug messages */
#define KETCUBE_TERMINAL_ECHO            TRUE   /*!< Turn ON/OFF echo */

#define KETCUBE_TERMINAL_HISTORY_LEN     3      /*!< Remember last 3 commands */
#define KETCUBE_TERMINAL_CMD_MAX_LEN     128    /*!< Max command length */

#define KETCUBE_TERMINAL_PRINTF(...)     ketCube_terminal_UsartPrint(__VA_ARGS__)    /*!< Printf wrapper */

#define KETCUBE_TERMINAL_PROMPT()        KETCUBE_TERMINAL_PRINTF(">> ")              /*!< Print command line PROMPT */
#define KETCUBE_TERMINAL_ENDL()          KETCUBE_TERMINAL_PRINTF("\n\r")             /*!< Print ASCI terminal ENDL */
#define KETCUBE_TERMINAL_CLR_LINE()      ketCube_terminal_ClearCmdLine()             /*!< Clear ASCI terminal line */

/**
 * @brief Terminal user input item
 */
typedef struct {
    char buffer[KETCUBE_TERMINAL_CMD_MAX_LEN + 1];
    uint8_t ptr;
} ketCube_terminal_buffer_t;

extern void ketCube_terminal_Init(void);
extern void ketCube_terminal_ProcessCMD(void);
extern void ketCube_terminal_UpdateCmdLine(void);
extern void ketCube_terminal_ClearCmdLine(void);

extern void ketCube_terminal_cmd_about(void);
extern void ketCube_terminal_cmd_help(void);

void ketCube_terminal_UsartPrint(char *format, ...);

void ketCube_terminal_Print(char *format, ...);
void ketCube_terminal_Println(char *format, ...);

extern void ketCube_terminal_CoreSeverityPrintln(ketCube_severity_t
                                                 msgSeverity, char *format,
                                                 ...);

extern void ketCube_terminal_DriverSeverityPrintln(const char * drvName, ketCube_severity_t msgSeverity, char *format, ...);

extern void ketCube_terminal_ModSeverityPrintln(ketCube_severity_t
                                                msgSeverity,
                                                ketCube_cfg_moduleIDs_t
                                                modId, char *format,
                                                va_list args);

extern void ketCube_terminal_DebugPrint(char *format, ...);
extern void ketCube_terminal_DebugPrintln(char *format, ...);



/** @defgroup  KETCube_Terminal_SeverityPrintFn KETCube Terminal Severity print functions
  * @brief  KETCube Terminal Severity print functions
  *
  * The following functions enable severity-configurable printing
  * 
  * @note Inline functions were used instead of variadic macros to achieve inter-compiler portability, while avoiding extrea ifdefs. The ##__VA_ARGS__ construct is only awailable in GNU C compiler
  *
  * @ingroup KETCube_Terminal 
  * @{
  */

static inline void ketCube_terminal_AlwaysPrintln(ketCube_cfg_moduleIDs_t
                                                  modId, char *format, ...)
{
    va_list args;
    va_start(args, format);
    ketCube_terminal_ModSeverityPrintln(KETCUBE_CFG_SEVERITY_NONE, modId,
                                        format, args);
    va_end(args);
}

static inline void ketCube_terminal_NewDebugPrintln(ketCube_cfg_moduleIDs_t
                                                    modId, char *format,
                                                    ...)
{
    va_list args;
    va_start(args, format);
    ketCube_terminal_ModSeverityPrintln(KETCUBE_CFG_SEVERITY_DEBUG, modId,
                                        format, args);
    va_end(args);
}

static inline void ketCube_terminal_ErrorPrintln(ketCube_cfg_moduleIDs_t
                                                 modId, char *format, ...)
{
    va_list args;
    va_start(args, format);
    ketCube_terminal_ModSeverityPrintln(KETCUBE_CFG_SEVERITY_ERROR, modId,
                                        format, args);
    va_end(args);
}

static inline void ketCube_terminal_InfoPrintln(ketCube_cfg_moduleIDs_t
                                                modId, char *format, ...)
{
    va_list args;
    va_start(args, format);
    ketCube_terminal_ModSeverityPrintln(KETCUBE_CFG_SEVERITY_INFO, modId,
                                        format, args);
    va_end(args);
}

/**
* @}
*/


/**
* @}
*/

#endif                          /* KETCUBE_TERMINAL_H */
