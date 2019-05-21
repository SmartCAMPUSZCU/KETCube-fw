/**
 * @file    ketCube_remote_terminal.c
 * @author  Martin Ubl
 * @version 0.1
 * @date    2019-03-30
 * @brief   This file contains the KETCube remote terminal
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

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "utilities.h"

#include "ketCube_remote_terminal.h"

#define KETCUBE_REMOTE_TERMINAL_BUF_SIZE 64

/* remote terminal packets are copied here */
static char deferCmdBuf[KETCUBE_REMOTE_TERMINAL_BUF_SIZE];
/* length of deferred command packet */
static int deferCmdBufLen = 0;
/* boolean flag - is something deferred? */
static bool isCmdDefered = FALSE;
/* buffer for building command response */
static char responseBuf[KETCUBE_REMOTE_TERMINAL_BUF_SIZE];

/* stored response function (to be able to use correct radio driver) */
static ketCube_remoteTerminal_responseFnc_t responseFnc;

/**
 * @brief Appends error output to command response buffer
 *
 * @param err error code
 * @param response pointer to response buffer (iterator; will be modified)
 * @param responseLen current response buffer length (will be modified)
 */
static void ketCube_remoteTerminal_sendError(
    ketCube_terminal_command_errorCode_t err,
    char** response,
    int* responseLen)
{
    // all errors are 1 byte long and contain no additional info
    
    (*response)[0] = (char)err;
    (*response)++;
    *responseLen += 1;
}

/**
 * @brief After validating context, this function executes commands, if
 *        parameters are correct and the command could be executed
 *
 * @param cmd command pointer
 * @param activeFlags currently used flags
 * @param paramBuf remaining command bytes pointer (contains parameters)
 * @param bufLen remaining command bytes buffer length (not necesarilly exclusive)
 * @param responseTarget pointer to response buffer (iterator; will be modified)
 * @param responseLen current response buffer length (will be modified)
 */
static void ketCube_remoteTerminal_tryExecuteCmd(ketCube_terminal_cmd_t* cmd,
    ketCube_terminal_command_flags_t activeFlags, char* paramBuf, int bufLen,
    char** responseTarget, int* responseLen)
{
    ketCube_terminal_cmdCheckResult_t result =
                    ketCube_terminal_checkCmdContext(cmd, FALSE);
    
    if (result == KETCUBE_CMD_CHECK_FAILED_CONTEXT_ONLY_LOCAL
        || result == KETCUBE_CMD_CHECK_FAILED_DEFINITION)
    {
        ketCube_remoteTerminal_sendError(
                    KETCUBE_TERMINAL_CMD_ERR_FAILED_CONTEXT,
                    responseTarget, responseLen);
        return;
    }

    if (result != KETCUBE_CMD_CHECK_OK)
    {
        ketCube_remoteTerminal_sendError(
                    KETCUBE_TERMINAL_CMD_ERR_UNSPECIFIED_ERROR,
                    responseTarget, responseLen);
        return;
    }
    
    /* here we are interested in minimal size the params would fit into;
       this leaves out e.g. strings, byte arrays and other non-static stuff,
       which has minimum of one or zero bytes*/
    int paramLen = ketCube_terminal_GetIOParamsMinStaticLength(cmd->paramSetType);
    if (bufLen < paramLen)
    {
        ketCube_remoteTerminal_sendError(
                    KETCUBE_TERMINAL_CMD_ERR_INVALID_PARAMS,
                    responseTarget, responseLen);
        return;
    }
    memcpy(&commandIOParams, paramBuf, bufLen);

    ketCube_terminal_execute(cmd, activeFlags);
    
    if (commandErrorCode != KETCUBE_TERMINAL_CMD_ERR_OK)
    {
        ketCube_remoteTerminal_sendError(commandErrorCode,
                                         responseTarget, responseLen);
        return;
    }
    
    paramLen = ketCube_terminal_GetIOParamsLength(cmd->outputSetType);

    (*responseTarget)[0] = KETCUBE_TERMINAL_CMD_ERR_OK;

    if (paramLen > 0) {
        memcpy(&(*responseTarget)[1], &commandIOParams, paramLen);
    }
    
    *responseTarget += 1 + paramLen;
    *responseLen = 1 + paramLen;
}

/**
 * @brief Defers command to be processed outside radio IRQ context
 *        (in next base period)
 *
 * @param bytes incoming bytes pointer
 * @param len incoming bytes count
 * @param respFnc response function pointer to use for response
 */
int ketCube_remoteTerminal_deferCmd(char* bytes, int len,
    ketCube_remoteTerminal_responseFnc_t respFnc)
{
    memcpy(deferCmdBuf, bytes, len);
    deferCmdBufLen = len;
    responseFnc = respFnc;
    
    isCmdDefered = TRUE;
    
    return TRUE;
}

/**
 * @brief Processes a command from byte buffer
 *
 * @param startItr start pointer (where the actual command block begins)
 * @param len length of command block in bytes
 * @param response pointer to response buffer (iterator; will be modified)
 * @param responseLen current response buffer length (will be modified)
 */
static int ketCube_remoteTerminal_processCmd(char* startItr, int len,
    char** response, int* responseLen)
{
    uint16_t cmdIndex;
    int curFindIdx;
    bool found;
    char* byteItr;
    
    ketCube_terminal_command_flags_t activeFlags;
    ketCube_terminal_cmd_t* cmdList;
    
    byteItr = startItr;
    curFindIdx = *byteItr;
    cmdIndex = 0;
    cmdList = ketCube_terminal_commands;

    found = FALSE;
    // find in cmd tree root
    for (; cmdList[cmdIndex].cmd != NULL; )
    {
        if (cmdIndex == curFindIdx)
        {
            found = TRUE;
               
            byteItr++;

            curFindIdx = (int)byteItr[0] | (int)byteItr[1] << 8;
            break;
        }
        
        cmdIndex++;
    }

    if (found != TRUE)
    {
        ketCube_remoteTerminal_sendError(
                            KETCUBE_TERMINAL_CMD_ERR_COMMAND_NOT_FOUND,
                            response, responseLen);
        return FALSE;
    }
    
    activeFlags = cmdList[cmdIndex].flags;
    
    if (activeFlags.isGeneric
        && activeFlags.isGroup
        && (activeFlags.isSetCmd || activeFlags.isShowCmd))
    {
        activeFlags.isGroup = FALSE;
        cmdList = cmdList[cmdIndex].settingsPtr.subCmdList;
        
        cmdIndex = 0;
        found = FALSE;
        for ( ; cmdList[cmdIndex].cmd != NULL; )
        {
            if (cmdList[cmdIndex].moduleId == curFindIdx)
            {
                found = TRUE;
                byteItr += 2;
                curFindIdx = *byteItr;
                break;
            }
            
            cmdIndex++;
        }

        if (found != TRUE)
        {
            ketCube_remoteTerminal_sendError(
                                    KETCUBE_TERMINAL_CMD_ERR_COMMAND_NOT_FOUND,
                                    response, responseLen);
            return TRUE;
        }
        
        if (!cmdList[cmdIndex].flags.isGroup)
        {
            ketCube_remoteTerminal_sendError(
                                    KETCUBE_TERMINAL_CMD_ERR_COMMAND_NOT_FOUND,
                                    response, responseLen);
            return TRUE;
        }

        ketCube_terminal_andCmdFlags(&activeFlags, &activeFlags, &(cmdList[cmdIndex].flags));
        cmdList = cmdList[cmdIndex].settingsPtr.subCmdList;
        cmdIndex = 0;
        found = FALSE;
        
        for ( ; cmdList[cmdIndex].cmd != NULL; )
        {
            if (cmdIndex == curFindIdx)
            {
                ketCube_terminal_andCmdFlags(&activeFlags,
                                             &activeFlags,
                                             &(cmdList[cmdIndex].flags));
                
                if (!cmdList[cmdIndex].flags.isGroup)
                {
                    found = TRUE;
                    break;
                }
                
                cmdList = cmdList[cmdIndex].settingsPtr.subCmdList;
                cmdIndex = 0;
                byteItr++;
                curFindIdx = *byteItr;
                continue;
            }

            cmdIndex++;
        }
        
        if (found != TRUE)
        {
            ketCube_remoteTerminal_sendError(
                                    KETCUBE_TERMINAL_CMD_ERR_COMMAND_NOT_FOUND,
                                    response, responseLen);
            return TRUE;
        }
        
        if (ketCube_terminal_checkCmdSubtreeContext(&activeFlags) != TRUE)
        {
            ketCube_remoteTerminal_sendError(
                                    KETCUBE_TERMINAL_CMD_ERR_FAILED_CONTEXT,
                                    response, responseLen);
            return TRUE;
        }
        
        // move to next byte (first param byte)
        byteItr++;
        
        ketCube_remoteTerminal_tryExecuteCmd(&cmdList[cmdIndex], activeFlags,
                        byteItr, len - (byteItr - startItr),
                        response, responseLen);
    }
    else if (!cmdList[cmdIndex].flags.isGroup)
    {
        ketCube_remoteTerminal_tryExecuteCmd(&cmdList[cmdIndex], activeFlags,
                        byteItr, len - (byteItr - startItr),
                        response, responseLen);
    }
    else
    {
        ketCube_remoteTerminal_sendError(KETCUBE_TERMINAL_CMD_ERR_NOT_SUPPORTED,
                                         response, responseLen);
    }
    
    return TRUE;
}

/**
 * @brief Processes single command from buffer (single cmd opcode)
 *
 * @param startItr start pointer (where the actual command block begins)
 * @param len length of command block in bytes
 * @param response pointer to response buffer (iterator; will be modified)
 * @param responseLen current response buffer length (will be modified)
 */
static int ketCube_remoteTerminal_processSingleCmd(char* startItr, int len,
    char** response, int* responseLen)
{
    int cmdLen, res;
    
    res = ketCube_remoteTerminal_processCmd(startItr, len, response, &cmdLen);
    
    *responseLen += cmdLen;
    
    return res;
}

/**
 * @brief Processes command batch from buffer (command batch opcode)
 *
 * @param cmdBuf start pointer (where the actual command block begins)
 * @param length length of command block in bytes
 * @param response pointer to response buffer (iterator; will be modified)
 * @param responseLen current response buffer length (will be modified)
 */
int ketCube_remoteTerminal_processCommandBatch(char* cmdBuf, int length,
    char** response, int* responseLen)
{
    int lenCtr;
    int curLen;
    char* lenTarget;
    bool allResult = TRUE;
    int cmdResponseLen;
    
    for (lenCtr = 0; lenCtr < length; )
    {
        curLen = cmdBuf[lenCtr];
        cmdResponseLen = 0;
        
        lenTarget = *response;
        (*response)++;
        (*responseLen)++;
        
        allResult |= ketCube_remoteTerminal_processCmd(
                        &cmdBuf[lenCtr + 1], curLen,
                        response, &cmdResponseLen);
        
        *responseLen += cmdResponseLen;
        *lenTarget = cmdResponseLen;
        lenCtr += 1 + curLen;
    }
    
    return allResult;
}

/**
 * @brief Processes deferred command, if any
 *
 * @param response pointer to response buffer (will be filled with actual address)
 * @param responseLen where to put response length
 */
int ketCube_remoteTerminal_processDeferredCmd(char** response, int* responseLen)
{
    char* byteItr;
    char* respItr;
    int remLength;

    ketCube_terminal_command_opcode_t termOpcode;
    uint8_t seq;
    
    if (isCmdDefered != TRUE)
        return FALSE;

    isCmdDefered = FALSE;

    if (deferCmdBufLen < 2)
        return FALSE;

    termOpcode = (ketCube_terminal_command_opcode_t)deferCmdBuf[0];
    seq = deferCmdBuf[1];

    *responseLen = 0;
    *response = responseBuf;
    respItr = responseBuf;
    
    responseBuf[0] = termOpcode;
    responseBuf[1] = seq;
    
    respItr += 2;
    *responseLen += 2;

    byteItr = deferCmdBuf + 2;
    remLength = deferCmdBufLen - 2;

    switch (termOpcode)
    {
        case KETCUBE_TERMINAL_OPCODE_CMD:
            return ketCube_remoteTerminal_processSingleCmd(byteItr, remLength,
                                                &respItr, responseLen);
        case KETCUBE_TERMINAL_OPCODE_BATCH:
            return ketCube_remoteTerminal_processCommandBatch(byteItr, remLength,
                                                &respItr, responseLen);
    }
    
    return FALSE;
}

/**
 * @brief Interface function - process deferred command, if any
 *
 */
void ketCube_remoteTerminal_ProcessCMD(void)
{
    int respLen;
    char* resp;
    
    if (isCmdDefered != TRUE) {
        return;
    }
    
    if (ketCube_remoteTerminal_processDeferredCmd(&resp, &respLen) == TRUE) {
        responseFnc((uint8_t*)resp, (uint8_t*)&respLen);
    }
}
