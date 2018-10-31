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

#define KETCUBE_TERMINAL_USART_CLK_ENABLE()              __USART1_CLK_ENABLE();
#define KETCUBE_TERMINAL_USART_RX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()
#define KETCUBE_TERMINAL_USART_TX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE() 
#define KETCUBE_TERMINAL_USART_FORCE_RESET()             __USART1_FORCE_RESET()
#define KETCUBE_TERMINAL_USART_RELEASE_RESET()           __USART1_RELEASE_RESET()

#define KETCUBE_TERMINAL_USART_TX_PIN                    GPIO_PIN_9
#define KETCUBE_TERMINAL_USART_TX_GPIO_PORT              GPIOA  
#define KETCUBE_TERMINAL_USART_TX_AF                     GPIO_AF4_USART1
#define KETCUBE_TERMINAL_USART_RX_PIN                    GPIO_PIN_10
#define KETCUBE_TERMINAL_USART_RX_GPIO_PORT              GPIOA 
#define KETCUBE_TERMINAL_USART_RX_AF                     GPIO_AF4_USART1

static UART_HandleTypeDef ketCube_terminal_UsartHandle;
static ketCube_UART_descriptor_t ketCube_terminal_UsartDescriptor;

#define USART_BUFFER_SIZE                                256 // It must be 256, because overflows control circular buffer properties

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
static void ketCube_terminal_cmd_set_core_basePeriod(void);
static void ketCube_terminal_cmd_set_core_startDelay(void);

// Place module command sets here
#ifdef KETCUBE_CFG_INC_MOD_BATMEAS
#include "ketCube_batMeas_cmd.c"
#endif

#ifdef KETCUBE_CFG_INC_MOD_LORA
#include "ketCube_lora_cmd.c"
#endif

#ifdef KETCUBE_CFG_INC_MOD_MBUS
#include "ketCube_mbus_cmd.c"
#endif

#ifdef KETCUBE_CFG_INC_MOD_S0
#include "ketCube_s0_cmd.c"
#endif

#ifdef KETCUBE_CFG_INC_MOD_MODBUS_AL
#include "ketCube_modBUSAL_cmd.c"
#endif

/**
  *
	*  @brief List of KETCube terminal commands
	*
  */
ketCube_terminal_cmd_t ketCube_terminal_commands[] = {
    {((char *) &("about")),
     ((char *)
      &("Print ABOUT information: Copyright, License, ...")),
     0,
     0,
     &ketCube_terminal_cmd_about},

    {((char *) &("help")),
     ((char *) &("Print HELP")),
     0,
     0,
     &ketCube_terminal_cmd_help},

    {((char *) &("disable")),
     ((char *) &("Disable KETCube module")),
     0,
     1,
     &ketCube_terminal_cmd_disable},

    {((char *) &("enable")),
     ((char *) &("Enable KETCube module")),
     0,
     1,
     &ketCube_terminal_cmd_enable},

    {((char *) &("list")),
     ((char *) &("List available KETCube modules")),
     0,
     0,
     &ketCube_terminal_cmd_list},

    {((char *) &("reload")),
     ((char *) &("Reload KETCube")),
     0,
     0,
     &ketCube_terminal_cmd_reload},

    {((char *) &("show")),
     ((char *) &("Show LoRa, SigFox ... parameters")),
     0,
     0,
     (void *) NULL},

    {((char *) &("core")),
     ((char *) &("Show KETCube Core parameters")),
     1,
     0,
     (void *) NULL},

    {((char *) &("basePeriod")),
     ((char *) &("KETCube base period")),
     2,
     0,
     &ketCube_terminal_cmd_show_core_basePeriod},

    {((char *) &("startDelay")),
     ((char *) &("First periodic action is delayed after power-up")),
     2,
     0,
     &ketCube_terminal_cmd_show_core_startDelay},

#ifdef KETCUBE_CFG_INC_MOD_BATMEAS
    {((char *) &("batMeas")),
     ((char *) &("Show batMeas parameters")),
     1,
     0,
     (void *) NULL},

    {((char *) &("bat")),
     ((char *) &("Show selected battery.")),
     2,
     0,
     &ketCube_terminal_cmd_show_batMeas_bat},
     
     {((char *) &("list")),
     ((char *) &("Show supported batteries.")),
     2,
     0,
     &ketCube_terminal_cmd_show_batMeas_list},
     
#endif        /* KETCUBE_CFG_INC_MOD_BATMEAS */
     
#ifdef KETCUBE_CFG_INC_MOD_LORA

    {((char *) &("LoRa")),
     ((char *) &("Show LoRa parameters")),
     1,
     0,
     (void *) NULL},

    {((char *) &("ABP")),
     ((char *) &("Is ABP enabled?")),
     2,
     0,
     &ketCube_LoRa_cmd_show_ABP},

    {((char *) &("appEUI")),
     ((char *) &("Show LoRa application EUI.")),
     2,
     0,
     &ketCube_LoRa_cmd_show_appEUI},

    {((char *) &("appKey")),
     ((char *) &("Show LoRa application key.")),
     2,
     0,
     &ketCube_LoRa_cmd_show_appKey},


    {((char *) &("appSKey")),
     ((char *) &("Show LoRa app session Key.")),
     2,
     0,
     &ketCube_LoRa_cmd_show_appSKey},

    {((char *) &("devAddr")),
     ((char *) &("Show LoRa device address.")),
     2,
     0,
     &ketCube_LoRa_cmd_show_devAddr},

    {((char *) &("devEUI")),
     ((char *) &("Show LoRa device EUI.")),
     2,
     0,
     &ketCube_LoRa_cmd_show_devEUI},

    {((char *) &("devEUIType")),
     ((char *)
      &
      ("Show LoRa device EUI type: custom (user-defined) or deviceID-based.")),
     2,
     0,
     &ketCube_LoRa_cmd_show_devEUIType},

    {((char *) &("OTAA")),
     ((char *) &("Is OTAA enabled?")),
     2,
     0,
     &ketCube_LoRa_cmd_show_OTAA},

    {((char *) &("nwkSKey")),
     ((char *) &("Show LoRa network session Key.")),
     2,
     0,
     &ketCube_LoRa_cmd_show_nwkSKey},

#endif                          /* KETCUBE_CFG_INC_MOD_LORA */

#ifdef KETCUBE_CFG_INC_MOD_MBUS
    {((char *) &("MBUS")),
     ((char *) &("Show M-BUS parameters")),
     1,
     0,
     (void *) NULL},
     
     {((char *) &("attempts")),
     ((char *) &("Show M-BUS # attempts.")),
     2,
     0,
     &ketCube_terminal_cmd_show_MBUS_RetryCount},

    {((char *) &("baudrate")),
     ((char *) &("Show M-BUS configured baudrate.")),
     2,
     0,
     &ketCube_terminal_cmd_show_MBUS_baudRate},

    {((char *) &("challenge")),
     ((char *) &("Show M-BUS challenge parameters")),
     2,
     0,
     (void *) NULL},

    {((char *) &("address")),
     ((char *) &("Show address of slave to be challenged")),
     3,
     0,
     &ketCube_terminal_cmd_show_MBUS_ChallengeAddress},

    {((char *) &("class")),
     ((char *) &("Show class of request")),
     3,
     0,
     &ketCube_terminal_cmd_show_MBUS_ChallengeClass},

    {((char *) &("response")),
     ((char *) &("Show M-BUS response cutting parameters")),
     2,
     0,
     (void *) NULL},

    {((char *) &("offset")),
     ((char *) &("Show offset of data to be extracted")),
     3,
     0,
     &ketCube_terminal_cmd_show_MBUS_ResponseOffset},

    {((char *) &("length")),
     ((char *) &("Show length of data to be extracted, beginning from <offset>")),
     3,
     0,
     &ketCube_terminal_cmd_show_MBUS_ResponseLength},
#endif        /* KETCUBE_CFG_INC_MOD_MBUS */

#ifdef KETCUBE_CFG_INC_MOD_S0
    {((char *) &("S0")),
     ((char *) &("Show S0 parameters")),
     1,
     0,
     (void *) NULL},

    {((char *) &("OnOff")),
     ((char *) &("Show S0 timers ON/OFF state.")),
     2,
     1,
     &ketCube_terminal_cmd_show_S0_OnOff},

     {((char *) &("Timeout")),
     ((char *) &("Show S0 timer's Timeout.")),
     2,
     1,
     &ketCube_terminal_cmd_show_S0_timeout},
     
     {((char *) &("Value")),
     ((char *) &("Show S0 timer's current value.")),
     2,
     1,
     &ketCube_terminal_cmd_show_S0_value},
     
#endif        /* KETCUBE_CFG_INC_MOD_S0 */

#ifdef KETCUBE_CFG_INC_MOD_MODBUS_AL
    {((char *) &("MODBUS")),
     ((char *) &("Show Modbus parameters")),
     1,
     0,
     (void *) NULL},
    
    {((char *) &("baudrate")),
     ((char *) &("Show ModBUS baudrate")),
     2,
     0,
     &ketCube_terminal_cmd_show_ModBUS_AL_BaudRate},

    {((char *) &("challenge")),
     ((char *) &("Show Modbus challenge parameters")),
     2,
     0,
     (void *) NULL},

    {((char *) &("address")),
     ((char *) &("Show address of slave to be challenged")),
     3,
     0,
     &ketCube_terminal_cmd_show_ModBUS_AL_ChallengeAddress},

    {((char *) &("coil")),
     ((char *) &("Show coil challenge parameters")),
     3,
     0,
     (void *) NULL},

    {((char *) &("start")),
     ((char *) &("Show coil starting address")),
     4,
     0,
     &ketCube_terminal_cmd_show_ModBUS_AL_CoilStart},
		
    {((char *) &("count")),
     ((char *) &("Show coil count")),
     4,
     0,
     &ketCube_terminal_cmd_show_ModBUS_AL_CoilCount},
		
    {((char *) &("discreteinput")),
     ((char *) &("Show discrete input challenge parameters")),
     3,
     0,
     (void *) NULL},

    {((char *) &("start")),
     ((char *) &("Show discrete input starting address")),
     4,
     0,
     &ketCube_terminal_cmd_show_ModBUS_AL_DiscreteInputStart},
		
    {((char *) &("count")),
     ((char *) &("Show discrete input count")),
     4,
     0,
     &ketCube_terminal_cmd_show_ModBUS_AL_DiscreteInputCount},
		
		{((char *) &("inputregister")),
     ((char *) &("Show input register challenge parameters")),
     3,
     0,
     (void *) NULL},

    {((char *) &("start")),
     ((char *) &("Show input register starting address")),
     4,
     0,
     &ketCube_terminal_cmd_show_ModBUS_AL_InputRegisterStart},
		
    {((char *) &("count")),
     ((char *) &("Show input register count")),
     4,
     0,
     &ketCube_terminal_cmd_show_ModBUS_AL_InputRegisterCount},

		{((char *) &("holdingregister")),
     ((char *) &("Show holding register challenge parameters")),
     3,
     0,
     (void *) NULL},

    {((char *) &("start")),
     ((char *) &("Show holding register starting address")),
     4,
     0,
     &ketCube_terminal_cmd_show_ModBUS_AL_HoldingRegisterStart},
		
    {((char *) &("count")),
     ((char *) &("Show input register count")),
     4,
     0,
     &ketCube_terminal_cmd_show_ModBUS_AL_HoldingRegisterCount},

#endif        /* KETCUBE_CFG_INC_MOD_MODBUS_AL */
     
    {((char *) &("set")),
     ((char *) &("Set LoRa, Sigfox ... parameters")),
     0,
     0,
     (void *) NULL},


    {((char *) &("core")),
     ((char *) &("Set KETCube Core parameters")),
     1,
     0,
     (void *) NULL},

    {((char *) &("basePeriod")),
     ((char *) &("KETCube base period")),
     2,
     1,
     &ketCube_terminal_cmd_set_core_basePeriod},

    {((char *) &("startDelay")),
     ((char *) &("First periodic action is delayed after power-up")),
     2,
     1,
     &ketCube_terminal_cmd_set_core_startDelay},

#ifdef KETCUBE_CFG_INC_MOD_BATMEAS
    {((char *) &("batMeas")),
     ((char *) &("Set batMeas parameters")),
     1,
     0,
     (void *) NULL},

    {((char *) &("bat")),
     ((char *) &("Select battery.")),
     2,
     1,
     &ketCube_terminal_cmd_set_batMeas_bat},
     
#endif        /* KETCUBE_CFG_INC_MOD_BATMEAS */
     
#ifdef KETCUBE_CFG_INC_MOD_LORA

    {((char *) &("LoRa")),
     ((char *) &("Set LoRa parameters")),
     1,
     0,
     (void *) NULL},

    {((char *) &("ABP")),
     ((char *) &("Enable ABP")),
     2,
     0,
     &ketCube_LoRa_cmd_set_ABP},

    {((char *) &("appEUI")),
     ((char *) &("Set LoRa application EUI")),
     2,
     1,
     &ketCube_LoRa_cmd_set_appEUI},

    {((char *) &("appKey")),
     ((char *) &("Set LoRa application key")),
     2,
     1,
     &ketCube_LoRa_cmd_set_appKey},

    {((char *) &("appSKey")),
     ((char *) &("Set LoRa application session key")),
     2,
     1,
     &ketCube_LoRa_cmd_set_appSKey},

    {((char *) &("devAddr")),
     ((char *) &("Set LoRa device address")),
     2,
     1,
     &ketCube_LoRa_cmd_set_devAddr},

    {((char *) &("devEUI")),
     ((char *) &("Set LoRa device EUI")),
     2,
     1,
     &ketCube_LoRa_cmd_set_devEUI},

    {((char *) &("devEUICustom")),
     ((char *) &("Use custom (user-defined) LoRa device EUI")),
     2,
     0,
     &ketCube_LoRa_cmd_set_devEUICustom},

    {((char *) &("devEUIBoard")),
     ((char *) &("Use board (boardID-based) LoRa device EUI")),
     2,
     0,
     &ketCube_LoRa_cmd_set_devEUIBoard},

    {((char *) &("OTAA")),
     ((char *) &("Enable OTAA")),
     2,
     0,
     &ketCube_LoRa_cmd_set_OTAA},

    {((char *) &("nwkSKey")),
     ((char *) &("Set LoRa network session key")),
     2,
     1,
     &ketCube_LoRa_cmd_set_nwkSKey},

#endif                          /* KETCUBE_CFG_INC_MOD_LORA */

#ifdef KETCUBE_CFG_INC_MOD_MBUS
    {((char *) &("MBUS")),
     ((char *) &("Set M-BUS parameters")),
     1,
     0,
     (void *) NULL},

     {((char *) &("attempts")),
     ((char *) &("Set M-BUS # attempts")),
     2,
     1,
     &ketCube_terminal_cmd_set_MBUS_RetryCount},
     
    {((char *) &("baudrate")),
     ((char *) &("Set M-BUS baudrate (use 300, 600, 1200, 2400, 4800, 9600, 19200 and 38400 only)")),
     2,
     1,
     &ketCube_terminal_cmd_set_MBUS_baudRate},

    {((char *) &("challenge")),
     ((char *) &("M-BUS challenge parameters")),
     2,
     0,
     (void *) NULL},

     {((char *) &("address")),
     ((char *) &("Address of slave to be challenged (0 - 253)")),
     3,
     1,
     &ketCube_terminal_cmd_set_MBUS_ChallengeAddress},

    {((char *) &("class")),
     ((char *) &("Class of request (1 or 2)")),
     3,
     1,
     &ketCube_terminal_cmd_set_MBUS_ChallengeClass},

    {((char *) &("response")),
     ((char *) &("M-BUS response cutting parameters")),
     2,
     0,
     (void *) NULL},

    {((char *) &("offset")),
     ((char *) &("Offset of data to be extracted (0 - 255)")),
     3,
     1,
     &ketCube_terminal_cmd_set_MBUS_ResponseOffset},

    {((char *) &("length")),
     ((char *) &("Length of data to be extracted, beginning from <offset> (1 - 255)")),
     3,
     1,
     &ketCube_terminal_cmd_set_MBUS_ResponseLength},

#endif        /* KETCUBE_CFG_INC_MOD_MBUS */
     
#ifdef KETCUBE_CFG_INC_MOD_S0
    {((char *) &("S0")),
     ((char *) &("Set S0 parameters")),
     1,
     0,
     (void *) NULL},

    {((char *) &("OnOff")),
     ((char *) &("Set S0 timers ON/OFF state.")),
     2,
     1,
     &ketCube_terminal_cmd_set_S0_OnOff},

     {((char *) &("Timeout")),
     ((char *) &("Set S0 timer's Timeout [minutes].")),
     2,
     2,
     &ketCube_terminal_cmd_set_S0_timeout},
     
#endif        /* KETCUBE_CFG_INC_MOD_S0 */

#ifdef KETCUBE_CFG_INC_MOD_MODBUS_AL
    {((char *) &("MODBUS")),
     ((char *) &("Set Modbus parameters")),
     1,
     0,
     (void *) NULL},
    
    {((char *) &("baudrate")),
     ((char *) &("Set ModBUS baudrate (use 300, 600, 1200, 2400, 4800, 9600, 19200 or 38400 only)")),
     2,
     1,
     &ketCube_terminal_cmd_set_ModBUS_AL_BaudRate},

    {((char *) &("challenge")),
     ((char *) &("Set Modbus challenge parameters")),
     2,
     0,
     (void *) NULL},

    {((char *) &("address")),
     ((char *) &("Set address of slave to be challenged")),
     3,
     1,
     &ketCube_terminal_cmd_set_ModBUS_AL_ChallengeAddress},

    {((char *) &("coil")),
     ((char *) &("Set coil challenge parameters")),
     3,
     0,
     (void *) NULL},

    {((char *) &("start")),
     ((char *) &("Set coil starting address")),
     4,
     1,
     &ketCube_terminal_cmd_set_ModBUS_AL_CoilStart},
		
    {((char *) &("count")),
     ((char *) &("Set coil count")),
     4,
     1,
     &ketCube_terminal_cmd_set_ModBUS_AL_CoilCount},
		
    {((char *) &("discreteinput")),
     ((char *) &("Set discrete input challenge parameters")),
     3,
     0,
     (void *) NULL},

    {((char *) &("start")),
     ((char *) &("Set discrete input starting address")),
     4,
     1,
     &ketCube_terminal_cmd_set_ModBUS_AL_DiscreteInputStart},
		
    {((char *) &("count")),
     ((char *) &("Set discrete input count")),
     4,
     1,
     &ketCube_terminal_cmd_set_ModBUS_AL_DiscreteInputCount},
		
		{((char *) &("inputregister")),
     ((char *) &("Set input register challenge parameters")),
     3,
     0,
     (void *) NULL},

    {((char *) &("start")),
     ((char *) &("Set input register starting address")),
     4,
     1,
     &ketCube_terminal_cmd_set_ModBUS_AL_InputRegisterStart},
		
    {((char *) &("count")),
     ((char *) &("Set input register count")),
     4,
     1,
     &ketCube_terminal_cmd_set_ModBUS_AL_InputRegisterCount},

		{((char *) &("holdingregister")),
     ((char *) &("Set holding register challenge parameters")),
     3,
     0,
     (void *) NULL},

    {((char *) &("start")),
     ((char *) &("Set holding register starting address")),
     4,
     1,
     &ketCube_terminal_cmd_set_ModBUS_AL_HoldingRegisterStart},
		
    {((char *) &("count")),
     ((char *) &("Set input register count")),
     4,
     1,
     &ketCube_terminal_cmd_set_ModBUS_AL_HoldingRegisterCount},

#endif        /* KETCUBE_CFG_INC_MOD_MODBUS_AL */

    {(char *) NULL,
     (char *) NULL,
     0,
     0,
     (void *) NULL}
};


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
    KETCUBE_TERMINAL_PRINTF("%s Command-line Interface HELP", KETCUBE_CFG_DEV_NAME);
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
    KETCUBE_TERMINAL_PRINTF ("KETCube home: https://github.com/SmartCAMPUSZCU/KETCube-docs");
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
        ("Performing system reset and reloading %s configuration ...", KETCUBE_CFG_DEV_NAME);
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

    KETCUBE_TERMINAL_ENDL();

    KETCUBE_TERMINAL_PRINTF
        ("Available modules (E == Enabled; D == Disabled):");
    KETCUBE_TERMINAL_ENDL();

    for (i = 0; i < ketCube_modules_CNT; i++) {
        if (ketCube_cfg_Load
            (((uint8_t *) & data), (ketCube_cfg_moduleIDs_t) i,
             (ketCube_cfg_AllocEEPROM_t) 0,
             (ketCube_cfg_LenEEPROM_t) 1) == KETCUBE_CFG_OK) {
            KETCUBE_TERMINAL_PRINTF("\t");
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
}

/**
 * @brief Enable/disable given KETCube module
 *
 * @param enable enable given module
 * 
 */
void ketCube_terminal_cmd_enableDisable(bool enable)
{
    uint8_t j;
    uint16_t i;
    ketCube_cfg_ModuleCfgByte_t tmpCfgByte;

    for (i = 0; i < ketCube_modules_CNT; i++) {
        j = 0;
        while (ketCube_modules_List[i].name[j] != 0x00) {
            if (ketCube_modules_List[i].name[j] !=
                commandBuffer[commandParams + j]) {
                break;
            }
            j++;
        }
        if (ketCube_modules_List[i].name[j] == 0x00) {
            break;
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
      
      /* Enable RX/TX port clocks */
      KETCUBE_TERMINAL_USART_TX_GPIO_CLK_ENABLE();
      KETCUBE_TERMINAL_USART_RX_GPIO_CLK_ENABLE();
      
      /* UART TX GPIO pin configuration  */
      ketCube_UART_SetupPin(KETCUBE_TERMINAL_USART_TX_PIN, KETCUBE_TERMINAL_USART_TX_AF, KETCUBE_TERMINAL_USART_TX_GPIO_PORT);
      ketCube_UART_SetupPin(KETCUBE_TERMINAL_USART_RX_PIN, KETCUBE_TERMINAL_USART_RX_AF, KETCUBE_TERMINAL_USART_RX_GPIO_PORT);
}

void ketCube_terminal_usartIoDeInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
      
    KETCUBE_TERMINAL_USART_TX_GPIO_CLK_ENABLE();
    KETCUBE_TERMINAL_USART_RX_GPIO_CLK_ENABLE();
    
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
      
    GPIO_InitStructure.Pin =  KETCUBE_TERMINAL_USART_TX_PIN ;
    HAL_GPIO_Init(KETCUBE_TERMINAL_USART_TX_GPIO_PORT, &GPIO_InitStructure );
    
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
  HAL_UART_Receive_IT(&ketCube_terminal_UsartHandle, (uint8_t *)&usartRxBuffer[usartRxWrite], 1);
}

void ketCube_terminal_usartTx(void)
{
     HAL_NVIC_ClearPendingIRQ(KETCUBE_TERMINAL_USART_IRQn);
     HAL_UART_Receive_IT(&ketCube_terminal_UsartHandle, (uint8_t *)&usartRxBuffer[usartRxWrite], 1);
}

void ketCube_terminal_usartErrorCallback(void)
{
    HAL_UART_Receive_IT(&ketCube_terminal_UsartHandle, (uint8_t *)&usartRxBuffer[usartRxWrite], 1);
}

void ketCube_terminal_usartWakeupCallback(void)
{
    HAL_UART_Receive_IT(&ketCube_terminal_UsartHandle, (uint8_t *)&usartRxBuffer[usartRxWrite], 1);
}

void ketCube_terminal_UsartPrintVa(char *format, va_list args)
{
    uint8_t len, i;
    
    len = vsprintf(&(usartTxBuffer[0]), format, args);
    
    for (i = 0; i < len; i++) {
        HAL_UART_Transmit(&ketCube_terminal_UsartHandle,(uint8_t *) &(usartTxBuffer[i]), 1, 300);    
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

char GetNewChar ( void)
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
    ketCube_terminal_UsartHandle.Instance        = KETCUBE_TERMINAL_USART_INSTANCE;
    ketCube_terminal_UsartHandle.Init.BaudRate   = KETCUBE_TERMINAL_USART_BR;
    ketCube_terminal_UsartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    ketCube_terminal_UsartHandle.Init.StopBits   = UART_STOPBITS_1;
    ketCube_terminal_UsartHandle.Init.Parity     = UART_PARITY_NONE;
    ketCube_terminal_UsartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    ketCube_terminal_UsartHandle.Init.Mode       = UART_MODE_TX_RX;
    
    __HAL_RCC_USART1_CONFIG(RCC_USART1CLKSOURCE_HSI);
    
    /* register callbacks in generic UART manager */
    ketCube_terminal_UsartDescriptor.handle = &ketCube_terminal_UsartHandle;
    ketCube_terminal_UsartDescriptor.irqNumber = KETCUBE_TERMINAL_USART_IRQn;
    ketCube_terminal_UsartDescriptor.irqPriority = 0x1;
    ketCube_terminal_UsartDescriptor.irqSubPriority = 0;
    ketCube_terminal_UsartDescriptor.fnIoInit = &ketCube_terminal_usartIoInit;
    ketCube_terminal_UsartDescriptor.fnIoDeInit = &ketCube_terminal_usartIoDeInit;
    ketCube_terminal_UsartDescriptor.fnIRQCallback = &ketCube_terminal_usartTx;
    ketCube_terminal_UsartDescriptor.fnReceiveCallback = &ketCube_terminal_usartRx;
    ketCube_terminal_UsartDescriptor.fnTransmitCallback = &ketCube_terminal_usartTx;
    ketCube_terminal_UsartDescriptor.fnErrorCallback = &ketCube_terminal_usartErrorCallback;
    ketCube_terminal_UsartDescriptor.fnWakeupCallback = &ketCube_terminal_usartWakeupCallback;
    
    if (ketCube_UART_RegisterHandle(KETCUBE_TERMINAL_USART_CHANNEL, &ketCube_terminal_UsartDescriptor) != KETCUBE_CFG_MODULE_OK) {
        ketCube_common_BasicErrorHandler();
    }
    
    HAL_UART_Receive_IT(&ketCube_terminal_UsartHandle, (uint8_t *)&usartRxBuffer[usartRxWrite], 1);
    
    commandBuffer[0] = 0x00;

    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PRINTF("Welcome to %s Command-line Interface", KETCUBE_CFG_DEV_NAME);
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
  * Use when it is necessary to print something and not to disrupt
  *
  */
void ketCube_terminal_UpdateCmdLine(void)
{
    KETCUBE_TERMINAL_ENDL();
    KETCUBE_TERMINAL_PROMPT();
    KETCUBE_TERMINAL_PRINTF("%s", commandBuffer);
}

/**
  * @brief Return next command parameter index
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
//     char buff[128];
// 
//     va_list args;
//     va_start(args, format);
// 
//     vsprintf(&(buff[0]), format, args);
// 
//     va_end(args);
// 
//     KETCUBE_TERMINAL_PRINTF("%s", &(buff[0]));
//     ketCube_terminal_UpdateCmdLine();
    
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
//     char buff[128];
// 
//     va_list args;
//     va_start(args, format);
// 
//     vsprintf(&(buff[0]), format, args);
// 
//     va_end(args);
// 
//     KETCUBE_TERMINAL_PRINTF("%s", &(buff[0]));

    va_list args;
    va_start(args, format);

    ketCube_terminal_UsartPrintVa(format, args);
    
    va_end(args);
}


/**
  * @brief Print Debug info to serial line + newline
  *
  */
void ketCube_terminal_DebugPrintln(char *format, ...)
{
//     char buff[128];

    if (ketCube_modules_List[KETCUBE_LISTS_MODULEID_DEBUGDISPLAY].cfgByte.
        enable != TRUE) {
        return;
    }

//     va_list args;
//     va_start(args, format);
// 
//     vsprintf(&(buff[0]), format, args);
// 
//     va_end(args);
// 
//     KETCUBE_TERMINAL_PRINTF("%s", &(buff[0]));
//     ketCube_terminal_UpdateCmdLine();
    
    
    va_list args;
    va_start(args, format);

    ketCube_terminal_UsartPrintVa(format, args);
    ketCube_terminal_UpdateCmdLine();
    
    va_end(args);
}


/**
  * @brief Print Debug info to serial line
  *
  */
void ketCube_terminal_DebugPrint(char *format, ...)
{
//     char buff[128];

    if (ketCube_modules_List[KETCUBE_LISTS_MODULEID_DEBUGDISPLAY].cfgByte.
        enable != TRUE) {
        return;
    }

//     va_list args;
//     va_start(args, format);
// 
//     vsprintf(&(buff[0]), format, args);
// 
//     va_end(args);
// 
//     KETCUBE_TERMINAL_PRINTF("%s", &(buff[0]));
    
    
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
