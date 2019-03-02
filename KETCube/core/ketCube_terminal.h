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

#include "ketCube_common.h"
#include "ketCube_cfg.h"
#include "vcom.h"

/** @defgroup  KETCube_Terminal KETCube Terminal
  * @brief KETCube serial terminal
  *
  * This KETCube module implements the serial terminal interface
  *
  * @ingroup KETCube_Core 
  * @{
  */

#define KETCUBE_TERMINAL_DEBUG           FALSE  /*<! Turn ON/OFF low-level debug messages */
#define KETCUBE_TERMINAL_MODDEBUG        TRUE   /*<! Turn ON/OFF structured debug messages */
#define KETCUBE_TERMINAL_ECHO            TRUE   /*<! Turn ON/OFF echo */

#define KETCUBE_TERMINAL_HISTORY_LEN     3      /*<! Remember last 3 commands */
#define KETCUBE_TERMINAL_CMD_MAX_LEN     128    /*<! Max command length */

#define KETCUBE_TERMINAL_PRINTF(...)     ketCube_terminal_UsartPrint(__VA_ARGS__)

#define KETCUBE_TERMINAL_PROMPT()        KETCUBE_TERMINAL_PRINTF(">> ")
#define KETCUBE_TERMINAL_ENDL()          KETCUBE_TERMINAL_PRINTF("\n\r")
#define KETCUBE_TERMINAL_CLR_LINE()      ketCube_terminal_ClearCmdLine()

/**
 * @brief KETCube terminal command possible error codes
 */
typedef enum ketCube_terminal_command_errorCode_t {
    KETCUBE_TERMINAL_CMD_ERR_OK = 0,
    KETCUBE_TERMINAL_CMD_ERR_INVALID_PARAMS,
    KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL,
} ketCube_terminal_command_errorCode_t;

/**
 * @brief KETCube terminal command flags
 */
typedef struct ketCube_terminal_command_flags_t {
    /* Record groups */
    bool isGroup:1;           ///< this record is command group (has subcommands, does not have handler)
    
    /* Command origin */
    bool isLocal:1;           ///< this command can be executed from local terminal
    bool isRemote:1;          ///< this command can be executed from remote terminal
    
    /* Command type */
    bool isEnvCmd:1;          ///< this command is a ENV command
    bool isSetCmd:1;          ///< this command is a SET command
    bool isShowCmd:1;         ///< this command is a SHOW command
    
    /* Storage region -- for SET/SHOW commands */
    bool isEEPROM:1;          ///< this command modifies EEPROM variables
    bool isRAM:1;             ///< this command modifies RAM variables
    
    /* Command specifier */
    bool isGeneric:1;         ///< this command is a Generic command
} ketCube_terminal_command_flags_t;

/**
 * @brief Returns and of flag(s) AND
 * 
 * @param out pointer to output flags
 * @param in1 pointer to input flags
 * @param in2 pointer to input flags
 * 
 * @note this inline function replaces the bitwise AND operation as it is tricky 
 *       to write AND (&) for bit-structs in a generic way in C
 * 
 */
static inline void ketCube_terminal_andCmdFlags(
    ketCube_terminal_command_flags_t * out,
    ketCube_terminal_command_flags_t * in1,
    ketCube_terminal_command_flags_t * in2)
{
    out->isGroup   = in1->isGroup   && in2->isGroup;
    
    out->isLocal   = in1->isLocal   && in2->isLocal;
    out->isRemote  = in1->isRemote  && in2->isRemote;
    
    out->isEnvCmd  = in1->isEnvCmd  && in2->isEnvCmd;
    out->isSetCmd  = in1->isSetCmd  && in2->isSetCmd;
    out->isShowCmd = in1->isShowCmd && in2->isShowCmd;
    
    out->isEEPROM  = in1->isEEPROM  && in2->isEEPROM;
    out->isRAM     = in1->isRAM     && in2->isRAM;
    
    out->isGeneric = in1->isGeneric && in2->isGeneric;
}

/**
 * @brief KETCube terminal command parameter type definitions
 */
typedef enum ketCube_terminal_paramSet_type_t {
    KETCUBE_TERMINAL_PARAMS_NONE = 0,
    KETCUBE_TERMINAL_PARAMS_BOOLEAN,
    KETCUBE_TERMINAL_PARAMS_STRING,
    KETCUBE_TERMINAL_PARAMS_BYTE,
    KETCUBE_TERMINAL_PARAMS_INT32,
    KETCUBE_TERMINAL_PARAMS_UINT32,
    KETCUBE_TERMINAL_PARAMS_INT32_PAIR,
    KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY
} ketCube_terminal_paramSet_type_t;

/* Maximum length of string returned by command */
#define KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH               64
#define KETCUBE_TERMINAL_PARAM_BYTE_ARRAY_MAX_LENGTH        32

/**
 * @brief KETCube terminal command parameter container
 */
typedef union ketCube_terminal_paramSet_t {
    /* KETCUBE_TERMINAL_PARAMS_BOOLEAN */
    bool as_bool;
    /* KETCUBE_TERMINAL_PARAMS_BYTE */
    uint8_t as_byte;
    /* KETCUBE_TERMINAL_PARAMS_INT32 */
    int as_int32;
    /* KETCUBE_TERMINAL_PARAMS_UINT32 */
    int as_uint32;
    /* KETCUBE_TERMINAL_PARAMS_STRING */
    char as_string[KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH];
    /* KETCUBE_TERMINAL_PARAMS_INT32_PAIR */
    struct {
        int first;
        int second;
    } as_int32_pair;
    struct {
        byte data[KETCUBE_TERMINAL_PARAM_BYTE_ARRAY_MAX_LENGTH];
        uint16_t length;
    } as_byte_array;
} ketCube_terminal_paramSet_t;

/**
 * @brief Returns parameter count based on param set type
 */
static inline int ketCube_terminal_ParamSetTypeToCount(
    ketCube_terminal_paramSet_type_t type)
{
    switch (type)
    {
        case KETCUBE_TERMINAL_PARAMS_NONE:
        default:
            return 0;
        case KETCUBE_TERMINAL_PARAMS_BOOLEAN:
        case KETCUBE_TERMINAL_PARAMS_BYTE:
        case KETCUBE_TERMINAL_PARAMS_STRING:
        case KETCUBE_TERMINAL_PARAMS_INT32:
        case KETCUBE_TERMINAL_PARAMS_UINT32:
        case KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY:
            return 1;
        case KETCUBE_TERMINAL_PARAMS_INT32_PAIR:
            return 2;
    }
}

/**
* @brief  KETCube terminal command definition.
*/
typedef struct ketCube_terminal_cmd_t {
    char * cmd;                       /*< command format */
    char * descr;                     /*< Human-readable command description/help */
    ketCube_terminal_command_flags_t flags;           /*< command flags */
    ketCube_terminal_paramSet_type_t outputSetType;   /*< cmd output parameter type */
    ketCube_terminal_paramSet_type_t paramSetType;    /*< cmd input parameter type */
    union
    {
        void (*callback) (void);                      /*< Ptr to a custom callback */
        struct ketCube_terminal_cmd_t * subCmdList;   /*< Ptr to a subcommand list (if this is root command) */
        struct ketCube_cfg_varDescr_t * cfgVarPtr;    /*< The configuration variable descriptor - RAM/EEPROM (use for generic commands) */ 
    } settingsPtr;
} ketCube_terminal_cmd_t;

extern ketCube_terminal_cmd_t ketCube_terminal_commands[];

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
