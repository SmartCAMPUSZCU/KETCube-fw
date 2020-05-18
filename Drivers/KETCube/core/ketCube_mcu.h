/**
 * @file    ketCube_mcu.h
 * @author  Jan Belohoubek
 * @version alpha
 * @date    2019-12-10
 * @brief   This file contains the KETCube MCU defs
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 University of West Bohemia in Pilsen
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

#ifndef __KETCUBE_MCU_H
#define __KETCUBE_MCU_H

#include "ketCube_cfg.h"

/** @defgroup KETCube_MCU KETCube MCU
  * @brief KETCube MCU
  * @ingroup KETCube_Core
  * @{
  */

/* Low Power modes selection */
#define KETCUBE_MCU_LPMODE_STOP   0
#define KETCUBE_MCU_LPMODE_SLEEP  1

#define KETCUBE_MCU_LPMODE   KETCUBE_MCU_LPMODE_STOP

#define KETCUBE_MCU_WD_SAFE_TIMER_CNT      25                         /*<! Watchdog reset period in (pseudo)seconds (seconds are approximated by the closest power of 2); use 30 as the maxValue ! */

/**
 * @brief HardFault registers should be dumped to investigate reset reason
 * 
 * @note The Segger's appnote https://www.segger.com/downloads/application-notes/AN00016 has been used to define dbg struct for HardFault reasoning
 * 
 */
typedef struct ketCube_MCU_HardFaultRegs_t {
  struct {
    volatile unsigned int r0;            // Register R0
    volatile unsigned int r1;            // Register R1
    volatile unsigned int r2;            // Register R2
    volatile unsigned int r3;            // Register R3
    volatile unsigned int r12;           // Register R12
    volatile unsigned int lr;            // Link register
    volatile unsigned int pc;            // Program counter
    union {
      volatile unsigned int word;
      struct {
        unsigned int IPSR : 8;           // Interrupt Program Status register (IPSR)
        unsigned int EPSR : 19;          // Execution Program Status register (EPSR)
        unsigned int APSR : 5;           // Application Program Status register (APSR)
      } bits;
    } psr;                               // Program status register.
  } SavedRegs;

  union {
    volatile unsigned int word;
    struct {
      unsigned int MEMFAULTACT    : 1;   // Read as 1 if memory management fault is active
      unsigned int BUSFAULTACT    : 1;   // Read as 1 if bus fault exception is active
      unsigned int UnusedBits1    : 1;
      unsigned int USGFAULTACT    : 1;   // Read as 1 if usage fault exception is active
      unsigned int UnusedBits2    : 3;
      unsigned int SVCALLACT      : 1;   // Read as 1 if SVC exception is active
      unsigned int MONITORACT     : 1;   // Read as 1 if debug monitor exception is active
      unsigned int UnusedBits3    : 1;
      unsigned int PENDSVACT      : 1;   // Read as 1 if PendSV exception is active
      unsigned int SYSTICKACT     : 1;   // Read as 1 if SYSTICK exception is active
      unsigned int USGFAULTPENDED : 1;   // Usage fault pended; usage fault started but was replaced by a higher-priority exception
      unsigned int MEMFAULTPENDED : 1;   // Memory management fault pended; memory management fault started but was replaced by a higher-priority exception
      unsigned int BUSFAULTPENDED : 1;   // Bus fault pended; bus fault handler was started but was replaced by a higher-priority exception
      unsigned int SVCALLPENDED   : 1;   // SVC pended; SVC was started but was replaced by a higher-priority exception
      unsigned int MEMFAULTENA    : 1;   // Memory management fault handler enable
      unsigned int BUSFAULTENA    : 1;   // Bus fault handler enable
      unsigned int USGFAULTENA    : 1;   // Usage fault handler enable
    } bits;
  } syshndctrl;                          // System Handler Control and State Register (0xE000ED24)

  union {
    volatile unsigned char word;
    struct {
      unsigned char IACCVIOL    : 1;     // Instruction access violation
      unsigned char DACCVIOL    : 1;     // Data access violation
      unsigned char UnusedBits  : 1;
      unsigned char MUNSTKERR   : 1;     // Unstacking error
      unsigned char MSTKERR     : 1;     // Stacking error
      unsigned char UnusedBits2 : 2;
      unsigned char MMARVALID   : 1;     // Indicates the MMAR is valid
    } bits;
  } mfsr;                                // Memory Management Fault Status Register (0xE000ED28)

  union {
    volatile unsigned int word;
    struct {
      unsigned int IBUSERR    : 1;       // Instruction access violation
      unsigned int PRECISERR  : 1;       // Precise data access violation
      unsigned int IMPREISERR : 1;       // Imprecise data access violation
      unsigned int UNSTKERR   : 1;       // Unstacking error
      unsigned int STKERR     : 1;       // Stacking error
      unsigned int UnusedBits : 2;
      unsigned int BFARVALID  : 1;       // Indicates BFAR is valid
    } bits;
  } bfsr;                                // Bus Fault Status Register (0xE000ED29)
  volatile unsigned int bfar;            // Bus Fault Manage Address Register (0xE000ED38)

  union {
    volatile unsigned short word;
    struct {
      unsigned short UNDEFINSTR : 1;     // Attempts to execute an undefined instruction
      unsigned short INVSTATE   : 1;     // Attempts to switch to an invalid state (e.g., ARM)
      unsigned short INVPC      : 1;     // Attempts to do an exception with a bad value in the EXC_RETURN number
      unsigned short NOCP       : 1;     // Attempts to execute a coprocessor instruction
      unsigned short UnusedBits : 4;
      unsigned short UNALIGNED  : 1;     // Indicates that an unaligned access fault has taken place
      unsigned short DIVBYZERO  : 1;     // Indicates a divide by zero has taken place (can be set only if DIV_0_TRP is set)
    } bits;
  } ufsr;                                // Usage Fault Status Register (0xE000ED2A)

  union {
    volatile unsigned int word;
    struct {
      unsigned int UnusedBits  : 1;
      unsigned int VECTBL      : 1;      // Indicates hard fault is caused by failed vector fetch
      unsigned int UnusedBits2 : 28;
      unsigned int FORCED      : 1;      // Indicates hard fault is taken because of bus fault/memory management fault/usage fault
      unsigned int DEBUGEVT    : 1;      // Indicates hard fault is triggered by debug event
    } bits;
  } hfsr;                                // Hard Fault Status Register (0xE000ED2C)

  union {
    volatile unsigned int word;
    struct {
      unsigned int HALTED   : 1;         // Halt requested in NVIC
      unsigned int BKPT     : 1;         // BKPT instruction executed
      unsigned int DWTTRAP  : 1;         // DWT match occurred
      unsigned int VCATCH   : 1;         // Vector fetch occurred
      unsigned int EXTERNAL : 1;         // EDBGRQ signal asserted
    } bits;
  } dfsr;                                // Debug Fault Status Register (0xE000ED30)

  volatile unsigned int afsr;            // Auxiliary Fault Status Register (0xE000ED3C), Vendor controlled (optional)
} ketCube_MCU_HardFaultRegs_t;

/** @defgroup KETCube_MCU_fn Public Functions
* @{
*/

extern uint32_t ketCube_MCU_GetRandomSeed(void);
extern void ketCube_MCU_GetUniqueId(uint8_t *id);

extern void ketCube_MCU_Sleep(void);
extern void ketCube_MCU_EnableSleep(void);
extern void ketCube_MCU_DisableSleep(void);
extern bool ketCube_MCU_IsSleepEnabled(void);

extern void ketCube_MCU_WD_Init(void);
extern void ketCube_MCU_WD_Reset(void);

extern void ketCube_MCU_ClockConfig(void);

extern void ketCube_MCU_DumpHardFaultRegs(ketCube_MCU_HardFaultRegs_t * HardFaultRegs);

/**
* @}
*/



/**
* @}
*/

#endif                          /* __KETCUBE_MCU_H */
