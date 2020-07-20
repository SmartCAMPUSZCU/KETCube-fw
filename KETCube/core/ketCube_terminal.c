/**
 * @file    ketCube_terminal.c
 * @author  Jan Belohoubek
 * @author  Martin Ubl
 * @version 0.2
 * @date    2018-10-10
 * @brief   This file contains the KETCube Terminal
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

#include "ketCube_terminal.h"
#include "ketCube_mainBoard.h"
#include "ketCube_common.h"
#include "ketCube_cfg.h"
#include "ketCube_coreCfg.h"
#include "ketCube_eeprom.h"
#include "ketCube_modules.h"
#include "ketCube_uart.h"

// BEGIN of USART configuration
#define KETCUBE_TERMINAL_USART_INSTANCE                  USART1
#define KETCUBE_TERMINAL_USART_IRQn                      USART1_IRQn
#define KETCUBE_TERMINAL_USART_BR                        9600
#define KETCUBE_TERMINAL_USART_CHANNEL                   KETCUBE_UART_CHANNEL_1
#define KETCUBE_TERMINAL_USART_TX_AF                     GPIO_AF4_USART1
#define KETCUBE_TERMINAL_USART_RX_AF                     GPIO_AF4_USART1

#define KETCUBE_TERMINAL_USART_CLK_ENABLE()              __USART1_CLK_ENABLE();
#define KETCUBE_TERMINAL_USART_RX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()
#define KETCUBE_TERMINAL_USART_TX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()
#define KETCUBE_TERMINAL_USART_FORCE_RESET()             __USART1_FORCE_RESET()
#define KETCUBE_TERMINAL_USART_RELEASE_RESET()           __USART1_RELEASE_RESET()

#define KETCUBE_TERMINAL_USART_TX_PIN                    KETCUBE_MAIN_BOARD_PIN_IO2_PIN
#define KETCUBE_TERMINAL_USART_TX_GPIO_PORT              KETCUBE_MAIN_BOARD_PIN_IO2_PORT
#define KETCUBE_TERMINAL_USART_RX_PIN                    KETCUBE_MAIN_BOARD_PIN_IO1_PIN
#define KETCUBE_TERMINAL_USART_RX_GPIO_PORT              KETCUBE_MAIN_BOARD_PIN_IO1_PORT

static UART_HandleTypeDef ketCube_terminal_UsartHandle;
static ketCube_UART_descriptor_t ketCube_terminal_UsartDescriptor;

#define USART_BUFFER_SIZE                                256    // It must be 256, because overflows control circular buffer properties

static volatile uint8_t usartRxWrite = 0;
static volatile uint8_t usartRxRead = 0;
static char usartRxBuffer[USART_BUFFER_SIZE];

static char usartTxBuffer[USART_BUFFER_SIZE];

/* helper functions definition */
static uint8_t ketCube_terminal_getNextParam(uint8_t ptr);

// END of USART configuration

static ketCube_terminal_buffer_t
    commandHistory[KETCUBE_TERMINAL_HISTORY_LEN];
static char *commandBuffer = (char *) &(commandHistory[0].buffer[0]);

static uint8_t *commandPtr = (uint8_t *) & (commandHistory[0].ptr);
static uint8_t commandHistoryPtr = 0;
/* Index of the first command parameter in buffer */
static uint8_t commandParamsPos;

void ketCube_terminal_usartIoInit(void)
{
    /* Enable USART1 clock */
    KETCUBE_TERMINAL_USART_CLK_ENABLE();

    /* UART TX GPIO pin configuration  */
    ketCube_UART_SetupPin(KETCUBE_TERMINAL_USART_TX_GPIO_PORT,
                          KETCUBE_TERMINAL_USART_TX_PIN,
                          KETCUBE_TERMINAL_USART_TX_AF
                         );
}

void ketCube_terminal_usartIoDeInit(void)
{
    KETCUBE_TERMINAL_USART_RX_GPIO_CLK_ENABLE();

    ketCube_GPIO_Release(KETCUBE_TERMINAL_USART_TX_GPIO_PORT,
                         KETCUBE_TERMINAL_USART_TX_PIN);

    __HAL_UART_ENABLE_IT(&ketCube_terminal_UsartHandle, UART_IT_WUF);
    HAL_UARTEx_EnableStopMode(&ketCube_terminal_UsartHandle);
}

void ketCube_terminal_usartRx(void)
{
#if (USART_BUFFER_SIZE == 256)
    // buffer is sized to be 256 bytes long; usartRxWrite automatically overflows at the end of buffer ...
    usartRxWrite = (usartRxWrite + 1) & 0xFF;
#else
    usartRxWrite = (usartRxWrite + 1) % USART_BUFFER_SIZE;
#endif
    HAL_UART_Receive_IT(&ketCube_terminal_UsartHandle,
                        (uint8_t *) & usartRxBuffer[usartRxWrite], 1);
}

void ketCube_terminal_usartTx(void)
{
    HAL_NVIC_ClearPendingIRQ(KETCUBE_TERMINAL_USART_IRQn);
    HAL_UART_Receive_IT(&ketCube_terminal_UsartHandle,
                        (uint8_t *) & usartRxBuffer[usartRxWrite], 1);
}

void ketCube_terminal_usartErrorCallback(void)
{
    HAL_UART_Receive_IT(&ketCube_terminal_UsartHandle,
                        (uint8_t *) & usartRxBuffer[usartRxWrite], 1);
}

void ketCube_terminal_usartWakeupCallback(void)
{
    HAL_UART_Receive_IT(&ketCube_terminal_UsartHandle,
                        (uint8_t *) & usartRxBuffer[usartRxWrite], 1);
}

void ketCube_terminal_UsartPrintVa(char *format, va_list args)
{
    uint8_t len, i;

    len = vsprintf(&(usartTxBuffer[0]), format, args);

    for (i = 0; i < len; i++) {
        HAL_UART_Transmit(&ketCube_terminal_UsartHandle,
                          (uint8_t *) & (usartTxBuffer[i]), 1, 300);
    }

    if (ketCube_terminal_UsartHandle.RxState == HAL_UART_STATE_READY) {
        // TODO This should never happen, but "(sh)it happens" !! Why?
        // resolved by adding HAL_UART_Receive_IT to vcom_Print()
        //ketCube_terminal_Println("VCOM UART :: %d ", usartRxWrite);

        // This causes, that ketCube_terminal_usartTx will restore IRQ ...
        HAL_NVIC_SetPendingIRQ(KETCUBE_TERMINAL_USART_IRQn);
    }

}

void ketCube_terminal_UsartPrint(char *format, ...)
{
    va_list args;
    va_start(args, format);

    ketCube_terminal_UsartPrintVa(format, args);

    va_end(args);
}

bool IsNewCharReceived(void)
{
    if (usartRxWrite != usartRxRead) {
        return TRUE;
    }

    return FALSE;
}

char GetNewChar(void)
{
    char byte;

    if (usartRxWrite != usartRxRead) {
        byte = usartRxBuffer[usartRxRead];
#if (USART_BUFFER_SIZE == 256)
        // buffer is sized to be 256 bytes long; usartRxRead automatically overflows at the end of buffer ...
        usartRxRead = (usartRxRead + 1) & 0xFF;
#else
        usartRxRead = (usartRxRead + 1) % USART_BUFFER_SIZE;
#endif
        return byte;
    }
    return '\0';
}

/**
  * @brief Init terminal
  *
  */
void ketCube_terminal_Init(void)
{
    ketCube_terminal_command_flags_t context = {
        .isGroup  = TRUE,
        .isLocal  = TRUE,
        .isEnvCmd = TRUE,
        .isEEPROM = TRUE,
        .isRAM    = TRUE,
    };
    
    /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
    ketCube_terminal_UsartHandle.Instance =
        KETCUBE_TERMINAL_USART_INSTANCE;
    ketCube_terminal_UsartHandle.Init.BaudRate = KETCUBE_TERMINAL_USART_BR;
    ketCube_terminal_UsartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    ketCube_terminal_UsartHandle.Init.StopBits = UART_STOPBITS_1;
    ketCube_terminal_UsartHandle.Init.Parity = UART_PARITY_NONE;
    ketCube_terminal_UsartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    ketCube_terminal_UsartHandle.Init.Mode = UART_MODE_TX_RX;

    __HAL_RCC_USART1_CONFIG(RCC_USART1CLKSOURCE_HSI);

    /* register callbacks in generic UART manager */
    ketCube_terminal_UsartDescriptor.handle =
        &ketCube_terminal_UsartHandle;
    ketCube_terminal_UsartDescriptor.irqNumber =
        KETCUBE_TERMINAL_USART_IRQn;
    ketCube_terminal_UsartDescriptor.irqPriority = 0x1;
    ketCube_terminal_UsartDescriptor.irqSubPriority = 0;
    ketCube_terminal_UsartDescriptor.fnIoInit =
        &ketCube_terminal_usartIoInit;
    ketCube_terminal_UsartDescriptor.fnIoDeInit =
        &ketCube_terminal_usartIoDeInit;
    ketCube_terminal_UsartDescriptor.fnIRQCallback =
        &ketCube_terminal_usartTx;
    ketCube_terminal_UsartDescriptor.fnReceiveCallback =
        &ketCube_terminal_usartRx;
    ketCube_terminal_UsartDescriptor.fnTransmitCallback =
        &ketCube_terminal_usartTx;
    ketCube_terminal_UsartDescriptor.fnErrorCallback =
        &ketCube_terminal_usartErrorCallback;
    ketCube_terminal_UsartDescriptor.fnWakeupCallback =
        &ketCube_terminal_usartWakeupCallback;

    /* Initial GPIO configuration for UART */
    ketCube_UART_SetupPin(KETCUBE_TERMINAL_USART_RX_GPIO_PORT,
                          KETCUBE_TERMINAL_USART_RX_PIN,
                          KETCUBE_TERMINAL_USART_RX_AF
                         );
     ketCube_UART_SetupPin(KETCUBE_TERMINAL_USART_TX_GPIO_PORT,
                           KETCUBE_TERMINAL_USART_TX_PIN,
                          KETCUBE_TERMINAL_USART_TX_AF
                         );
        
    if (ketCube_UART_RegisterHandle
        (KETCUBE_TERMINAL_USART_CHANNEL,
         &ketCube_terminal_UsartDescriptor) != KETCUBE_CFG_DRV_OK) {
        ketCube_common_BasicErrorHandler();
    }

    HAL_UART_Receive_IT(&ketCube_terminal_UsartHandle,
                        (uint8_t *) & usartRxBuffer[usartRxWrite], 1);

    commandBuffer[0] = 0x00;

    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("Welcome to %s Command-line Interface",
                            KETCUBE_CFG_DEV_NAME);
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("-----------------------------------------");
    KETCUBE_TERMINAL_ENDL();
#ifdef KETCUBE_VERSION
    KETCUBE_TERMINAL_PRINTF("Version: %s (build: %s)", KETCUBE_VERSION, KETCUBE_BUILD_ID);
    KETCUBE_TERMINAL_ENDL();
#endif
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("Use [TAB] key to show build-in help for current command");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("Use [ENTER] key to execute current command");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("Use [+]/[-] keys to browse command history");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();

    ketCube_terminal_printCmdList(NULL, ketCube_terminal_commands, &context);
    
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PROMPT();
}

/**
 * @brief Parse command arguments and validate them
 * 
 * @param command ptr to current command
 * @param contextFlags ptr to context flags
 * 
 * @retval TRUE if parameters parsed successfuly, else return FALSE
 * 
 */
static bool ketCube_terminal_parseParams(ketCube_terminal_cmd_t* command,
                                         ketCube_terminal_command_flags_t *contextFlags)
{
    uint8_t ptr = 0;
    uint8_t len = 0;
    int i, tmpCmdLen, tmpSeverity;
    char *endptr;
    
    if ((contextFlags->isGeneric == TRUE) 
        && (contextFlags->isShowCmd == TRUE)) {
            /* expect no params ... */
            return TRUE;
    }
    
    switch (command->paramSetType)
    {
        default:
        case KETCUBE_TERMINAL_PARAMS_NONE:
            return TRUE;
        case KETCUBE_TERMINAL_PARAMS_STRING:
        {
            strncpy(commandIOParams.as_string,
                    &(commandBuffer[commandParamsPos]),
                    KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH);
            return TRUE;
        }
        case KETCUBE_TERMINAL_PARAMS_BYTE:
        {
            commandIOParams.as_byte =
                        strtol(&(commandBuffer[commandParamsPos]), &endptr, 10);

            /* no integer on input */
            if (endptr == &(commandBuffer[commandParamsPos])) {
                return FALSE;
            }
            return TRUE;
        }
        case KETCUBE_TERMINAL_PARAMS_BOOLEAN:
        {
            commandIOParams.as_uint32 =
                        strtoul(&(commandBuffer[commandParamsPos]), &endptr, 10);

            /* no integer on input */
            if (endptr == &(commandBuffer[commandParamsPos])) {
                return FALSE;
            }
            
            if (commandIOParams.as_uint32 != 0) {
                commandIOParams.as_bool = TRUE;
            } else {
                commandIOParams.as_bool = FALSE;
            }
            return TRUE;
        }
        case KETCUBE_TERMINAL_PARAMS_MODULEID:
        {
            commandIOParams.as_module_id.module_id = (uint16_t)-1;
            commandIOParams.as_module_id.severity = KETCUBE_CORECFG_DEFAULT_SEVERITY;
            
            for (i = KETCUBE_LISTS_MODULEID_FIRST; i < ketCube_modules_CNT; i++)
            {
                tmpCmdLen = strlen(&(ketCube_modules_List[i].name[0]));
        
                if (strncmp(&(ketCube_modules_List[i].name[0]), &(commandBuffer[commandParamsPos]), tmpCmdLen) == 0) {
                    
                    commandIOParams.as_module_id.module_id = ketCube_modules_List[i].id;
                    
                    if (commandBuffer[commandParamsPos + tmpCmdLen] == 0x00) {
                        break;
                    }
                    
                    if (commandBuffer[commandParamsPos + tmpCmdLen] == ' ') {
                        sscanf(&(commandBuffer[commandParamsPos + tmpCmdLen + 1]), "%d", (int*)&tmpSeverity);
                        commandIOParams.as_module_id.severity = (ketCube_severity_t)tmpSeverity;
                        if (commandIOParams.as_module_id.severity > KETCUBE_CFG_SEVERITY_DEBUG) {
                            commandIOParams.as_module_id.severity = KETCUBE_CORECFG_DEFAULT_SEVERITY;
                        }
                        break;
                    }
                }
            }
            
            if (i == ketCube_modules_CNT)
                return FALSE;
            
            return TRUE;
        }
        case KETCUBE_TERMINAL_PARAMS_INT32:
        {
            commandIOParams.as_int32 =
                        strtol(&(commandBuffer[commandParamsPos]), &endptr, 10);

            /* no integer on input */
            if (endptr == &(commandBuffer[commandParamsPos])) {
                return FALSE;
            }
            return TRUE;
        }
        case KETCUBE_TERMINAL_PARAMS_UINT32:
        {
            commandIOParams.as_uint32 =
                        strtoul(&(commandBuffer[commandParamsPos]), &endptr, 10);

            /* no integer on input */
            if (endptr == &(commandBuffer[commandParamsPos])) {
                return FALSE;
            }
            return TRUE;
        }
        case KETCUBE_TERMINAL_PARAMS_INT32_PAIR:
        {
            commandIOParams.as_int32_pair.first
                        = strtol(&(commandBuffer[commandParamsPos]),
                                 &endptr, 10);

            /* no integer on input */
            if (endptr == &(commandBuffer[commandParamsPos])) {
                return FALSE;
            }

            ptr = ketCube_terminal_getNextParam(commandParamsPos);
            /* no next parameter */
            if (ptr == 0) {
                return FALSE;
            }
            commandIOParams.as_int32_pair.second
                        = strtol(&(commandBuffer[ptr]), &endptr, 10);

            /* no integer on input */
            if (endptr == &(commandBuffer[ptr])) {
                return FALSE;
            }
            return TRUE;
        }
        case KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY:
        {
            len = ketCube_common_Min(strlen(&(commandBuffer[commandParamsPos])), KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH);
            
            if (ketCube_common_IsHexString(&(commandBuffer[commandParamsPos]), len) == FALSE) {
                return FALSE;
            }
            
            ketCube_common_Hex2Bytes((uint8_t *) &(commandIOParams.as_byte_array.data[0]), &(commandBuffer[commandParamsPos]), len);
            
            commandIOParams.as_byte_array.length = len/2;
            
            return TRUE;
        }
    }
}

/**
 * @brief Parse and print command outputs
 * 
 */
static void ketCube_terminal_printCommandOutput(ketCube_terminal_cmd_t* command)
{
    uint16_t i;
    
    if (command->outputSetType == KETCUBE_TERMINAL_PARAMS_NONE) {
        return;
    }
    
    KETCUBE_TERMINAL_PRINTF("%s returned: ", command->cmd);
    
    switch (command->outputSetType)
    {
        case KETCUBE_TERMINAL_PARAMS_BOOLEAN:
        {
            if (commandIOParams.as_bool == TRUE) {
                KETCUBE_TERMINAL_PRINTF("TRUE");
            } else {
                KETCUBE_TERMINAL_PRINTF("FALSE");
            }
            break;
        }
        case KETCUBE_TERMINAL_PARAMS_STRING:
        {
            KETCUBE_TERMINAL_PRINTF("%s", commandIOParams.as_string);
            break;
        }
        case KETCUBE_TERMINAL_PARAMS_INT32:
        {
            KETCUBE_TERMINAL_PRINTF("%d", commandIOParams.as_int32);
            break;
        }
        case KETCUBE_TERMINAL_PARAMS_UINT32:
        {
            KETCUBE_TERMINAL_PRINTF("%d", commandIOParams.as_uint32);
            break;
        }
        case KETCUBE_TERMINAL_PARAMS_BYTE:
        {
            KETCUBE_TERMINAL_PRINTF("%d", commandIOParams.as_byte);
            break;
        }
        case KETCUBE_TERMINAL_PARAMS_INT32_PAIR:
        {
            KETCUBE_TERMINAL_PRINTF("%d, %d",
                                    commandIOParams.as_int32_pair.first,
                                    commandIOParams.as_int32_pair.second);
            break;
        }
        case KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY:
        {
            for (i = 0; i < commandIOParams.as_byte_array.length; i++) {
                KETCUBE_TERMINAL_PRINTF("%02X-",commandIOParams.as_byte_array.data[i]);
            }
            KETCUBE_TERMINAL_PRINTF("\b \b");
            break;
        }
        case KETCUBE_TERMINAL_PARAMS_MODULEID:
        {
            for (i = KETCUBE_LISTS_MODULEID_FIRST; i < ketCube_modules_CNT; i++) {
                if (ketCube_modules_List[i].id == commandIOParams.as_module_id.module_id) {
                    KETCUBE_TERMINAL_PRINTF("%s", ketCube_modules_List[i].name);
                    break;
                }
            }
            
            if (i == ketCube_modules_CNT) {
                KETCUBE_TERMINAL_PRINTF("invalid module");
            }
        }
        default:
        {
            KETCUBE_TERMINAL_PRINTF("<unknown return type>");
            break;
        }
    }
    
    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Processes command error code, prints the result
 */
static int ketCube_terminal_processCommandErrors()
{
    if (commandErrorCode == KETCUBE_TERMINAL_CMD_ERR_OK) {
        KETCUBE_TERMINAL_PRINTF("Command execution OK");
        KETCUBE_TERMINAL_ENDL();
        return 0;
    }
    
    KETCUBE_TERMINAL_PRINTF("Command returned error: ");
    
    switch (commandErrorCode)
    {
        case KETCUBE_TERMINAL_CMD_ERR_INVALID_PARAMS:
            KETCUBE_TERMINAL_PRINTF("invalid parameters");
            break;
        case KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL:
            KETCUBE_TERMINAL_PRINTF("could not read/write memory");
            break;
        default:
            KETCUBE_TERMINAL_PRINTF("unknown error (%d)", commandErrorCode);
            break;
    }
    
    KETCUBE_TERMINAL_ENDL();
    return 1;
}

bool ketCube_terminal_canRunCommand(ketCube_terminal_cmd_t* command, bool local)
{
    ketCube_terminal_cmdCheckResult_t result =
            ketCube_terminal_checkCmdContext(command, local);
    
    switch (result) {
        case KETCUBE_CMD_CHECK_OK:
            return TRUE;
        case KETCUBE_CMD_CHECK_FAILED_DEFINITION:
            KETCUBE_TERMINAL_PRINTF("Cannot execute command in current context!");
            break;
        case KETCUBE_CMD_CHECK_FAILED_CONTEXT_ONLY_LOCAL:
            KETCUBE_TERMINAL_PRINTF("Cannot execute local-only command"
                                    " in remote terminal!");
            break;
        case KETCUBE_CMD_CHECK_FAILED_CONTEXT_ONLY_REMOTE:
            KETCUBE_TERMINAL_PRINTF("Cannot execute remote-only command"
                                    " in local terminal!");
            break;
    }
    
    KETCUBE_TERMINAL_ENDL();
    return FALSE;
}

/**
  * @brief Execute command-in-buffer
  *
  */
void ketCube_terminal_execCMD(void)
{
    uint16_t cmdIndex, j, cmdBuffIndex;
    bool eq;
    ketCube_terminal_cmd_t* cmdList;
    
    uint8_t commandTreeLevel = 0;
    ketCube_terminal_command_flags_t activeFlags;
    ketCube_terminal_command_flags_t contextFlags;

    KETCUBE_TERMINAL_ENDL();

    if (*commandPtr == 0) {
        KETCUBE_TERMINAL_PROMPT();
        return;
    }

    cmdIndex = 0;
    cmdBuffIndex = 0;
    cmdList = ketCube_terminal_commands;

    while (cmdList[cmdIndex].cmd != NULL) {

        eq = TRUE;
        j = 0;

        while ((cmdList[cmdIndex].cmd[j] != 0x00)
               && (*commandPtr > cmdBuffIndex)) {
            /* compare command characters */
            if (cmdList[cmdIndex].cmd[j] !=
                commandBuffer[cmdBuffIndex]) {
                eq = FALSE;
            }
            j++;
            cmdBuffIndex++;
        }

        /* cause mismatch for command substrings */
        if ((eq == TRUE) && (*commandPtr > cmdBuffIndex)
            && (commandBuffer[cmdBuffIndex] != 0x00)
            && (commandBuffer[cmdBuffIndex] != ' ')) {
            eq = FALSE;
        }

        /* set active flags for root commands */
        if (commandTreeLevel == 0) {
            // set active flags
            contextFlags = cmdList[cmdIndex].flags;
            contextFlags.isGroup = FALSE;
        }
        
        /* move to next command if mismatch */
        if (eq == FALSE) {
            cmdBuffIndex -= j;
            cmdIndex++;
            continue;
        }
        
        /* move to subcommand if exist */
        if ((cmdList[cmdIndex].cmd[j] == 0x00) &&
            (commandBuffer[cmdBuffIndex] != 0x00) &&
            (cmdList[cmdIndex].flags.isGroup == TRUE)) {
            ketCube_terminal_andCmdFlags(&contextFlags, &contextFlags, &(cmdList[cmdIndex].flags));
            commandTreeLevel += 1;
            
            cmdBuffIndex++;     // remove space character: ' '
            cmdList = cmdList[cmdIndex].settingsPtr.subCmdList;
            cmdIndex = 0;
            continue;
        }

        /* execute command */
        if ((eq == TRUE)
            && (cmdList[cmdIndex].cmd[j] == 0x00)
            && ((commandBuffer[cmdBuffIndex] == 0x00)
                || ((commandBuffer[cmdBuffIndex] == ' ')
                    && (ketCube_terminal_ParamSetTypeToCount(
                            cmdList[cmdIndex].paramSetType) != 0)))) {
            commandParamsPos = cmdBuffIndex + 1;
        
            /* set flags for current command */
            ketCube_terminal_andCmdFlags(&activeFlags, &contextFlags, &(cmdList[cmdIndex].flags));
        
            /* Check command flags */
            if (ketCube_terminal_checkCmdSubtreeContext(&activeFlags) == FALSE) {
                /* Command/group is not enabled in this context */
                
                /* Removed because of same command names with different flags are enabled */
                /* KETCUBE_TERMINAL_PRINTF("Command not enabled in current context!");
                KETCUBE_TERMINAL_ENDL();
                KETCUBE_TERMINAL_PROMPT(); */
                
                /* continue with next command -- enable same command names with different flags */
                cmdIndex++;
                cmdBuffIndex -= j;
                continue;
            }
        
            /* we've reached command parsing end, now check if this is a valid
               command or group, parse parameters and eventually execute */
            if ((cmdList[cmdIndex].flags.isGroup == FALSE)
                && (cmdList[cmdIndex].settingsPtr.callback != NULL)
                && (ketCube_terminal_parseParams(&cmdList[cmdIndex], &activeFlags) == TRUE)) {

                /* now we are sure it is a command and have valid params;
                   let's check, whether we can execute it or not */
                if (ketCube_terminal_canRunCommand(&cmdList[cmdIndex], TRUE) == TRUE) {

                    KETCUBE_TERMINAL_PRINTF("Executing command: %s",
                                            cmdList[cmdIndex].cmd);
                    KETCUBE_TERMINAL_ENDL();

                    /* assume no error as default, commands will set error code
                       if needed */
                    commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_OK;
                    
                    /* Execute specific command callback or generic callback */
                    ketCube_terminal_execute(&cmdList[cmdIndex], activeFlags);
                    
                    /* if the command executed OK, print outputs */
                    if (ketCube_terminal_processCommandErrors() == 0) {
                        ketCube_terminal_printCommandOutput(&cmdList[cmdIndex]);
                    }
                }
            } else {
                KETCUBE_TERMINAL_PRINTF("Help for command: %s",
                                        cmdList[cmdIndex].cmd);
                KETCUBE_TERMINAL_ENDL();
                ketCube_terminal_printCmdList(&cmdList[cmdIndex],
                                              cmdList[cmdIndex].settingsPtr.subCmdList,
                                              &activeFlags);
            }

            KETCUBE_TERMINAL_PROMPT();

            commandHistoryPtr =
                (commandHistoryPtr + 1) % KETCUBE_TERMINAL_HISTORY_LEN;
            commandBuffer =
                (char *) &(commandHistory[commandHistoryPtr].buffer[0]);
            commandPtr =
                (uint8_t *) & (commandHistory[commandHistoryPtr].ptr);
            *commandPtr = 0;
            commandBuffer[*commandPtr] = 0x00;

            return;
        }
        cmdIndex++;
        cmdBuffIndex -= j; /* for concurent command names - to resolve different parameter settings */
    }
    
    /* check if command was not found or error raised during command parsing */
    if (cmdList[cmdIndex].cmd == NULL) {
        /* Command was not found */
        KETCUBE_TERMINAL_PRINTF("Command not found!");
        KETCUBE_TERMINAL_ENDL();
        KETCUBE_TERMINAL_PROMPT();
    }

    commandHistoryPtr =
        (commandHistoryPtr + 1) % KETCUBE_TERMINAL_HISTORY_LEN;
    commandBuffer =
        (char *) &(commandHistory[commandHistoryPtr].buffer[0]);
    commandPtr = (uint8_t *) & (commandHistory[commandHistoryPtr].ptr);
    *commandPtr = 0;
    commandBuffer[*commandPtr] = 0x00;

    return;
}

/**
  * @brief Print help related to the current user input
  *
  */
void ketCube_terminal_printCMDHelp(void)
{
    uint16_t cmdIndex, j, cmdBuffIndex, k, l;
    bool eq;
    ketCube_terminal_cmd_t* cmdList;
    
    uint8_t commandTreeLevel = 0;
    ketCube_terminal_command_flags_t activeFlags;
    ketCube_terminal_command_flags_t localContext;

    KETCUBE_TERMINAL_ENDL();

    if (*commandPtr == 0) {
        KETCUBE_TERMINAL_PROMPT();
        return;
    }

    cmdList = ketCube_terminal_commands;
    cmdIndex = 0;
    cmdBuffIndex = 0;

    while (cmdList[cmdIndex].cmd != NULL) {

        eq = TRUE;
        j = 0;

        while ((cmdList[cmdIndex].cmd[j] != 0x00)
               && (*commandPtr > cmdBuffIndex)) {
            /* compare command characters */
            if (cmdList[cmdIndex].cmd[j] !=
                commandBuffer[cmdBuffIndex]) {
                eq = FALSE;
            }
            j++;
            cmdBuffIndex++;
        }

        /* cause mismatch for command substrings */
        if ((eq == TRUE) && (*commandPtr > cmdBuffIndex)
            && (commandBuffer[cmdBuffIndex] != 0x00)
            && (commandBuffer[cmdBuffIndex] != ' ')) {
            eq = FALSE;
        }

        /* set active flags for root commands */
        if (commandTreeLevel == 0) {
            // set active flags
            activeFlags = cmdList[cmdIndex].flags;
            activeFlags.isGroup = FALSE;
        }
        
        /* move to next command if mismatch */
        if (eq == FALSE) {
            cmdBuffIndex -= j;
            cmdIndex++;
            continue;
        }

        /* move to subcommand if exist */
        if ((cmdList[cmdIndex].cmd[j] == 0x00) &&
            (commandBuffer[cmdBuffIndex] != 0x00) &&
            (cmdList[cmdIndex].flags.isGroup == TRUE)) {
            ketCube_terminal_andCmdFlags(&activeFlags, &activeFlags, &(cmdList[cmdIndex].flags));
            commandTreeLevel += 1;
        
            cmdBuffIndex++;     // remove space character: ' '
            cmdList = cmdList[cmdIndex].settingsPtr.subCmdList;
            cmdIndex = 0;
            continue;
        }
        
        if ((eq == TRUE)
            && (cmdList[cmdIndex].cmd[j] == 0x00)
            && ((commandBuffer[cmdBuffIndex] == 0x00))
            && (ketCube_terminal_checkCmdSubtreeContext(&activeFlags) == TRUE)) {
            
            /* set falgs for current command */
            ketCube_terminal_andCmdFlags(&activeFlags, &activeFlags, &(cmdList[cmdIndex].flags));
            
            KETCUBE_TERMINAL_PRINTF("Help for command %s: \t%s",
                                    cmdList[cmdIndex].cmd,
                                    cmdList[cmdIndex].descr);
            KETCUBE_TERMINAL_ENDL();
            if (cmdList[cmdIndex].flags.isGroup) {
                ketCube_terminal_printCmdList(&cmdList[cmdIndex],
                                              cmdList[cmdIndex].settingsPtr.subCmdList,
                                              &activeFlags);
            }
            break;
        } else if (eq == TRUE) {
            /* set falgs for current command */
            ketCube_terminal_andCmdFlags(&activeFlags, &activeFlags, &(cmdList[cmdIndex].flags));
            
            KETCUBE_TERMINAL_PRINTF("Available commands: ");
            KETCUBE_TERMINAL_ENDL();

            k = cmdIndex;
            while (cmdList[k].cmd != NULL) {
                eq = TRUE;
                for (l = 0; l < j; l++) {
                    if (cmdList[cmdIndex].cmd[l] !=
                        cmdList[k].cmd[l]) {
                        eq = FALSE;
                    }
                }
                ketCube_terminal_andCmdFlags(&localContext, &activeFlags, &(cmdList[k].flags));
                if ((eq == TRUE) 
                     && (ketCube_terminal_checkCmdSubtreeContext(&localContext) == TRUE)) {
                    
                    KETCUBE_TERMINAL_PRINTF("\t%s",
                                            cmdList
                                            [k].cmd);
                    KETCUBE_TERMINAL_ENDL();
                }
                k++;
            }
            break;
        }
        cmdIndex++;
    }

    ketCube_terminal_UpdateCmdLine();
}

/**
  * @brief Print current unfinished command 
  *
  * Use when it is necessary to print something and not to disrupt current command
  *
  */
void ketCube_terminal_UpdateCmdLine(void)
{
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PROMPT();
    KETCUBE_TERMINAL_PRINTF("%s", commandBuffer);
}

/**
  * @brief Clear command line
  *
  * Use to clear user input and prompt
  *
  */
void ketCube_terminal_ClearCmdLine(void)
{
    uint8_t i;

    KETCUBE_TERMINAL_PRINTF("\r");
    // clear command + prompt
    for (i = 0; i < (*commandPtr + 3); i++) {
        KETCUBE_TERMINAL_PRINTF(" ");
    }
    KETCUBE_TERMINAL_PRINTF("\r");
}


/**
  * @brief Return next command parameter index
  *
  * @note This function is deprecated and will be removed in the next release(s)
  * @todo This is a deprecated function
  * 
  * @param ptr index where to start parameter search
  * 
  * @retval index next parameter index
  *
  */
uint8_t ketCube_terminal_getNextParam(uint8_t ptr)
{
    // find next param
    while (commandBuffer[ptr] != ' ') {
        if (ptr < KETCUBE_TERMINAL_CMD_MAX_LEN) {
            ptr++;
        } else {
            return 0;
        }
    }
    while (commandBuffer[ptr] == ' ') {
        if (ptr < KETCUBE_TERMINAL_CMD_MAX_LEN) {
            ptr++;
        } else {
            return 0;
        }
    }

    return ptr;
}

/**
  * @brief Process user input
  *
  */
void ketCube_terminal_ProcessCMD(void)
{
    char tmpchar;
    uint8_t i;

    /* Process all commands */
    while (IsNewCharReceived() == TRUE) {
        tmpchar = GetNewChar();

        if (((tmpchar >= 'a') && (tmpchar <= 'z'))
            || ((tmpchar >= 'A') && (tmpchar <= 'Z'))
            || ((tmpchar >= '0') && (tmpchar <= '9'))
            || (tmpchar == ' ')
            || (tmpchar == ',')
            || (tmpchar == '+')
            || (tmpchar == '-')) {
            if (*commandPtr > KETCUBE_TERMINAL_CMD_MAX_LEN) {
                KETCUBE_TERMINAL_PRINTF
                    ("Command too long, remove characters or press [ENTER] to exec command!");
                KETCUBE_TERMINAL_ENDL();
                KETCUBE_TERMINAL_PROMPT();
                KETCUBE_TERMINAL_PRINTF("%s", &(commandBuffer[0]));
            }

            commandBuffer[*commandPtr] = tmpchar;
            *commandPtr = (*commandPtr + 1);
            commandBuffer[*commandPtr] = 0x00;

#if KETCUBE_TERMINAL_ECHO == TRUE
            KETCUBE_TERMINAL_PRINTF("%c", tmpchar);
#endif
        } else if ((tmpchar == '\b') || (tmpchar == 127)) {     // delete char
            if (*commandPtr > 0) {
                *commandPtr = (*commandPtr - 1);
                commandBuffer[*commandPtr] = 0x00;
                KETCUBE_TERMINAL_PRINTF("\b \b");
            }
        } else if ((tmpchar == '\n') || (tmpchar == '\r')) {    // end of command
            ketCube_terminal_execCMD();
        } else if (tmpchar == '\t') {   // display help
            ketCube_terminal_printCMDHelp();
        } else if (tmpchar == '>') {    // up history
            KETCUBE_TERMINAL_PRINTF("\r");
            for (i = 0; i < *commandPtr; i++) {
                KETCUBE_TERMINAL_PRINTF(" ");
            }
            KETCUBE_TERMINAL_PRINTF("\r");

            commandHistoryPtr =
                (commandHistoryPtr + 1) % KETCUBE_TERMINAL_HISTORY_LEN;
            if (commandHistoryPtr == 0) {
                commandHistoryPtr = KETCUBE_TERMINAL_HISTORY_LEN - 1;
            }
            commandBuffer =
                (char *) &(commandHistory[commandHistoryPtr].buffer[0]);
            commandPtr =
                (uint8_t *) & (commandHistory[commandHistoryPtr].ptr);
            KETCUBE_TERMINAL_PROMPT();
            KETCUBE_TERMINAL_PRINTF("%s", commandBuffer);
        } else if (tmpchar == '<') {    // down history
            KETCUBE_TERMINAL_PRINTF("\r");
            for (i = 0; i < *commandPtr; i++) {
                KETCUBE_TERMINAL_PRINTF(" ");
            }
            KETCUBE_TERMINAL_PRINTF("\r");

            commandHistoryPtr = commandHistoryPtr - 1;
            if (commandHistoryPtr >= KETCUBE_TERMINAL_HISTORY_LEN) {
                commandHistoryPtr = 0;
            }
            commandBuffer =
                (char *) &(commandHistory[commandHistoryPtr].buffer[0]);
            commandPtr =
                (uint8_t *) & (commandHistory[commandHistoryPtr].ptr);
            KETCUBE_TERMINAL_PROMPT();
            KETCUBE_TERMINAL_PRINTF("%s", commandBuffer);
        } else {                // ignored characters: '\r' , ...
            //KETCUBE_TERMINAL_PRINTF("X >> %d ", (int) tmpchar);
            continue;
        }
    }
}

/**
  * @brief Print line to serial line + newline
  *
  */
void ketCube_terminal_Println(char *format, ...)
{
    va_list args;
    va_start(args, format);

    ketCube_terminal_UsartPrintVa(format, args);
    ketCube_terminal_UpdateCmdLine();

    va_end(args);
}


/**
  * @brief Print format to serial line
  *
  */
void ketCube_terminal_Print(char *format, ...)
{
    va_list args;
    va_start(args, format);

    ketCube_terminal_UsartPrintVa(format, args);

    va_end(args);
}

/**
  * @brief Print Debug info to serial line + newline
  *
  * @param msgSeverity mesage severity
  * @param format printf-style format string
  * @param args 
  * 
  * @note ketCube_terminal_CoreSeverityPrintln() does not introduce any formatting in contrast with ketCube_terminal_ModSeverityPrintln(), where the produced string is prefixed by originator module Name
  * 
  */
void ketCube_terminal_CoreSeverityPrintln(ketCube_severity_t msgSeverity,
                                          char *format, ...)
{
    if (ketCube_coreCfg.severity < msgSeverity) {
        return;
    }

    KETCUBE_TERMINAL_CLR_LINE();
    va_list args;
    va_start(args, format);
    ketCube_terminal_UsartPrintVa(format, args);
    va_end(args);
    ketCube_terminal_UpdateCmdLine();
}

/**
  * @brief Print Debug info to serial line + newline
  *
  * @param msgSeverity mesage severity
  * @param format printf-style format string
  * @param args 
  * 
  */
void ketCube_terminal_DriverSeverityPrintln(const char * drvName, ketCube_severity_t msgSeverity, char *format, ...)
{
    if (ketCube_coreCfg.driverSeverity < msgSeverity) {
        return;
    }

    KETCUBE_TERMINAL_CLR_LINE();
    KETCUBE_TERMINAL_PRINTF("Driver :: %s :: ", drvName);
    va_list args;
    va_start(args, format);
    ketCube_terminal_UsartPrintVa(format, args);
    va_end(args);
    ketCube_terminal_UpdateCmdLine();
}

/**
  * @brief Print Debug info to serial line + newline
  *
  * @param msgSeverity mesage severity
  * @param modID mesage origin moduleID
  * @param format printf-style format string
  * @param args va_list
  * 
  */
void ketCube_terminal_ModSeverityPrintln(ketCube_severity_t msgSeverity,
                                         ketCube_cfg_moduleIDs_t modId,
                                         char *format, va_list args)
{
    if (ketCube_modules_List[modId].cfgPtr->severity < msgSeverity) {
        return;
    }

    KETCUBE_TERMINAL_CLR_LINE();
    KETCUBE_TERMINAL_PRINTF("%s :: ",
                            &(ketCube_modules_List[modId].name[0]));
    ketCube_terminal_UsartPrintVa(format, args);
    ketCube_terminal_UpdateCmdLine();
}

/**
  * @brief Print Debug info to serial line + newline
  *
  * @note This function is deprecated and will be removed in the next release(s)
  * @todo This is a deprecated function
  * 
  */
void ketCube_terminal_DebugPrintln(char *format, ...)
{
#ifdef KETCUBE_CFG_INC_MOD_DEBUGDISPLAY
    if (ketCube_modules_List[KETCUBE_LISTS_MODULEID_DEBUGDISPLAY].
        cfgByte.enable != TRUE) {
        return;
    }

    KETCUBE_TERMINAL_CLR_LINE();
    va_list args;
    va_start(args, format);

    ketCube_terminal_UsartPrintVa(format, args);
    ketCube_terminal_UpdateCmdLine();

    va_end(args);
#endif // KETCUBE_CFG_INC_MOD_DEBUGDISPLAY
}

/**
  * @brief Print Debug info to serial line
  *
  * @note This function is deprecated and will be removed in the next release(s)
  * @todo This is a deprecated function
  * 
  */
void ketCube_terminal_DebugPrint(char *format, ...)
{
#ifdef KETCUBE_CFG_INC_MOD_DEBUGDISPLAY
    if (ketCube_modules_List[KETCUBE_LISTS_MODULEID_DEBUGDISPLAY].
        cfgByte.enable != TRUE) {
        return;
    }

    va_list args;
    va_start(args, format);

    ketCube_terminal_UsartPrintVa(format, args);

    va_end(args);
#endif // KETCUBE_CFG_INC_MOD_DEBUGDISPLAY
}
