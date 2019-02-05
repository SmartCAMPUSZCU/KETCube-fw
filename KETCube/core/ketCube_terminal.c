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

// END of USART configuration

static ketCube_terminal_buffer_t
    commandHistory[KETCUBE_TERMINAL_HISTORY_LEN];
static char *commandBuffer = (char *) &(commandHistory[0].buffer[0]);
static uint8_t *commandPtr = (uint8_t *) & (commandHistory[0].ptr);
static uint8_t commandHistoryPtr = 0;
static uint8_t commandParams;   //< Index of the first command parameter in buffer

/* Helper function prototypes */
static uint8_t ketCube_terminal_getNextParam(uint8_t ptr);
static void ketCube_terminal_printCmdList(uint16_t cmdIndex,
                                          uint8_t level);
static void ketCube_terminal_saveCfgHEXStr(char *data,
                                           ketCube_cfg_moduleIDs_t id,
                                           ketCube_cfg_AllocEEPROM_t addr,
                                           ketCube_cfg_LenEEPROM_t len);
static void ketCube_terminal_saveCfgDECStr(char *data,
                                           ketCube_cfg_moduleIDs_t id,
                                           ketCube_cfg_AllocEEPROM_t addr,
                                           ketCube_cfg_LenEEPROM_t len);

/* Core command callback prototypes */
static void ketCube_terminal_cmd_reload(void);
static void ketCube_terminal_cmd_list(void);
static void ketCube_terminal_cmd_enable(void);
static void ketCube_terminal_cmd_disable(void);

/* Core configuration */
static void ketCube_terminal_cmd_show_core_basePeriod(void);
static void ketCube_terminal_cmd_show_core_startDelay(void);
static void ketCube_terminal_cmd_show_core_severity(void);
static void ketCube_terminal_cmd_show_driver_severity(void);
static void ketCube_terminal_cmd_set_core_basePeriod(void);
static void ketCube_terminal_cmd_set_core_startDelay(void);
static void ketCube_terminal_cmd_set_core_severity(void);
static void ketCube_terminal_cmd_set_driver_severity(void);

// List of KETCube commands
#include "../../Projects/src/ketCube_cmdList.c" // include a project-specific file

/* --------------------------------- */
/* --- Terminal Helper functions --- */

/**
 * @brief A wrapper to ketCube_cfg_SaveStr function 
 *
 * Provides textual output to terminal is provided
 */
void ketCube_terminal_saveCfgHEXStr(char *data, ketCube_cfg_moduleIDs_t id,
                                    ketCube_cfg_AllocEEPROM_t addr,
                                    ketCube_cfg_LenEEPROM_t len)
{
    ketCube_cfg_Error_t ret;

    KETCUBE_TERMINAL_ENDL();

    ret = ketCube_cfg_SaveStr(data, id, addr, len);
    if (ret == KETCUBE_CFG_INV_HEX) {
        KETCUBE_TERMINAL_PRINTF("Invalid HEX number!");
        KETCUBE_TERMINAL_ENDL();
        return;
    } else if (ret == ketCube_cfg_Save_ERROR) {
        KETCUBE_TERMINAL_PRINTF("Write error!");
        KETCUBE_TERMINAL_ENDL();
        return;
    } else {
        KETCUBE_TERMINAL_PRINTF("Write success!");
    }

    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief A wrapper to ketCube_cfg_saveStr function 
 *
 * Provides textual output to terminal
 */
void ketCube_terminal_saveCfgDECStr(char *data, ketCube_cfg_moduleIDs_t id,
                                    ketCube_cfg_AllocEEPROM_t addr,
                                    ketCube_cfg_LenEEPROM_t len)
{
    ketCube_cfg_Error_t ret;

    KETCUBE_TERMINAL_ENDL();

    if (ketCube_common_IsDecString(data, strlen(data)) == FALSE) {
        KETCUBE_TERMINAL_PRINTF("Invalid DEC number!");
        KETCUBE_TERMINAL_ENDL();
        return;
    } else if (len > sizeof(int32_t)) {
        KETCUBE_TERMINAL_PRINTF("Invalid number size!");
        KETCUBE_TERMINAL_ENDL();
        return;
    }

    int32_t target;
    ketCube_common_Dec2int(&target, data, strlen(data));

    ret = ketCube_cfg_Save((uint8_t *) & target, id, addr, len);
    if (ret == ketCube_cfg_Save_ERROR) {
        KETCUBE_TERMINAL_PRINTF("Write error!");
        KETCUBE_TERMINAL_ENDL();
        return;
    } else {
        KETCUBE_TERMINAL_PRINTF("Write success!");
    }

    KETCUBE_TERMINAL_ENDL();
}

/**
  *
  * @brief Prints command list at index/level
  * @param cmdIndex index of the first command to print
  * @param level level of commands to print
  *
  */
static void ketCube_terminal_printCmdList(uint16_t cmdIndex, uint8_t level)
{

    if (level > 0) {
        KETCUBE_TERMINAL_PRINTF("List of sub-commands for command %s: ",
                                ketCube_terminal_commands[cmdIndex -
                                                          1].cmd);
        KETCUBE_TERMINAL_ENDL();
    } else {
        KETCUBE_TERMINAL_PRINTF("List of commands: ");
        KETCUBE_TERMINAL_ENDL();
    }

    while (ketCube_terminal_commands[cmdIndex].cmd != NULL) {
        if (ketCube_terminal_commands[cmdIndex].cmdLevel == level) {
            KETCUBE_TERMINAL_PRINTF("\t%s\t%s",
                                    ketCube_terminal_commands
                                    [cmdIndex].cmd,
                                    ketCube_terminal_commands
                                    [cmdIndex].descr);
            KETCUBE_TERMINAL_ENDL();
        }

        if (ketCube_terminal_commands[cmdIndex].cmdLevel < level) {
            return;
        }
        cmdIndex++;
    }
}

/* ------------------------------ */
/* --- Core command callbacks --- */


void ketCube_terminal_cmd_help(void)
{
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("%s Command-line Interface HELP",
                            KETCUBE_CFG_DEV_NAME);
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("-----------------------------------");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();

    ketCube_terminal_printCmdList(0, 0);

    KETCUBE_TERMINAL_ENDL();
}

void ketCube_terminal_cmd_about(void)
{
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("About %s", KETCUBE_CFG_DEV_NAME);
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("-------------");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();

    KETCUBE_TERMINAL_PRINTF
        ("KETCube was created on University of West Bohemia in Pilsen.");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("KETCube home: https://github.com/SmartCAMPUSZCU/KETCube-docs");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("KETCube is provided under NCSA Open Source License - see LICENSE.txt.");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("Copyright (c) 2018 University of West Bohemia in Pilsen");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("All rights reserved.");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("Developed by:");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("The SmartCampus Team");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("Department of Technologies and Measurement");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("www.smartcampus.cz | www.zcu.cz");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();

    KETCUBE_TERMINAL_PRINTF
        ("KETCube includes code provided by SEMTECH S.A.");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("under Revised BSD License - see LICENSE_SEMTECH.txt");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("Copyright (c) 2013, SEMTECH S.A.");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("All rights reserved.");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();

    KETCUBE_TERMINAL_PRINTF("KETCube includes code provided by");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("STMicroelectronics International N.V. - see LICENSE_STM.txt");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("Copyright (c) 2017 STMicroelectronics International N.V.");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("All rights reserved.");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Perform KETCube reset
 * 
 */
void ketCube_terminal_cmd_reload(void)
{
    KETCUBE_TERMINAL_ENDL();

    KETCUBE_TERMINAL_PRINTF
        ("Performing system reset and reloading %s configuration ...",
         KETCUBE_CFG_DEV_NAME);
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();

    NVIC_SystemReset();
}

/**
 * @brief Show available KETCube modules
 * 
 */
void ketCube_terminal_cmd_list(void)
{
    uint16_t i;
    ketCube_cfg_ModuleCfgByte_t data;
    char severityEEPROM = 'N';
    char severity = 'N';

    KETCUBE_TERMINAL_ENDL();

    KETCUBE_TERMINAL_PRINTF("Available modules:");
    KETCUBE_TERMINAL_ENDL();

    for (i = 0; i < ketCube_modules_CNT; i++) {
        if (ketCube_cfg_Load
            (((uint8_t *) & data), (ketCube_cfg_moduleIDs_t) i,
             (ketCube_cfg_AllocEEPROM_t) 0,
             (ketCube_cfg_LenEEPROM_t) 1) == KETCUBE_CFG_OK) {

            switch (data.severity) {
            case KETCUBE_CFG_SEVERITY_NONE:
                severity = 'N';
                break;
            case KETCUBE_CFG_SEVERITY_ERROR:
                severity = 'R';
                break;
            case KETCUBE_CFG_SEVERITY_INFO:
                severity = 'I';
                break;
            case KETCUBE_CFG_SEVERITY_DEBUG:
                severity = 'D';
                break;
            }

            switch (ketCube_modules_List[i].cfgByte.severity) {
            case KETCUBE_CFG_SEVERITY_NONE:
                severityEEPROM = 'N';
                break;
            case KETCUBE_CFG_SEVERITY_ERROR:
                severityEEPROM = 'R';
                break;
            case KETCUBE_CFG_SEVERITY_INFO:
                severityEEPROM = 'I';
                break;
            case KETCUBE_CFG_SEVERITY_DEBUG:
                severityEEPROM = 'D';
                break;
            }

            if (severityEEPROM != severity) {
                KETCUBE_TERMINAL_PRINTF("%c -> %c\t", severityEEPROM,
                                        severity);
            } else {
                KETCUBE_TERMINAL_PRINTF("  %c\t", severity);
            }

            if (data.enable != ketCube_modules_List[i].cfgByte.enable) {
                if (ketCube_modules_List[i].cfgByte.enable == TRUE) {
                    KETCUBE_TERMINAL_PRINTF("E -> ");
                } else {
                    KETCUBE_TERMINAL_PRINTF("D -> ");
                }
            }

            if (data.enable == TRUE) {
                KETCUBE_TERMINAL_PRINTF("E");
            } else {
                KETCUBE_TERMINAL_PRINTF("D");
            }

            KETCUBE_TERMINAL_PRINTF("\t%s\t%s",
                                    ketCube_modules_List[i].name,
                                    ketCube_modules_List[i].descr);
            KETCUBE_TERMINAL_ENDL();
        } else {
            KETCUBE_TERMINAL_PRINTF
                ("Error while reading module configuration!");
        }

    }


    KETCUBE_TERMINAL_ENDL();

    KETCUBE_TERMINAL_PRINTF
        ("Module State: E == Module Enabled; D == Module Disabled");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("Module severity: N = NONE, R = ERROR; I = INFO; D = DEBUG");


    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Enable/disable given KETCube module
 *
 * @param enable enable given module
 * 
 */
void ketCube_terminal_cmd_enableDisable(bool enable)
{
    uint16_t i;
    ketCube_cfg_ModuleCfgByte_t tmpCfgByte;
    uint8_t tmpCmdLen;
    ketCube_severity_t severity = KETCUBE_CORECFG_DEFAULT_SEVERITY;
    int32_t tmpSeverity;


    for (i = 0; i < ketCube_modules_CNT; i++) {
        tmpCmdLen = strlen(&(ketCube_modules_List[i].name[0]));

        if (strncmp
            (&(ketCube_modules_List[i].name[0]),
             &(commandBuffer[commandParams]), tmpCmdLen) == 0) {

            if (commandBuffer[commandParams + tmpCmdLen] == 0x00) {
                break;
            }

            if (commandBuffer[commandParams + tmpCmdLen] == ' ') {
                // get next param - severity
                sscanf(&(commandBuffer[commandParams + tmpCmdLen + 1]),
                       "%d", (int *) &tmpSeverity);
                severity = (ketCube_severity_t) tmpSeverity;
                if (severity > KETCUBE_CFG_SEVERITY_DEBUG) {
                    severity = KETCUBE_CORECFG_DEFAULT_SEVERITY;
                }
                break;
            }
        }
    }

    if (i == ketCube_modules_CNT) {
        KETCUBE_TERMINAL_PRINTF("Invalid module name!");
        return;
    }

    KETCUBE_TERMINAL_PRINTF("Setting module %s:",
                            ketCube_modules_List[i].name);

    //do not enable/disable now but when reload ...
    tmpCfgByte = ketCube_modules_List[i].cfgByte;
    tmpCfgByte.enable = enable; // enable/disable
    tmpCfgByte.severity = severity;     // set severity

    if (ketCube_cfg_Save
        (((uint8_t *) & (tmpCfgByte)), (ketCube_cfg_moduleIDs_t) i,
         (ketCube_cfg_AllocEEPROM_t) 0,
         (ketCube_cfg_LenEEPROM_t) 1) == KETCUBE_CFG_OK) {
        KETCUBE_TERMINAL_PRINTF("\t success!");
    } else {
        KETCUBE_TERMINAL_PRINTF("\t error!");
    }
}


/**
 * @brief Enable given KETCube module
 * 
 */
void ketCube_terminal_cmd_enable(void)
{
    KETCUBE_TERMINAL_ENDL();
    ketCube_terminal_cmd_enableDisable(TRUE);
    KETCUBE_TERMINAL_ENDL();
}


/**
 * @brief Enable given KETCube module
 * 
 */
void ketCube_terminal_cmd_disable(void)
{
    KETCUBE_TERMINAL_ENDL();
    ketCube_terminal_cmd_enableDisable(FALSE);
    KETCUBE_TERMINAL_ENDL();
}

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
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF
        ("Use [TAB] key to show build-in help for current command");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("Use [ENTER] key to execute current command");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("Use [+]/[-] keys to browse command hostory");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_ENDL();

    ketCube_terminal_printCmdList(0, 0);

    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PROMPT();
}

/**
  * @brief Execute command-in-buffer
  *
  */
void ketCube_terminal_execCMD(void)
{
    uint16_t cmdIndex, j, cmdBuffIndex;
    bool eq;
    uint8_t cmdLevel = 0;

    KETCUBE_TERMINAL_ENDL();

    if (*commandPtr == 0) {
        KETCUBE_TERMINAL_PROMPT();
        return;
    }

    cmdIndex = 0;
    cmdBuffIndex = 0;

    while (ketCube_terminal_commands[cmdIndex].cmd != NULL) {

        // skip non actual levels
        if (ketCube_terminal_commands[cmdIndex].cmdLevel > cmdLevel) {
            cmdIndex++;
            continue;
        } else if (ketCube_terminal_commands[cmdIndex].cmdLevel < cmdLevel) {
            break;
        }

        eq = TRUE;
        j = 0;

        while ((ketCube_terminal_commands[cmdIndex].cmd[j] != 0x00)
               && (*commandPtr > cmdBuffIndex)) {
            /* compare command characters */
            if (ketCube_terminal_commands[cmdIndex].cmd[j] !=
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

        /* move to next command if mismatch */
        if (eq == FALSE) {
            cmdBuffIndex -= j;
            cmdIndex++;
            continue;
        }

        /* move to subcommand if exist */
        if ((ketCube_terminal_commands[cmdIndex].cmd[j] == 0x00) &&
            (commandBuffer[cmdBuffIndex] != 0x00) &&
            (ketCube_terminal_commands[cmdIndex].cmdLevel <
             ketCube_terminal_commands[cmdIndex + 1].cmdLevel)) {
            cmdBuffIndex++;     // remove space character: ' '
            cmdIndex++;
            cmdLevel++;
            continue;
        }

        /* execute command */
        if ((eq == TRUE)
            && (ketCube_terminal_commands[cmdIndex].cmd[j] == 0x00)
            && ((commandBuffer[cmdBuffIndex] == 0x00)
                || ((commandBuffer[cmdBuffIndex] == ' ')
                    && (ketCube_terminal_commands[cmdIndex].paramCnt !=
                        0)))) {
            commandParams = cmdBuffIndex + 1;
            if (ketCube_terminal_commands[cmdIndex].callback != NULL) {
                KETCUBE_TERMINAL_PRINTF("Executing command: %s",
                                        ketCube_terminal_commands
                                        [cmdIndex].cmd);
                KETCUBE_TERMINAL_ENDL();
                ketCube_terminal_commands[cmdIndex].callback();
            } else {
                KETCUBE_TERMINAL_PRINTF("Help for command: %s",
                                        ketCube_terminal_commands
                                        [cmdIndex].cmd);
                KETCUBE_TERMINAL_ENDL();
                ketCube_terminal_printCmdList(cmdIndex + 1,
                                              ketCube_terminal_commands
                                              [cmdIndex].cmdLevel + 1);
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
    }
    KETCUBE_TERMINAL_PRINTF("Command not found!");
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PROMPT();

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
    uint8_t cmdLevel = 0;

    KETCUBE_TERMINAL_ENDL();

    if (*commandPtr == 0) {
        KETCUBE_TERMINAL_PROMPT();
        return;
    }

    cmdIndex = 0;
    cmdBuffIndex = 0;

    while (ketCube_terminal_commands[cmdIndex].cmd != NULL) {

        // skip non actual levels
        if (ketCube_terminal_commands[cmdIndex].cmdLevel > cmdLevel) {
            cmdIndex++;
            continue;
        } else if (ketCube_terminal_commands[cmdIndex].cmdLevel < cmdLevel) {
            break;
        }

        eq = TRUE;
        j = 0;

        while ((ketCube_terminal_commands[cmdIndex].cmd[j] != 0x00)
               && (*commandPtr > cmdBuffIndex)) {
            /* compare command characters */
            if (ketCube_terminal_commands[cmdIndex].cmd[j] !=
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

        /* move to next command if mismatch */
        if (eq == FALSE) {
            cmdBuffIndex -= j;
            cmdIndex++;
            continue;
        }

        /* move to subcommand if exist */
        if ((ketCube_terminal_commands[cmdIndex].cmd[j] == 0x00) &&
            (commandBuffer[cmdBuffIndex] != 0x00) &&
            (ketCube_terminal_commands[cmdIndex].cmdLevel <
             ketCube_terminal_commands[cmdIndex + 1].cmdLevel)) {
            cmdBuffIndex++;     // remove space character: ' '
            cmdIndex++;
            cmdLevel++;
            continue;
        }

        if ((eq == TRUE)
            && (ketCube_terminal_commands[cmdIndex].cmd[j] == 0x00)
            && ((commandBuffer[cmdBuffIndex] == 0x00))) {
            KETCUBE_TERMINAL_PRINTF("Help for command %s: \t%s",
                                    ketCube_terminal_commands
                                    [cmdIndex].cmd,
                                    ketCube_terminal_commands
                                    [cmdIndex].descr);
            KETCUBE_TERMINAL_ENDL();
            ketCube_terminal_printCmdList(cmdIndex + 1,
                                          ketCube_terminal_commands
                                          [cmdIndex].cmdLevel + 1);
            break;
        } else if (eq == TRUE) {
            KETCUBE_TERMINAL_PRINTF("Available commands: ");
            KETCUBE_TERMINAL_ENDL();

            k = cmdIndex;
            while ((ketCube_terminal_commands[k].cmd != NULL) &&
                   (ketCube_terminal_commands[k].cmdLevel >=
                    ketCube_terminal_commands[cmdIndex].cmdLevel)) {
                if (ketCube_terminal_commands[k].cmdLevel ==
                    ketCube_terminal_commands[cmdIndex].cmdLevel) {
                    eq = TRUE;
                    for (l = 0; l < j; l++) {
                        if (ketCube_terminal_commands[cmdIndex].cmd[l] !=
                            ketCube_terminal_commands[k].cmd[l]) {
                            eq = FALSE;
                        }
                    }
                    if (eq == TRUE) {
                        KETCUBE_TERMINAL_PRINTF("\t%s",
                                                ketCube_terminal_commands
                                                [k].cmd);
                        KETCUBE_TERMINAL_ENDL();
                    }
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
            || (tmpchar == ',')) {
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
        } else if (tmpchar == '+') {    // up history
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
        } else if (tmpchar == '-') {    // down history
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
    if (ketCube_coreCfg_severity < msgSeverity) {
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
    if (ketCube_coreCfg_driverSeverity < msgSeverity) {
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
    if (ketCube_modules_List[modId].cfgByte.severity < msgSeverity) {
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
    if (ketCube_modules_List[KETCUBE_LISTS_MODULEID_DEBUGDISPLAY].
        cfgByte.enable != TRUE) {
        return;
    }

    va_list args;
    va_start(args, format);

    ketCube_terminal_UsartPrintVa(format, args);

    va_end(args);
}


/* -------------------------- */
/* --- Core configuration --- */

/**
 * @brief Show KETCube base period
 * 
 */
void ketCube_terminal_cmd_show_core_basePeriod(void)
{
    KETCUBE_TERMINAL_PRINTF("KETCube Core base period is: %d ms",
                            ketCube_coreCfg_BasePeriod);
    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Show KETCube start delay
 * 
 */
void ketCube_terminal_cmd_show_core_startDelay(void)
{
    KETCUBE_TERMINAL_PRINTF("KETCube Start delay is: %d ms",
                            ketCube_coreCfg_StartDelay);
    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Show KETCube severity
 * 
 */
void ketCube_terminal_cmd_show_core_severity(void)
{
    KETCUBE_TERMINAL_PRINTF("KETCube core severity: ");

    switch (ketCube_coreCfg_severity) {
    case KETCUBE_CFG_SEVERITY_NONE:
        KETCUBE_TERMINAL_PRINTF("NONE");
        break;
    case KETCUBE_CFG_SEVERITY_ERROR:
        KETCUBE_TERMINAL_PRINTF("ERROR");
        break;
    case KETCUBE_CFG_SEVERITY_INFO:
        KETCUBE_TERMINAL_PRINTF("INFO");
        break;
    case KETCUBE_CFG_SEVERITY_DEBUG:
        KETCUBE_TERMINAL_PRINTF("DEBUG");
        break;
    }

    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Show KETCube driver(s) severity
 * 
 */
void ketCube_terminal_cmd_show_driver_severity(void)
{
    KETCUBE_TERMINAL_PRINTF("KETCube driver(s) severity: ");

    switch (ketCube_coreCfg_driverSeverity) {
    case KETCUBE_CFG_SEVERITY_NONE:
        KETCUBE_TERMINAL_PRINTF("NONE");
        break;
    case KETCUBE_CFG_SEVERITY_ERROR:
        KETCUBE_TERMINAL_PRINTF("ERROR");
        break;
    case KETCUBE_CFG_SEVERITY_INFO:
        KETCUBE_TERMINAL_PRINTF("INFO");
        break;
    case KETCUBE_CFG_SEVERITY_DEBUG:
        KETCUBE_TERMINAL_PRINTF("DEBUG");
        break;
    }

    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Set KETCube driver(s) severity
 * 
 */
void ketCube_terminal_cmd_set_driver_severity(void)
{
    uint32_t value;
    char *endptr;

    value = strtol(&(commandBuffer[commandParams]), &endptr, 10);

    KETCUBE_TERMINAL_PRINTF("Setting KETCube driver(s) severity: ");
    switch ((ketCube_severity_t) value) {
    case KETCUBE_CFG_SEVERITY_NONE:
        KETCUBE_TERMINAL_PRINTF("NONE");
        break;
    case KETCUBE_CFG_SEVERITY_ERROR:
        KETCUBE_TERMINAL_PRINTF("ERROR");
        break;
    case KETCUBE_CFG_SEVERITY_INFO:
        KETCUBE_TERMINAL_PRINTF("INFO");
        break;
    case KETCUBE_CFG_SEVERITY_DEBUG:
        KETCUBE_TERMINAL_PRINTF("DEBUG");
        break;
    default:
        KETCUBE_TERMINAL_PRINTF("IVALID SEVERITY VALUE");
        KETCUBE_TERMINAL_ENDL();
        KETCUBE_TERMINAL_PRINTF("KETCube core severity set error!");
        KETCUBE_TERMINAL_ENDL();
        return;
    }

    KETCUBE_TERMINAL_ENDL();

    if (ketCube_EEPROM_WriteBuffer
        (KETCUBE_EEPROM_ALLOC_CORE + KETCUBE_CORECFG_ADR_DRIVER_SEVERITY,
         (uint8_t *) & (value), 1) == KETCUBE_EEPROM_OK) {
    } else {
        KETCUBE_TERMINAL_PRINTF("error!");
    }

    KETCUBE_TERMINAL_PRINTF("sucess!");

    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Set KETCube base period
 * 
 */
void ketCube_terminal_cmd_set_core_basePeriod(void)
{
    uint32_t value;
    char *endptr;

    value = strtol(&(commandBuffer[commandParams]), &endptr, 10);

    if (ketCube_EEPROM_WriteBuffer
        (KETCUBE_EEPROM_ALLOC_CORE + KETCUBE_CORECFG_ADR_BASEPERIOD,
         (uint8_t *) & (value), 4) == KETCUBE_EEPROM_OK) {
        KETCUBE_TERMINAL_PRINTF
            ("KETCube Core base period is set to: %d ms", value);
    } else {
        KETCUBE_TERMINAL_PRINTF("KETCube Core base period write error!");
    }
    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Set KETCube start delay
 * 
 */
void ketCube_terminal_cmd_set_core_startDelay(void)
{
    uint32_t value;
    char *endptr;

    value = strtol(&(commandBuffer[commandParams]), &endptr, 10);

    if (ketCube_EEPROM_WriteBuffer
        (KETCUBE_EEPROM_ALLOC_CORE + KETCUBE_CORECFG_ADR_STARTDELAY,
         (uint8_t *) & (value), 4) == KETCUBE_EEPROM_OK) {
        KETCUBE_TERMINAL_PRINTF("KETCube Start delay is set to: %d ms",
                                value);
    } else {
        KETCUBE_TERMINAL_PRINTF("KETCube Start delay write error!");
    }
    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Set KETCube severity
 * 
 */
void ketCube_terminal_cmd_set_core_severity(void)
{
    uint32_t value;
    char *endptr;

    value = strtol(&(commandBuffer[commandParams]), &endptr, 10);

    KETCUBE_TERMINAL_PRINTF("Setting KETCube core severity: ");
    switch ((ketCube_severity_t) value) {
    case KETCUBE_CFG_SEVERITY_NONE:
        KETCUBE_TERMINAL_PRINTF("NONE");
        break;
    case KETCUBE_CFG_SEVERITY_ERROR:
        KETCUBE_TERMINAL_PRINTF("ERROR");
        break;
    case KETCUBE_CFG_SEVERITY_INFO:
        KETCUBE_TERMINAL_PRINTF("INFO");
        break;
    case KETCUBE_CFG_SEVERITY_DEBUG:
        KETCUBE_TERMINAL_PRINTF("DEBUG");
        break;
    default:
        KETCUBE_TERMINAL_PRINTF("IVALID SEVERITY VALUE");
        KETCUBE_TERMINAL_ENDL();
        KETCUBE_TERMINAL_PRINTF("KETCube core severity set error!");
        KETCUBE_TERMINAL_ENDL();
        return;
    }

    KETCUBE_TERMINAL_ENDL();

    if (ketCube_EEPROM_WriteBuffer
        (KETCUBE_EEPROM_ALLOC_CORE + KETCUBE_CORECFG_ADR_SEVERITY,
         (uint8_t *) & (value), 1) == KETCUBE_EEPROM_OK) {
    } else {
        KETCUBE_TERMINAL_PRINTF("error!");
    }

    KETCUBE_TERMINAL_PRINTF("sucess!");

    KETCUBE_TERMINAL_ENDL();
}
