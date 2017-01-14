################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/user/critical/subdir.mk
-include src/user/subdir.mk
-include src/OS_prjcfg/network/subdir.mk
-include src/OS_prjcfg/subdir.mk
-include src/OS_code/test/gdd_test/subdir.mk
-include src/OS_code/bsp/boarddrv/STMPE811/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
qh_1.bin \

SECONDARY_LIST += \
qh_1.lst \

SECONDARY_SIZE += \
qh_1.siz \


# All Target
all: qh_1.elf secondary-outputs

# Tool invocations
qh_1.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O2 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -Wall  -g -T "..\config\release.lds" -nostartfiles -Xlinker --gc-sections -L"..\libc-release" -L"..\libOS-release" -L"..\Stm32F10xFirmWare-release" -Wl,-Map,"qh_1.map" -Xlinker --cref -o "qh_1.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

qh_1.bin: qh_1.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O binary "qh_1.elf"  "qh_1.bin"
	@echo 'Finished building: $@'
	@echo ' '

qh_1.lst: qh_1.elf
	@echo 'Invoking: Cross ARM GNU Create Listing'
	arm-none-eabi-objdump --source --all-headers --demangle --line-numbers --wide "qh_1.elf" > "qh_1.lst"
	@echo 'Finished building: $@'
	@echo ' '

qh_1.siz: qh_1.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "qh_1.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(S_UPPER_DEPS) qh_1.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
