################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/djysdk/djysrc/loader/si/loader.c \
D:/djysdk/djysrc/loader/si/pre_loader.c 

OBJS += \
./src/OS_code/loader/si/loader.doj \
./src/OS_code/loader/si/pre_loader.doj 

C_DEPS += \
./src/OS_code/loader/si/loader.d \
./src/OS_code/loader/si/pre_loader.d 


# Each subdirectory must supply rules for building sources it contributes
src/OS_code/loader/si/loader.doj: D:/djysdk/djysrc/loader/si/loader.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -file-attr_DEBUG -I"d:\djysdk\djysrc\djyos\include" -I"d:\djysdk\djysrc\djyos\gui\include" -I"d:\djysdk\djysrc\djyos\djyip\include" -I"d:\djysdk\djysrc\djyos\driver\pci" -I"d:\djysdk\djysrc\djyos\djyfs" -I"d:\djysdk\djysrc\lib\include" -I"d:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"d:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"d:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"d:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/loader/si/pre_loader.doj: D:/djysdk/djysrc/loader/si/pre_loader.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -file-attr_DEBUG -I"d:\djysdk\djysrc\djyos\include" -I"d:\djysdk\djysrc\djyos\gui\include" -I"d:\djysdk\djysrc\djyos\djyip\include" -I"d:\djysdk\djysrc\djyos\driver\pci" -I"d:\djysdk\djysrc\djyos\djyfs" -I"d:\djysdk\djysrc\lib\include" -I"d:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"d:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"d:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"d:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


