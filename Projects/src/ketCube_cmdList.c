/**
 * @file    ketCube_cmdList.c
 * @author  Jan Belohoubek
 * @version 0.2
 * @date    2018-10-30
 * @brief   This file contains the KETCube Command List
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

#include "ketCube_terminal.h"
#include "ketCube_common.h"
#include "ketCube_cfg.h"

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
     
    {((char *) &("severity")),
     ((char *) &("Core messages severity")),
     2,
     0,
     &ketCube_terminal_cmd_show_core_severity},

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
     
    {((char *) &("severity")),
     ((char *) &("Core messages severity: 0 = NONE, 1 = ERROR; 2 = INFO; 3 = DEBUG")),
     2,
     1,
     &ketCube_terminal_cmd_set_core_severity},

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
