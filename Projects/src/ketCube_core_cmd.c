/**
 * @file    ketCube_core_cmd.c
 * @author  Martin Ubl
 * @version 0.1
 * @date    2019-01-01
 * @brief   This file contains the KETCube core commandline definitions
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

 /* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __KETCUBE_CORE_CMD_H
#define __KETCUBE_CORE_CMD_H

#include "ketCube_cfg.h"
#include "ketCube_common.h"

/** @defgroup KETCube_core_CMD KETCube core CMD
  * @brief KETCube core commandline definitions
	* @ingroup KETCube_Terminal
  * @{
  */

/**
 * @brief Show KETCube base period
 * 
 */
void ketCube_terminal_cmd_show_core_basePeriod(void)
{
    commandIOParams.as_integer = ketCube_coreCfg_BasePeriod;
}

/**
 * @brief Show KETCube start delay
 * 
 */
void ketCube_terminal_cmd_show_core_startDelay(void)
{
    commandIOParams.as_integer = ketCube_coreCfg_StartDelay;
}

/**
 * @brief Show KETCube severity
 * 
 */
void ketCube_terminal_cmd_show_core_severity(void)
{
    const char* severity = "UNKNOWN";
    
    switch (ketCube_coreCfg_severity) {
        case KETCUBE_CFG_SEVERITY_NONE:
            severity = "NONE";
            break;
        case KETCUBE_CFG_SEVERITY_ERROR:
            severity = "ERROR";
            break;
        case KETCUBE_CFG_SEVERITY_INFO:
            severity = "INFO";
            break;
        case KETCUBE_CFG_SEVERITY_DEBUG:
            severity = "DEBUG";
            break;
    }
    
    snprintf(commandIOParams.as_string,
             KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH,
             "%s",
             severity);
}

/**
 * @brief Set KETCube base period
 * 
 */
void ketCube_terminal_cmd_set_core_basePeriod(void)
{
    if (ketCube_EEPROM_WriteBuffer
        (KETCUBE_EEPROM_ALLOC_CORE + KETCUBE_CORECFG_ADR_BASEPERIOD,
         (uint8_t *)&(commandIOParams.as_integer), 4)
            == KETCUBE_EEPROM_OK) {
        KETCUBE_TERMINAL_PRINTF
            ("KETCube Core base period is set to: %d ms",
                commandIOParams.as_integer);
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Set KETCube start delay
 * 
 */
void ketCube_terminal_cmd_set_core_startDelay(void)
{
    if (ketCube_EEPROM_WriteBuffer
        (KETCUBE_EEPROM_ALLOC_CORE + KETCUBE_CORECFG_ADR_STARTDELAY,
         (uint8_t *)&(commandIOParams.as_integer), 4)
            == KETCUBE_EEPROM_OK) {
        KETCUBE_TERMINAL_PRINTF("KETCube Start delay is set to: %d ms",
                                commandIOParams.as_integer);
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Set KETCube severity
 * 
 */
void ketCube_terminal_cmd_set_core_severity(void)
{
    KETCUBE_TERMINAL_PRINTF("Setting KETCube core severity: ");
    switch ((ketCube_severity_t) commandIOParams.as_integer) {
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
            commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_INVALID_PARAMS;
            return;
    }
    
    KETCUBE_TERMINAL_ENDL();
    
    if (ketCube_EEPROM_WriteBuffer
        (KETCUBE_EEPROM_ALLOC_CORE + KETCUBE_CORECFG_ADR_SEVERITY,
         (uint8_t *)&(commandIOParams.as_integer), 1)
            == KETCUBE_EEPROM_OK) {
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
    
    KETCUBE_TERMINAL_ENDL();
}

/**
 * @brief Show KETCube driver(s) severity
 * 
 */
void ketCube_terminal_cmd_show_driver_severity(void)
{
    const char* severity = "UNKNOWN";
    
    switch (ketCube_coreCfg_driverSeverity) {
        case KETCUBE_CFG_SEVERITY_NONE:
            severity = "NONE";
            break;
        case KETCUBE_CFG_SEVERITY_ERROR:
            severity = "ERROR";
            break;
        case KETCUBE_CFG_SEVERITY_INFO:
            severity = "INFO";
            break;
        case KETCUBE_CFG_SEVERITY_DEBUG:
            severity = "DEBUG";
            break;
    }
    
    snprintf(commandIOParams.as_string,
             KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH,
             "%s",
             severity);
}

/**
 * @brief Set KETCube driver(s) severity
 * 
 */
void ketCube_terminal_cmd_set_driver_severity(void)
{
    KETCUBE_TERMINAL_PRINTF("Setting KETCube driver(s) severity: ");
    switch ((ketCube_severity_t) commandIOParams.as_integer) {
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
            commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_INVALID_PARAMS;
            return;
    }
    
    KETCUBE_TERMINAL_ENDL();
    
    if (ketCube_EEPROM_WriteBuffer
        (KETCUBE_EEPROM_ALLOC_CORE + KETCUBE_CORECFG_ADR_DRIVER_SEVERITY,
         (uint8_t *)&(commandIOParams.as_integer), 1)
            == KETCUBE_EEPROM_OK) {
    } else {
        commandErrorCode = KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL;
    }
    
    KETCUBE_TERMINAL_ENDL();
}

/* Terminal command definitions */

ketCube_terminal_cmd_t ketCube_terminal_commands_show_core[] = {
    DEF_COMMAND("basePeriod",
                "KETCube base period",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_INTEGER,
                &ketCube_terminal_cmd_show_core_basePeriod),
    DEF_COMMAND("startDelay",
                "First periodic action is delayed after power-up",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_INTEGER,
                &ketCube_terminal_cmd_show_core_startDelay),
    DEF_COMMAND("severity",
                "Core messages severity",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_STRING,
                &ketCube_terminal_cmd_show_core_severity),
    DEF_TERMINATE()
};

ketCube_terminal_cmd_t ketCube_terminal_commands_set_core[] = {
    DEF_COMMAND("basePeriod",
                "KETCube base period",
                KETCUBE_TERMINAL_PARAMS_INTEGER,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_terminal_cmd_set_core_basePeriod),
    DEF_COMMAND("startDelay",
                "First periodic action is delayed after power-up",
                KETCUBE_TERMINAL_PARAMS_INTEGER,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_terminal_cmd_set_core_startDelay),
    DEF_COMMAND("severity",
                "Core messages severity: 0 = NONE, 1 = ERROR; 2 = INFO;"
                " 3 = DEBUG",
                KETCUBE_TERMINAL_PARAMS_INTEGER,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_terminal_cmd_set_core_severity),
    DEF_TERMINATE()
};

/* Terminal command definitions for driver subgroup */

ketCube_terminal_cmd_t ketCube_terminal_commands_show_driver[] = {
    DEF_COMMAND("severity",
                "Driver(s) messages severity",
                KETCUBE_TERMINAL_PARAMS_NONE,
                KETCUBE_TERMINAL_PARAMS_STRING,
                &ketCube_terminal_cmd_show_driver_severity),
    DEF_TERMINATE()
};

ketCube_terminal_cmd_t ketCube_terminal_commands_set_driver[] = {
    DEF_COMMAND("severity",
                "Driver(s) messages severity: 0 = NONE, 1 = ERROR; 2 = INFO;"
                " 3 = DEBUG",
                KETCUBE_TERMINAL_PARAMS_INTEGER,
                KETCUBE_TERMINAL_PARAMS_NONE,
                &ketCube_terminal_cmd_set_driver_severity),
    DEF_TERMINATE()
};

/**
* @}
*/

#endif                          /* KETCUBE_CORE_CMD_H */
