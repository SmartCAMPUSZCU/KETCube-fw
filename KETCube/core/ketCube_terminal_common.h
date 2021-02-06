/**
 * @file    ketCube_terminal_common.h
 * @author  Martin Ubl
 * @version 0.1
 * @date    2018-01-05
 * @brief   This file contains shared definitions for the terminal (local and remote)
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
#ifndef __KETCUBE_TERMINAL_COMMON_H
#define __KETCUBE_TERMINAL_COMMON_H

#include <stdlib.h>
#include <string.h>
#include "ketCube_common.h"
#include "ketCube_cfg.h"
#include "ketCube_module_id.h"

/**
 * @brief Terminal opcodes
 */
typedef enum ketCube_terminal_command_opcode_t {
    KETCUBE_TERMINAL_OPCODE_CMD = 0,
    KETCUBE_TERMINAL_OPCODE_BATCH = 1,
} ketCube_terminal_command_opcode_t;

/**
 * @brief KETCube terminal command possible error codes
 */
typedef enum ketCube_terminal_command_errorCode_t {
    KETCUBE_TERMINAL_CMD_ERR_OK = 0,
    KETCUBE_TERMINAL_CMD_ERR_INVALID_PARAMS,
    KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL,
    
    // remote terminal-related return codes
    KETCUBE_TERMINAL_CMD_ERR_COMMAND_NOT_FOUND,
    KETCUBE_TERMINAL_CMD_ERR_MODULE_NOT_FOUND,
    KETCUBE_TERMINAL_CMD_ERR_FAILED_CONTEXT,
    KETCUBE_TERMINAL_CMD_ERR_UNSPECIFIED_ERROR,
    KETCUBE_TERMINAL_CMD_ERR_NOT_SUPPORTED,
    KETCUBE_TERMINAL_CMD_ERR_CORE_API_MISMATCH,
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
 * @brief KETCube command "before execution" check outcomes enum
 */
typedef enum ketCube_terminal_cmdCheckResult_t {
    KETCUBE_CMD_CHECK_OK,
    KETCUBE_CMD_CHECK_FAILED_DEFINITION,
    KETCUBE_CMD_CHECK_FAILED_CONTEXT_ONLY_LOCAL,
    KETCUBE_CMD_CHECK_FAILED_CONTEXT_ONLY_REMOTE,
} ketCube_terminal_cmdCheckResult_t;

/**
 * @brief KETCube terminal command parameter type definitions
 */
typedef enum ketCube_terminal_paramSet_type_t {
    KETCUBE_TERMINAL_PARAMS_NONE       = 0,
    KETCUBE_TERMINAL_PARAMS_BOOLEAN    = 1,
    KETCUBE_TERMINAL_PARAMS_STRING     = 2,
    KETCUBE_TERMINAL_PARAMS_INT8       = 3,
    KETCUBE_TERMINAL_PARAMS_UINT8      = 4,
    KETCUBE_TERMINAL_PARAMS_BYTE       = 4,
    KETCUBE_TERMINAL_PARAMS_MODULEID   = 5,
    KETCUBE_TERMINAL_PARAMS_INT32      = 6,
    KETCUBE_TERMINAL_PARAMS_UINT32     = 7,
    KETCUBE_TERMINAL_PARAMS_INT32_PAIR = 8,
    KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY = 9
} ketCube_terminal_paramSet_type_t;

/* Maximum length of string returned by command */
#define KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH               64
#define KETCUBE_TERMINAL_PARAM_BYTE_ARRAY_MAX_LENGTH        32

#pragma pack(push, 1)

/**
 * @brief KETCube terminal command parameter container
 */
typedef union ketCube_terminal_paramSet_t {
    /* KETCUBE_TERMINAL_PARAMS_BOOLEAN */
    bool as_bool;
    /* KETCUBE_TERMINAL_PARAMS_BYTE */
    uint8_t as_byte;
    /* KETCUBE_TERMINAL_PARAMS_UINT8 */
    uint8_t as_uint8;
    /* KETCUBE_TERMINAL_PARAMS_INT8 */
    uint8_t as_int8;
    /* KETCUBE_TERMINAL_PARAMS_MODULEID */
    struct {
        uint16_t module_id;
        ketCube_severity_t severity;
    } as_module_id;
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
 * @brief KETCube terminal packet header (common)
 */
typedef struct ketCube_remoteTerminal_packet_header_t {
    uint8_t opcode : 2;             // 2 bit opcode
    uint8_t is_16b_moduleid : 1;    // flag - is moduleid 16 bit?
    uint8_t rfu : 1;                // flag - RFU
    uint8_t seq : 4;                // sequence number (4 bits)
    uint8_t coreApiVersion;         // core API version (always 8bit)
} ketCube_remoteTerminal_packet_header_t;

/**
 * @brief KETCube terminal single command packet header
 */
typedef struct ketCube_remoteTerminal_single_cmd_header_t {
    union {
        uint8_t moduleId_8bit;      // 8bit module ID
        uint16_t moduleId_16bit;    // 16bit module ID
    } module_id;  // length determined by common header is_16b_moduleid flag
} ketCube_remoteTerminal_single_cmd_header_t;

/**
 * @brief KETCube terminal batch command packet header (header of each command
 *        in batch)
 */
typedef struct ketCube_remoteTerminal_batch_cmd_header_t {
    uint8_t length;             // length of batch block (single cmd)
    ketCube_remoteTerminal_single_cmd_header_t cmdHeader;
                                // batch command always comprises a single cmd
} ketCube_remoteTerminal_batch_cmd_header_t;

#pragma pack(pop)

/** @defgroup KETCube_terminal_common_var KETCube terminal shared variables
  * @brief KETCube terminal shared variables (for local and remote context)
  * @ingroup KETCube_Terminal
  * @{
  */

extern ketCube_terminal_paramSet_t commandIOParams;
extern ketCube_terminal_command_errorCode_t commandErrorCode;

/**
  * @}
  */

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
        case KETCUBE_TERMINAL_PARAMS_MODULEID:
            return 1;
        case KETCUBE_TERMINAL_PARAMS_INT32_PAIR:
            return 2;
    }
}


/**
 * @brief Returns parameter length in bytes
 */
static inline int ketCube_terminal_GetIOParamsMinStaticLength(
    ketCube_terminal_paramSet_type_t type)
{
    switch (type)
    {
        case KETCUBE_TERMINAL_PARAMS_NONE:
        default:
            return 0;
        case KETCUBE_TERMINAL_PARAMS_BOOLEAN:
            return sizeof(bool);
        case KETCUBE_TERMINAL_PARAMS_INT8:
            return sizeof(int8_t);
        case KETCUBE_TERMINAL_PARAMS_BYTE:
            return sizeof(uint8_t);
        case KETCUBE_TERMINAL_PARAMS_MODULEID:
            return sizeof(uint16_t) + 1;
        case KETCUBE_TERMINAL_PARAMS_STRING:
            return 1; /* at least null terminator */
        case KETCUBE_TERMINAL_PARAMS_INT32:
            return sizeof(int32_t);
        case KETCUBE_TERMINAL_PARAMS_UINT32:
            return sizeof(uint32_t);
        case KETCUBE_TERMINAL_PARAMS_INT32_PAIR:
            return (2 * sizeof(int));
        case KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY:
            return 0;
    }
}

/**
 * @brief Returns parameter length in bytes
 */
static inline int ketCube_terminal_GetIOParamsLength(
    ketCube_terminal_paramSet_type_t type)
{
    int len = ketCube_terminal_GetIOParamsMinStaticLength(type);
    
    switch (type)
    {
        default:
            return len;
        case KETCUBE_TERMINAL_PARAMS_STRING:
            return (int)strlen(commandIOParams.as_string);
        case KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY:
            return (int)commandIOParams.as_byte_array.length;
    }
}

/**
* @brief  KETCube terminal command definition.
*/
typedef struct ketCube_terminal_cmd_t {
    char * cmd;                       ///< command format
    char * descr;                     ///< Human-readable command description/help
    ketCube_terminal_command_flags_t flags;           ///< command flags
    ketCube_terminal_paramSet_type_t outputSetType;   ///< cmd output parameter type
    ketCube_terminal_paramSet_type_t paramSetType;    ///< cmd input parameter type
    union
    {
        void (*callback) (void);                      ///< Ptr to a custom callback
        struct ketCube_terminal_cmd_t * subCmdList;   ///< Ptr to a subcommand list (if this is root command)
        struct ketCube_cfg_varDescr_t * cfgVarPtr;    ///< The configuration variable descriptor - RAM/EEPROM (use for generic commands)
    } settingsPtr;                    ///< Command settings - depends on flags
    ketCube_moduleID_t moduleId;
} ketCube_terminal_cmd_t;

extern ketCube_terminal_cmd_t ketCube_terminal_commands[];

/** @defgroup KETCube_terminal_common_fnc KETCube terminal shared functions
  * @brief KETCube terminal shared functions (for local and remote context)
  * @ingroup KETCube_Terminal
  * @{
  */

extern void ketCube_terminal_printCmdList(ketCube_terminal_cmd_t * parent,
                                   ketCube_terminal_cmd_t * cmdList,
                                   ketCube_terminal_command_flags_t * contextFlags);

extern ketCube_terminal_cmdCheckResult_t ketCube_terminal_checkCmdContext(
    ketCube_terminal_cmd_t* command, bool local);
extern bool ketCube_terminal_checkCmdSubtreeContext(
    ketCube_terminal_command_flags_t* flags);

extern void ketCube_terminal_execute(ketCube_terminal_cmd_t* cmd,
    ketCube_terminal_command_flags_t activeFlags);

/**
  * @}
  */

#endif
