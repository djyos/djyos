################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/djysdk/djysrc/djyos/driver/pci/cmd_pci.c \
D:/djysdk/djysrc/djyos/driver/pci/pci.c \
D:/djysdk/djysrc/djyos/driver/pci/pci_auto.c \
D:/djysdk/djysrc/djyos/driver/pci/pci_indirect.c 

OBJS += \
./src/OS_code/djyos/driver/pci/cmd_pci.doj \
./src/OS_code/djyos/driver/pci/pci.doj \
./src/OS_code/djyos/driver/pci/pci_auto.doj \
./src/OS_code/djyos/driver/pci/pci_indirect.doj 

C_DEPS += \
./src/OS_code/djyos/driver/pci/cmd_pci.d \
./src/OS_code/djyos/driver/pci/pci.d \
./src/OS_code/djyos/driver/pci/pci_auto.d \
./src/OS_code/djyos/driver/pci/pci_indirect.d 


# Each subdirectory must supply rules for building sources it contributes
src/OS_code/djyos/driver/pci/cmd_pci.doj: D:/djysdk/djysrc/djyos/driver/pci/cmd_pci.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/djyos/driver/pci/pci.doj: D:/djysdk/djysrc/djyos/driver/pci/pci.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/djyos/driver/pci/pci_auto.doj: D:/djysdk/djysrc/djyos/driver/pci/pci_auto.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/djyos/driver/pci/pci_indirect.doj: D:/djysdk/djysrc/djyos/driver/pci/pci_indirect.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


