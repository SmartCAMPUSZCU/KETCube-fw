# Copyright (c) 2018 - 2020 University of West Bohemia in Pilsen
# All rights reserved.
#
# Developed by:
# The SmartCampus Team
# Department of Technologies and Measurement
# www.smartcampus.cz | www.zcu.cz
#
# Permission is hereby granted, free of charge, to any person obtaining a copy 
# of this software and associated documentation files (the "Software"), 
# to deal with the Software without restriction, including without limitation 
# the rights to use, copy, modify, merge, publish, distribute, sublicense, 
# and/or sell copies of the Software, and to permit persons to whom the Software 
# is furnished to do so, subject to the following conditions:
#
#    - Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimers.
#    
#    - Redistributions in binary form must reproduce the above copyright notice, 
#      this list of conditions and the following disclaimers in the documentation 
#      and/or other materials provided with the distribution.
#    
#    - Neither the names of The SmartCampus Team, Department of Technologies and Measurement
#      and Faculty of Electrical Engineering University of West Bohemia in Pilsen, 
#      nor the names of its contributors may be used to endorse or promote products 
#      derived from this Software without specific prior written permission. 
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
# PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT HOLDERS 
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
# OR THE USE OR OTHER DEALINGS WITH THE SOFTWARE. 
 

TARGET=KETCube
COREDIR=../../
OUTDIR = ./build/

BUILD_ID = $(shell git rev-parse --short HEAD)
VERSION  = $(shell cat ../../VERSION | head -n 1)

###################################################

DEBUG = -g3
OPTIMIZE = -Os

###################################################

CC=arm-none-eabi-gcc
LD=arm-none-eabi-gcc
AR=arm-none-eabi-ar
AS=arm-none-eabi-as
CP=arm-none-eabi-objcopy
OD=arm-none-eabi-objdump
SE=arm-none-eabi-size
SF=st-flash

GDB = arm-none-eabi-gdb
STLINK = /usr/local/bin/st-flash
# Note, that it must be inclued in the $PATH variable; default in Linux is: $HOME/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin
STCUBEPROG = STM32_Programmer.sh

###################################################

INC_DIRS  = $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Inc
INC_DIRS += .
INC_DIRS += $(COREDIR)Drivers/BSP/CMWX1ZZABZ-0xx/
INC_DIRS += $(COREDIR)Drivers/BSP/Components/sx1276
INC_DIRS += $(COREDIR)Drivers/CMSIS/Device/ST/STM32L0xx/Include
INC_DIRS += $(COREDIR)Drivers/CMSIS/Documentation/Core/html
INC_DIRS += $(COREDIR)Drivers/CMSIS/Documentation/DSP/html
INC_DIRS += $(COREDIR)Drivers/CMSIS/Include
INC_DIRS += $(COREDIR)Drivers/CMSIS/RTOS/Template
INC_DIRS += $(COREDIR)Drivers/KETCube/core
INC_DIRS += $(COREDIR)Drivers/KETCube/modules
INC_DIRS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Inc
INC_DIRS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Inc/Legacy
INC_DIRS += $(COREDIR)KETCube/core
INC_DIRS += $(COREDIR)KETCube/modules/actuation
INC_DIRS += $(COREDIR)KETCube/modules/communication
INC_DIRS += $(COREDIR)KETCube/modules/sensing
INC_DIRS += $(COREDIR)Middlewares/Third_Party/Lora/Conf
INC_DIRS += $(COREDIR)Middlewares/Third_Party/Lora/Conf/Inc
INC_DIRS += $(COREDIR)Middlewares/Third_Party/Lora/Core
INC_DIRS += $(COREDIR)Middlewares/Third_Party/Lora/Crypto
INC_DIRS += $(COREDIR)Middlewares/Third_Party/Lora/Mac
INC_DIRS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/region
INC_DIRS += $(COREDIR)Middlewares/Third_Party/Lora/Phy
INC_DIRS += $(COREDIR)Middlewares/Third_Party/Lora/Utilities
INC_DIRS += $(COREDIR)Middlewares/Third_Party/Semtech/Utilities/
INC_DIRS += $(COREDIR)Projects/inc
INC_DIRS += $(COREDIR)Projects/inc/actuation
INC_DIRS += $(COREDIR)Projects/inc/communication
INC_DIRS += $(COREDIR)Projects/inc/sensing
INC_DIRS += $(COREDIR)Projects/inc/drivers

INCLUDE = $(addprefix -I, $(INC_DIRS))

###################################################

CFLAGS   = -Wall  -Wno-missing-braces -g -mthumb -mcpu=cortex-m0plus -fdiagnostics-color=auto $(OPTIMIZE)
CFLAGS  += -march=armv6-m -mlittle-endian -Wl,--gc-sections -TSTM32L072CZYx_FLASH.ld $(DEBUG)
CFLAGS  += -DSTM32L082xx -DUSE_B_L082Z_KETCube -DUSE_HAL_DRIVER -DREGION_EU868
CFLAGS  += -DKETCUBE_BUILD_ID="\"$(BUILD_ID)\"" -DKETCUBE_VERSION="\"$(VERSION)\""

ASFLAGS = -mcpu=cortex-m0plus -march=armv6-m -mthumb -mthumb-interwork

LDFLAGS  = -mcpu=cortex-m0 -march=armv6-m -TSTM32L072CZYx_FLASH.ld -Wl,-Map=$(TARGET).map,--gc-sections
LDFLAGS += -mthumb -mfloat-abi=soft -specs=nano.specs -specs=nosys.specs
LDFLAGS += -lc -lrdimon -u _printf_float

ALDFLAGS = 

###################################################

SRCS += $(COREDIR)Drivers/CMSIS/Device/ST/STM32L0xx/Source/Templates/system_stm32l0xx.c
SRCS += $(COREDIR)Drivers/BSP/CMWX1ZZABZ-0xx/mlm32l07x01.c
SRCS += $(COREDIR)Drivers/BSP/Components/sx1276/sx1276.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_spi.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_adc.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc_ex.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_gpio.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_dma.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr_ex.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rtc.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rtc_ex.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_adc_ex.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart_ex.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_tim_ex.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_tim.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_lptim.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2c.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2s.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash_ex.c
SRCS += $(COREDIR)Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_iwdg.c
SRCS += $(COREDIR)Projects/src/debug.c
SRCS += $(COREDIR)Projects/src/main.c
SRCS += $(COREDIR)Projects/src/mlm32l0xx_it.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Core/lora.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Core/lora-test.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/LoRaMac.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/LoRaMacCrypto.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/LoRaMacAdr.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/LoRaMacClassB.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/LoRaMacCommands.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/LoRaMacConfirmQueue.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/LoRaMacFCntHandler.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/LoRaMacParser.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/LoRaMacSerializer.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/region/Region.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/region/RegionAS923.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/region/RegionAU915.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/region/RegionCN470.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/region/RegionCN779.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/region/RegionCommon.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/region/RegionEU433.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/region/RegionEU868.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/region/RegionIN865.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/region/RegionKR920.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Mac/region/RegionUS915.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Utilities/systime.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Crypto/aes.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Crypto/cmac.c
SRCS += $(COREDIR)Middlewares/Third_Party/Lora/Crypto/soft-se.c
SRCS += $(COREDIR)Middlewares/Third_Party/Semtech/Utilities/timeServer.c
SRCS += $(COREDIR)Middlewares/Third_Party/Semtech/Utilities/utilities.c
SRCS += $(COREDIR)KETCube/core/ketCube_common.c
SRCS += $(COREDIR)KETCube/core/ketCube_cfg.c
SRCS += $(COREDIR)KETCube/core/ketCube_modules.c
SRCS += $(COREDIR)KETCube/core/ketCube_terminal.c
SRCS += $(COREDIR)KETCube/core/ketCube_terminal_common.c
SRCS += $(COREDIR)KETCube/core/ketCube_remote_terminal.c
SRCS += $(COREDIR)KETCube/core/ketCube_rxDisplay.c
SRCS += $(COREDIR)KETCube/core/ketCube_asyncTx.c
SRCS += $(COREDIR)KETCube/core/ketCube_txDisplay.c
SRCS += $(COREDIR)KETCube/core/ketCube_coreCfg.c
SRCS += $(COREDIR)KETCube/core/ketCube_resetMan.c
SRCS += $(COREDIR)KETCube/modules/communication/ketCube_lora.c
SRCS += $(COREDIR)KETCube/modules/communication/ketCube_lora_ext.c
SRCS += $(COREDIR)KETCube/modules/communication/ketCube_starNet.c
SRCS += $(COREDIR)KETCube/modules/sensing/ketCube_adc.c
SRCS += $(COREDIR)KETCube/modules/sensing/ketCube_hdc1080.c
SRCS += $(COREDIR)KETCube/modules/sensing/ketCube_batMeas.c
SRCS += $(COREDIR)KETCube/modules/sensing/ketCube_bmeX80.c
SRCS += $(COREDIR)KETCube/modules/sensing/ketCube_lis2hh12.c
SRCS += $(COREDIR)KETCube/modules/sensing/ketCube_ics43432.c
SRCS += $(COREDIR)Drivers/KETCube/core/ketCube_eeprom.c
SRCS += $(COREDIR)Drivers/KETCube/core/ketCube_mcu.c
SRCS += $(COREDIR)Drivers/KETCube/core/ketCube_uart.c
SRCS += $(COREDIR)Drivers/KETCube/core/ketCube_gpio.c
SRCS += $(COREDIR)Drivers/KETCube/core/ketCube_rtc.c
SRCS += $(COREDIR)Drivers/KETCube/core/ketCube_spi.c
SRCS += $(COREDIR)Drivers/KETCube/core/ketCube_radio.c
SRCS += $(COREDIR)Drivers/KETCube/modules/ketCube_ad.c
SRCS += $(COREDIR)Drivers/KETCube/modules/ketCube_i2c.c
SRCS += $(COREDIR)Drivers/KETCube/modules/ketCube_i2s.c
SRCS += $(COREDIR)Drivers/KETCube/modules/ketCube_timer.c

ASSRCSC  = $(COREDIR)Projects/src/hardFaultHandler.S

ASSRCS  = ./startup_stm32l082xx.s

###################################################

# Include project-specific sources here
include Makefile_proj

###################################################

OBJS  = $(addprefix $(OUTDIR),$(notdir $(SRCS:.c=.o)))
OBJS += $(addprefix $(OUTDIR),$(notdir $(ASSRCS:.s=.o)))
OBJS += $(addprefix $(OUTDIR),$(notdir $(ASSRCSC:.S=.o)))

OUTFILES = $(OUTDIR)$(TARGET).elf $(OUTDIR)$(TARGET).hex \
$(OUTDIR)$(TARGET).bin $(OUTDIR)lib$(TARGET).a
SRCPATHS = $(sort $(dir $(SRCS)))

VPATH = $(SRCPATHS)

###################################################

.PHONY: all size clean chipinfo flash erase debug sigdone

$(OUTDIR)%.o: %.c
	@echo Compiling $<
	@$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@
	
$(OUTDIR)%.o: %.S
	@echo Compiling $<
	@$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

$(OUTDIR)%.o: %.s
	@echo Compiling $<
	@$(AS) -c $(ASFLAGS) $(INCLUDE) $< -o $@

%.elf: $(OBJS)
	@echo Linking $@
	@$(LD) $(OBJS) $(LDFLAGS) -o $@

%.elf.dbg: CFLAGS  += -DDEBUG
%.elf.dbg: $(OBJS)
	@echo Linking $@
	@$(LD) $(OBJS) $(LDFLAGS) -o $@

%.hex: %.elf $(OBJS)
	@echo Creating $@
	@$(CP) -O ihex $< $@

%.bin: %.elf $(OBJS)
	@echo Creating $@
	@$(CP) -O binary -S $< $@

## Library linked in this way contains weak and non-weak symbols in the same time. This may lead to errorneous selection of weak symbl depending on the ordering in the library. If the library created in this way is used, it is recommended to link in the following way: "-Wl,--whole-archive -lKETCube -Wl,--no-whole-archive"; "-Wl,--gc-sections" will reduce binary size signifficantly, as this library contains number of unnecessary code
# %.a: $(OBJS)
# 	@echo Creating static library $@
# 	@$(AR) rcs $@ $(OBJS)

# Partial linking may be used as a workaround because objects contain number of weak and non-weak symbol pairs, whose leads to wrong symbol selection during the link process, when symbol ordering is weak, non-weak (in libraries, firs-hit symbol search strategy is used)
%.a: $(OBJS)
	@echo Creating static library $@
	@$(LD) $(OBJS) $(ALDFLAGS) -r -o $(@:.a=.o)
	@$(AR) rcs $@ $(@:.a=.o)

all: $(OBJS) $(OUTFILES)
	$(MAKE) size

size:
	@$(SE) $(OUTDIR)$(TARGET).elf
	$(MAKE) sigdone

clean:
	rm -f $(OUTDIR)*.o $(OUTDIR)$(TARGET).elf $(OUTDIR)lib$(TARGET).a $(OUTDIR)$(TARGET).hex $(OUTDIR)$(TARGET).bin
	$(MAKE) sigdone

chipinfo:
	st-info --chipid

stflash: $(OUTDIR)$(TARGET).bin
	-$(STLINK) write $(OUTDIR)$(TARGET).bin 0x8000000
	$(STLINK) reset
	$(MAKE) sigdone

erase:
	$(STLINK) erase
	$(MAKE) sigdone

ocd:
	@echo "If it is not possible to connect target, consider erasing target!"
	openocd -f ketcube-openocd.cfg

flash: $(OUTDIR)$(TARGET).elf
	$(GDB) -x gdb_flash $(OUTDIR)$(TARGET).elf
	$(MAKE) sigdone

eraseCubeProg:
	$(STCUBEPROG) -c port=SWD mode=UR -e all -q
	$(MAKE) sigdone
	
flashCubeProg: $(OUTDIR)$(TARGET).bin
	$(STCUBEPROG) -c port=SWD mode=UR -e all -q -d $(OUTDIR)$(TARGET).bin 0x8000000
	$(STCUBEPROG) -c port=SWD mode=UR -ob bfb2=0x0 #make sure, that BFB2 bit is 0
	$(MAKE) sigdone
	
# This target is for commandline-based debugging
# The "ocd" target is the prerequisity -- run it in separate terminal
debug: $(OUTDIR)$(TARGET).elf.dbg
	$(GDB) -x gdb_startup $(OUTDIR)$(TARGET).elf.dbg

# This target is to allow interfacing with GDB clients - e.g. KDevelop
# The "ocd" target is the prerequisity -- run it in separate terminal
debugMI2: $(OUTDIR)$(TARGET).elf.dbg
	$(GDB) --interpreter=mi2 -x gdb_startup $(OUTDIR)$(TARGET).elf.dbg

# Signaling "DONE" on most Linux systems
sigdone:
	@echo "ALL DONE"
	@paplay /usr/share/sounds/freedesktop/stereo/complete.oga
	
# Include project-specific targets
include Makefile_proj_targets

