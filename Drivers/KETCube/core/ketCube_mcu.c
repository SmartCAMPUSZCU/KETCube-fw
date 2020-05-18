/**
 * @file    ketCube_mcu.c
 * @author  Jan Belohoubek
 * @version alpha
 * @date    2019-12-10
 * @brief   This file contains the KETCube MCU implementations
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


/* BACKUP_PRIMASK MUST be implemented at the begining of the funtion 
   that implement a critical section                        
   PRIMASK is saved on STACK and recovered at the end of the funtion
   That way RESTORE_PRIMASK ensures critical sections are maintained even in nested calls...*/

#define BACKUP_PRIMASK()  uint32_t primask_bit= __get_PRIMASK()
#define DISABLE_IRQ() __disable_irq()
#define ENABLE_IRQ() __enable_irq()
#define RESTORE_PRIMASK() __set_PRIMASK(primask_bit)

#include "debug.h"
#include "ketCube_mcu.h"
#include "ketCube_resetMan.h"
#include "ketCube_coreCfg.h"
#include "timeServer.h"
#include "stm32l0xx_hal_pwr.h"

/**
 *  @brief Unique Devices IDs register set ( STM32L0xxx )
 */
#define         ID1                                 ( 0x1FF80050 )
#define         ID2                                 ( 0x1FF80054 )
#define         ID3                                 ( 0x1FF80064 )


static volatile bool enableSleep = TRUE;

static TimerEvent_t KETCube_WDCheckTimer;  /* Timer to check WD */
static IWDG_HandleTypeDef hiwdg; /* IWDG handler */

PWR_PVDTypeDef sConfigPVD = {0};
volatile bool ketCube_MCU_PVD_Det = FALSE;

static volatile bool ketCube_MCU_WD_timWake = FALSE;

/**
  * @brief This function return a random seed
  * @note Seed is Based on the device unique ID
  * 
  * @retval seed
  */
uint32_t ketCube_MCU_GetRandomSeed(void) {
  return ( ( *( uint32_t* )ID1 ) ^ ( *( uint32_t* )ID2 ) ^ ( *( uint32_t* )ID3 ) );
}

/**
  * @brief This function returns a unique MCU ID
  * 
  * @param id
  */
void ketCube_MCU_GetUniqueId(uint8_t *id) {
    id[7] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) ) >> 24;
    id[6] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) ) >> 16;
    id[5] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) ) >> 8;
    id[4] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) );
    id[3] = ( ( *( uint32_t* )ID2 ) ) >> 24;
    id[2] = ( ( *( uint32_t* )ID2 ) ) >> 16;
    id[1] = ( ( *( uint32_t* )ID2 ) ) >> 8;
    id[0] = ( ( *( uint32_t* )ID2 ) );
}

/**
 * @brief Enable low power mode ...
 * 
 */
void ketCube_MCU_EnableSleep(void) {
    enableSleep = TRUE;
}

/**
 * @brief Disable low power mode ...
 * 
 */
void ketCube_MCU_DisableSleep(void) {
    enableSleep = FALSE;
}

/**
 * @brief Disable low power mode ...
 * 
 */
bool ketCube_MCU_IsSleepEnabled(void) {
    return enableSleep;
}

/**
  * @brief Exists Low Power Stop Mode
  * 
  * @note Enable the pll at 32MHz
  * 
  */
static void ketCube_MCU_ExitStopMode(void) {
  /* Disable IRQ while the MCU is not running on HSI */

  BACKUP_PRIMASK();
  
  DISABLE_IRQ( );

  /* After wake-up from STOP reconfigure the system clock */
  /* Enable HSI */
  __HAL_RCC_HSI_ENABLE();

  /* Wait till HSI is ready */
  while( __HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET ) {}
  
  /* Enable PLL */
  __HAL_RCC_PLL_ENABLE();
  /* Wait till PLL is ready */
  while( __HAL_RCC_GET_FLAG( RCC_FLAG_PLLRDY ) == RESET ) {}
  
  /* Select PLL as system clock source */
  __HAL_RCC_SYSCLK_CONFIG ( RCC_SYSCLKSOURCE_PLLCLK );
  
  /* Wait till PLL is used as system clock source */ 
  while( __HAL_RCC_GET_SYSCLK_SOURCE( ) != RCC_SYSCLKSOURCE_STATUS_PLLCLK ) {}
    
  /*initilizes the peripherals*/
  ketCube_UART_IoInitAll();

  RESTORE_PRIMASK( );
}

/**
  * @brief Enters Low Power Stop Mode
  * 
  * @note ARM exists the function when waking up
  * 
  */
void ketCube_MCU_EnterStopMode(void) {
  BACKUP_PRIMASK();

  DISABLE_IRQ( );

  /* DeIntit peripherals ... */
  ketCube_UART_IoDeInitAll();
  
  /*clear wake up flag*/
  SET_BIT(PWR->CR, PWR_CR_CWUF);
  
  RESTORE_PRIMASK( );

  /* Enter Stop Mode */
  HAL_PWR_EnterSTOPMode ( PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI );
}

/**
  * @brief Enters Low Power Sleep Mode
  * 
  * @note ARM exits the function when waking up
  */
void ketCube_MCU_EnterSleepMode( void)
{
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

/**
 * @brief Handle KETCube LowPower mode(s)
 * 
 */
void ketCube_MCU_Sleep(void) {
#ifndef LOW_POWER_DISABLE
    if (enableSleep == TRUE) {
        
#if (KETCUBE_MCU_LPMODE == KETCUBE_MCU_LPMODE_SLEEP)
        ketCube_MCU_EnterSleepMode();
#else 
        ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_DEBUG, "Entering Stop Mode");
        
        ketCube_MCU_EnterStopMode();
        
        // Stop mode ...
        
        ketCube_MCU_ExitStopMode();
        
        ketCube_RTC_setMcuWakeUpTime();
        
        ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_DEBUG, "Exiting Stop Mode");
        
#endif // (KETCUBE_MCU_LPMODE == KETCUBE_MCU_LPMODE_SLEEP)        
    }
#endif  /* LOW_POWER_DISABLE */
}

/**
  * @brief  Clock Configuration
  *         The system Clock is configured as follows:
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 32000000
  *            HCLK(Hz)                       = 32000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            PLLMUL                         = 6
  *            PLLDIV                         = 3
  *            Flash Latency(WS)              = 1
  * 
  */
void ketCube_MCU_ClockConfig(void) {
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    
    /* Enable HSE Oscillator and Activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSEState            = RCC_HSE_OFF;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.LSIState            = RCC_LSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL          = RCC_PLLMUL_6;
    RCC_OscInitStruct.PLL.PLLDIV          = RCC_PLLDIV_3;
    
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
      KETCube_ErrorHandler();
    }
    
    /* Set Voltage scale1 as MCU will run at 32MHz */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    
    /* Poll VOSF bit of in PWR_CSR. Wait until it is reset to 0 */
    while (__HAL_PWR_GET_FLAG(PWR_FLAG_VOS) != RESET) {};
    
    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
    clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
    {
      KETCube_ErrorHandler();
    }
}

/**
  * @brief Monitor WakeUp reasoning
  *
  * @note Do not remove this function, as it prevents optimization of safe-timer
  * 
  */
void ketCube_MCU_WD_Monitor(void* context) {
    ketCube_MCU_WD_timWake = TRUE;
}

/**
  * @brief KETCube Watchdog (IWDG) Setup
  *
  */
void ketCube_MCU_WD_Init(void) {
    
#ifdef KETCUBE_ENABLE_WD
    
    hiwdg.Instance = IWDG;
    /* For IoT nodes, the slowest clock are normally advantageous */
    hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
    hiwdg.Init.Window = 4095;  // (128 << 32) - 1 == 0xFFF
    hiwdg.Init.Reload = 4095;  // (128 << 32) - 1 == 0xFFF
    if (HAL_IWDG_Init(&hiwdg) != HAL_OK) {
        KETCube_ErrorHandler();
    }
    
    // Initialize timer to wake-up MCU periodically
    TimerInit(&KETCube_WDCheckTimer, &ketCube_MCU_WD_Monitor);
    TimerSetValue(&KETCube_WDCheckTimer, 1000 * KETCUBE_MCU_WD_SAFE_TIMER_CNT);
    TimerStart(&KETCube_WDCheckTimer);
#endif
    
}

/**
  * @brief KETCube Watchdog (IWDG) Reset
  *
  */
void ketCube_MCU_WD_Reset(void) {

#ifdef KETCUBE_ENABLE_WD
    
    HAL_IWDG_Refresh(&hiwdg);
    
    // WD periodic reset
    if (ketCube_MCU_WD_timWake == TRUE) {
        ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_DEBUG, "KETCube WD Reset - safe-counter");
    } else {
        ketCube_terminal_CoreSeverityPrintln(KETCUBE_CFG_SEVERITY_DEBUG, "KETCube WD Reset - application flow");
    }
    
    ketCube_MCU_WD_timWake = FALSE;
    
    /* wake-up peridically - set the (max) wake-up alarm (race conditions are ignored!) */
    TimerStop(&KETCube_WDCheckTimer);
    TimerSetValue(&KETCube_WDCheckTimer, 1000 * KETCUBE_MCU_WD_SAFE_TIMER_CNT);
    TimerStart(&KETCube_WDCheckTimer);
    
#endif
    
}

/**
  * @brief KETCube Voltage Detector init
  * 
  * @note Some KETCube parts are not operationalunder certain voltage. E.g. HDC1080 is not opperational below 2V7 (HDC2080 will be used in next platform release, thus 1V62 will be the minimal voltage level for RHT sensor).
  * @note Below 1V9 is realy "dangerous" voltage, thus PWR_PVDLEVEL_0 is used as a default value
  *
  */
static void ketCube_VoltageDet_Init(void) {
    /* Enable the PVD Output */
    HAL_PWR_DisablePVD();
    
    /* PVD Configuration */
    sConfigPVD.PVDLevel = PWR_PVDLEVEL_0;
    sConfigPVD.Mode = PWR_PVD_MODE_IT_RISING;
    HAL_PWR_ConfigPVD(&sConfigPVD);
    
    /* Enable the PVD Output */
    HAL_PWR_EnablePVD();
    
    /* Enable and set PVD Interrupt to highest priority */
    HAL_NVIC_SetPriority(PVD_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(PVD_IRQn);
}

/**
  * @brief Programmable Voltage Detector (PVD) IRQ
  *
  */
void PVD_IRQHandler(void) {    
    HAL_PWR_PVD_IRQHandler();
    
    /* to verify, that flags were cleared */
    do {
        __HAL_PWR_PVD_EXTI_CLEAR_FLAG();
        HAL_NVIC_ClearPendingIRQ(PVD_IRQn);
    } while ((HAL_NVIC_GetPendingIRQ(PVD_IRQn)) || __HAL_PWR_PVD_EXTI_GET_FLAG());
}

/**
  * @brief Programmable Voltage Detector (PVD) Callback
  * 
  * @todo The ketCube_MCU_PVD_Det flag should be used to trigger an action and after triggering it, it should be cleared
  * 
  */
void HAL_PWR_PVDCallback(void) {
    ketCube_MCU_PVD_Det = TRUE;
}

/**
  * @brief  Initializes the MSP
  * 
  * @retval None
  */
void HAL_MspInit(void) {
  __HAL_RCC_PWR_CLK_ENABLE();

#ifdef KETCUBE_ENABLE_PVD
  /* Use Voltage detector to detect "safe" voltage region */
  ketCube_VoltageDet_Init( );
#else
  /* Disable the Power Voltage Detector */
  HAL_PWR_DisablePVD( );
  /* Enables the Ultra Low Power mode */
  HAL_PWREx_EnableUltraLowPower( );
#endif
  
  __HAL_FLASH_SLEEP_POWERDOWN_ENABLE();
  
  /*In debug mode, e.g. when DBGMCU is activated, Arm core has always clocks
   * And will not wait that the FLASH is ready to be read. It can miss in this 
   * case the first instruction. To overcome this issue, the flash remain clocked during sleep mode
   */
  DBG( __HAL_FLASH_SLEEP_POWERDOWN_DISABLE(); );
  
#ifdef KETCUBE_ENABLE_FAST_WAKEUP
  /*Enable fast wakeUp*/  
  HAL_PWREx_EnableFastWakeUp( );
#else  
  HAL_PWREx_DisableFastWakeUp( );
#endif

  /* Initialize KETCube mainBoard */
  ketCube_mainBoard_Init();
}

/* --- Advanced Hard Fault handling --- */

#define SYSHND_CTRL  (*(volatile unsigned int*)  (0xE000ED24u))  // System Handler Control and State Register

#define NVIC_MFSR    (*(volatile unsigned char*) (0xE000ED28u))  // Memory Management Fault Status Register
#define NVIC_BFSR    (*(volatile unsigned char*) (0xE000ED29u))  // Bus Fault Status Register
#define NVIC_UFSR    (*(volatile unsigned short*)(0xE000ED2Au))  // Usage Fault Status Register
#define NVIC_HFSR    (*(volatile unsigned int*)  (0xE000ED2Cu))  // Hard Fault Status Register
#define NVIC_DFSR    (*(volatile unsigned int*)  (0xE000ED30u))  // Debug Fault Status Register
#define NVIC_BFAR    (*(volatile unsigned int*)  (0xE000ED38u))  // Bus Fault Manage Address Register
#define NVIC_AFSR    (*(volatile unsigned int*)  (0xE000ED3Cu))  // Auxiliary Fault Status Register

static volatile bool HardFaultHandler_ContinueA = TRUE; ///< Set THIS variables to FALSE if you would like to debug HardFault with debug probe instead of using KETCube's way
static volatile bool HardFaultHandler_ContinueB = TRUE; ///< Set THIS variables to FALSE if you would like to debug HardFault with debug probe instead of using KETCube's way

/**
 * @brief Advanced HardFault handler
 * 
 * @note The Segger's appnote https://www.segger.com/downloads/application-notes/AN00016 has been used to define this handler
 * 
 */
void HardFaultHandler(unsigned int* pStack) {
    // In case we received a hard fault because of a breakpoint instruction, we return.
    // This may happen when using semihosting for printf outputs and no debugger is connected,
    // i.e. when running a "Debug" configuration in release mode.
    //
    if (NVIC_HFSR & (1u << 31)) {
        NVIC_HFSR |=  (1u << 31);     // Reset Hard Fault status
        *(pStack + 6u) += 2u;         // PC is located on stack at SP + 24 bytes. Increment PC by 2 to skip break instruction.
        return;                       // Return to interrupted application
    }
    
    // DBG-info structure 
    ketCube_MCU_HardFaultRegs_t * HardFaultRegs = &(ketCube_coreCfg.volatileData.resetInfo.info.dbg.hardFault);
    
    //
    // Store NVIC registers
    (*HardFaultRegs).syshndctrl.word = SYSHND_CTRL;  // System Handler Control and State Register
    (*HardFaultRegs).mfsr.word       = NVIC_MFSR;    // Memory Fault Status Register
    (*HardFaultRegs).bfsr.word       = NVIC_BFSR;    // Bus Fault Status Register
    (*HardFaultRegs).bfar            = NVIC_BFAR;    // Bus Fault Manage Address Register
    (*HardFaultRegs).ufsr.word       = NVIC_UFSR;    // Usage Fault Status Register
    (*HardFaultRegs).hfsr.word       = NVIC_HFSR;    // Hard Fault Status Register
    (*HardFaultRegs).dfsr.word       = NVIC_DFSR;    // Debug Fault Status Register
    (*HardFaultRegs).afsr            = NVIC_AFSR;    // Auxiliary Fault Status Register
    
    
    // Halt execution
    // When using debbuger to investigate HardFault reasoning, you can modify this variable to stay in HardFault, then recompile and you will suck here
    while (HardFaultHandler_ContinueA == FALSE) {
        
    }
    
    //
    // Read saved registers from the stack.
    //
    (*HardFaultRegs).SavedRegs.r0       = pStack[0];  // Register R0
    (*HardFaultRegs).SavedRegs.r1       = pStack[1];  // Register R1
    (*HardFaultRegs).SavedRegs.r2       = pStack[2];  // Register R2
    (*HardFaultRegs).SavedRegs.r3       = pStack[3];  // Register R3
    (*HardFaultRegs).SavedRegs.r12      = pStack[4];  // Register R12
    (*HardFaultRegs).SavedRegs.lr       = pStack[5];  // Link register LR
    (*HardFaultRegs).SavedRegs.pc       = pStack[6];  // Program counter PC
    (*HardFaultRegs).SavedRegs.psr.word = pStack[7];  // Program status word PSR
    
    // Halt execution
    // When using debbuger to investigate HardFault reasoning, you can modify this variable to stay in HardFault, then recompile and you will suck here
    while (HardFaultHandler_ContinueB == FALSE) {
        
    }
    
    // Invoke reset 
    // Rely on KETCube's reset reasoning method - HardFault reasons SHOULD be printed (if the fault source is not related to critical part of KETCube core itself, which is the common case)
    ketCube_resetMan_requestReset(KETCUBE_RESETMAN_REASON_HARDFAULT);
}

/**
 * @brief Dump HardFault Register Content
 * 
 */
void ketCube_MCU_DumpHardFaultRegs(ketCube_MCU_HardFaultRegs_t * HardFaultRegs) {
    KETCUBE_TERMINAL_CLR_LINE();
    
    KETCUBE_TERMINAL_PRINTF("System Control Block Registers:");
    KETCUBE_TERMINAL_ENDL(); 
    KETCUBE_TERMINAL_PRINTF("  - SHCSR  = %08X  // System Handler Control and State Register", (*HardFaultRegs).syshndctrl.word);
    KETCUBE_TERMINAL_ENDL(); 

// following registers are not implemented in ARM M0+
#ifndef USE_B_L082Z_KETCube    
    KETCUBE_TERMINAL_ENDL();    
    
    KETCUBE_TERMINAL_PRINTF("NVIC Registers:");
    KETCUBE_TERMINAL_ENDL(); 
    KETCUBE_TERMINAL_PRINTF("  - MFSR   = %08X  // Memory Fault Status Register",              (*HardFaultRegs).mfsr.word      ); KETCUBE_TERMINAL_ENDL();    
    KETCUBE_TERMINAL_PRINTF("  - BFSR   = %08X  // Bus Fault Status Register",                 (*HardFaultRegs).bfsr.word      ); KETCUBE_TERMINAL_ENDL();    
    KETCUBE_TERMINAL_PRINTF("  - BFAR   = %08X  // Bus Fault Manage Address Register",         (*HardFaultRegs).bfar           ); KETCUBE_TERMINAL_ENDL();    
    KETCUBE_TERMINAL_PRINTF("  - UFSR   = %08X  // Usage Fault Status Register",               (*HardFaultRegs).ufsr.word      ); KETCUBE_TERMINAL_ENDL();    
    KETCUBE_TERMINAL_PRINTF("  - HFSR   = %08X  // Hard Fault Status Register",                (*HardFaultRegs).hfsr.word      ); KETCUBE_TERMINAL_ENDL();    
    KETCUBE_TERMINAL_PRINTF("  - DFSR   = %08X  // Debug Fault Status Register",               (*HardFaultRegs).dfsr.word      ); KETCUBE_TERMINAL_ENDL();    
    KETCUBE_TERMINAL_PRINTF("  - AFSR   = %08X  // Auxiliary Fault Status Register",           (*HardFaultRegs).afsr           ); KETCUBE_TERMINAL_ENDL();    
#endif

// following registers are not very useful for HardFault investigation, remove #ifndef directive if needed for ARM M0+ debugging
#ifndef USE_B_L082Z_KETCube   
    KETCUBE_TERMINAL_ENDL();  
    
    KETCUBE_TERMINAL_PRINTF("Saved registers from the stack:");
    KETCUBE_TERMINAL_ENDL();    
    KETCUBE_TERMINAL_PRINTF("  - R0     = %08X  // Register R0",                     (*HardFaultRegs).SavedRegs.r0      ); KETCUBE_TERMINAL_ENDL();    
    KETCUBE_TERMINAL_PRINTF("  - R1     = %08X  // Register R1",                     (*HardFaultRegs).SavedRegs.r1      ); KETCUBE_TERMINAL_ENDL();    
    KETCUBE_TERMINAL_PRINTF("  - R2     = %08X  // Register R2",                     (*HardFaultRegs).SavedRegs.r2      ); KETCUBE_TERMINAL_ENDL();    
    KETCUBE_TERMINAL_PRINTF("  - R3     = %08X  // Register R3",                     (*HardFaultRegs).SavedRegs.r3      ); KETCUBE_TERMINAL_ENDL();    
    KETCUBE_TERMINAL_PRINTF("  - R12    = %08X  // Register R12",                    (*HardFaultRegs).SavedRegs.r12     ); KETCUBE_TERMINAL_ENDL();    
    KETCUBE_TERMINAL_PRINTF("  - LR     = %08X  // Link register LR",                (*HardFaultRegs).SavedRegs.lr      ); KETCUBE_TERMINAL_ENDL();    
    KETCUBE_TERMINAL_PRINTF("  - PC     = %08X  // Program counter PC",              (*HardFaultRegs).SavedRegs.pc      ); KETCUBE_TERMINAL_ENDL();    
    KETCUBE_TERMINAL_PRINTF("  - PSR    = %08X  // Program status word PSR",         (*HardFaultRegs).SavedRegs.psr.word); KETCUBE_TERMINAL_ENDL();    
    
#endif
    
}
