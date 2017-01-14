################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include board/wb7293/src/critical/subdir.mk
-include board/wb7293/src/OS_prjcfg/net/subdir.mk
-include board/wb7293/src/OS_prjcfg/subdir.mk
-include board/wb7293/src/subdir.mk
-include board/myboard/src/critical/subdir.mk
-include board/myboard/src/OS_prjcfg/network/subdir.mk
-include board/myboard/src/OS_prjcfg/subdir.mk
-include board/myboard/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
iboot.bin \

SECONDARY_LIST += \
iboot.lst \

SECONDARY_SIZE += \
iboot.siz \


# All Target
all: iboot.elf secondary-outputs

# Tool invocations
iboot.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m7 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv5-d16 -O0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -Wall  -g -T "..\board\wb7293\lds\iboot.lds" -nostartfiles -nodefaultlibs -nostdlib -Xlinker --gc-sections -L"..\lib" -Wl,-Map,"iboot.map" -o "iboot.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

iboot.bin: iboot.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O binary "iboot.elf"  "iboot.bin"
	@echo 'Finished building: $@'
	@echo ' '

iboot.lst: iboot.elf
	@echo 'Invoking: Cross ARM GNU Create Listing'
	arm-none-eabi-objdump --source --all-headers --demangle --disassemble --file-headers --line-numbers --syms --wide "iboot.elf" > "iboot.lst"
	@echo 'Finished building: $@'
	@echo ' '

iboot.siz: iboot.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "iboot.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) iboot.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
