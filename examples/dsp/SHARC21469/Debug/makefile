################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := ${ComSpec}

-include ../makefile.init

RM := clean

# All of the sources participating in the build are defined here
-include sources.mk
-include src/user/subdir.mk
-include src/OS_code/loader/si/subdir.mk
-include src/OS_code/lib/libc/printf/subdir.mk
-include src/OS_code/lib/libc/ecc/subdir.mk
-include src/OS_code/lib/libc/crc/subdir.mk
-include src/OS_code/djyos/wdt/subdir.mk
-include src/OS_code/djyos/stdin/touch/subdir.mk
-include src/OS_code/djyos/stdin/keypad/subdir.mk
-include src/OS_code/djyos/stdin/char_term/subdir.mk
-include src/OS_code/djyos/stdin/subdir.mk
-include src/OS_code/djyos/shell/subdir.mk
-include src/OS_code/djyos/rsc/subdir.mk
-include src/OS_code/djyos/pipe/subdir.mk
-include src/OS_code/djyos/msg/subdir.mk
-include src/OS_code/djyos/memory/subdir.mk
-include src/OS_code/djyos/lock/subdir.mk
-include src/OS_code/djyos/int/subdir.mk
-include src/OS_code/djyos/gui/window/subdir.mk
-include src/OS_code/djyos/gui/lib/subdir.mk
-include src/OS_code/djyos/gui/gk_draw/subdir.mk
-include src/OS_code/djyos/gui/font/subdir.mk
-include src/OS_code/djyos/gui/encoding/subdir.mk
-include src/OS_code/djyos/gui/display/subdir.mk
-include src/OS_code/djyos/driver/subdir.mk
-include src/OS_code/djyos/djyfs/flashfile/subdir.mk
-include src/OS_code/djyos/djyfs/easynorflash/subdir.mk
-include src/OS_code/djyos/djyfs/subdir.mk
-include src/OS_code/djyos/core/subdir.mk
-include src/OS_code/djyos/clock/subdir.mk
-include src/OS_code/djyos/buffer/subdir.mk
-include src/OS_code/bsp/arch/21469/startup/subdir.mk
-include src/OS_code/bsp/arch/21469/core/subdir.mk
-include config/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(DSP_DEPS)),)
-include $(DSP_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LINKER_LOG_OUTPUTS += \
linker_log.xml \


# All Target
all: SHARC21469.dxe

# Tool invocations
SHARC21469.dxe: $(OBJS) $(LDF_SRCS)
	@echo 'Building target: $@'
	@echo 'Invoking: CrossCore SHARC Linker'
	cc21k.exe -proc ADSP-21469 -si-revision any -TD:/djysdk/djyos_dsp_project/examples/SHARC21469/config/Debug.ldf -no-mem -LD:/djysdk/djyos_dsp_project/examples/SHARC21469/Debug/system/startup_ldf -LD:/djysdk/djyos_dsp_project/examples/SHARC21469/Debug/src/OS_code/bsp/arch/21469/startup -LD:/djysdk/djyos_dsp_project/examples/SHARC21469/Debug/src/OS_code/bsp/arch/21469/core -flags-link -MDDEBUG -flags-link -i,D:\djysdk\djyos_dsp_project\examples\SHARC21469\system -add-debug-libpaths -o SHARC21469.dxe $(OBJS) $(LDF_SRCS)  -flags-link -od,.\.
	@echo 'Finished building target: $@'
	@echo ' '

linker_log.xml: SHARC21469.dxe $(OBJS) $(LDF_SRCS)

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(CC_DEPS)$(DSP_DEPS)$(DXE)$(CPP_DEPS)$(LINKER_LOG_OUTPUTS)$(S_DEPS)$(CXX_DEPS) SHARC21469.dxe
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
