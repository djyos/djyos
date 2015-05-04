################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/djysdk/djysrc/bsp/cpu_peripheral/Sharc21469/pci/fsl_dma.c \
D:/djysdk/djysrc/bsp/cpu_peripheral/Sharc21469/pci/fsl_law.c \
D:/djysdk/djysrc/bsp/cpu_peripheral/Sharc21469/pci/fsl_pci_init.c \
D:/djysdk/djysrc/bsp/cpu_peripheral/Sharc21469/pci/law.c \
D:/djysdk/djysrc/bsp/cpu_peripheral/Sharc21469/pci/p1020_serdes.c 

OBJS += \
./src/OS_code/bsp/cpu_peripheral/pci/fsl_dma.doj \
./src/OS_code/bsp/cpu_peripheral/pci/fsl_law.doj \
./src/OS_code/bsp/cpu_peripheral/pci/fsl_pci_init.doj \
./src/OS_code/bsp/cpu_peripheral/pci/law.doj \
./src/OS_code/bsp/cpu_peripheral/pci/p1020_serdes.doj 

C_DEPS += \
./src/OS_code/bsp/cpu_peripheral/pci/fsl_dma.d \
./src/OS_code/bsp/cpu_peripheral/pci/fsl_law.d \
./src/OS_code/bsp/cpu_peripheral/pci/fsl_pci_init.d \
./src/OS_code/bsp/cpu_peripheral/pci/law.d \
./src/OS_code/bsp/cpu_peripheral/pci/p1020_serdes.d 


# Each subdirectory must supply rules for building sources it contributes
src/OS_code/bsp/cpu_peripheral/pci/fsl_dma.doj: D:/djysdk/djysrc/bsp/cpu_peripheral/Sharc21469/pci/fsl_dma.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/bsp/cpu_peripheral/pci/fsl_law.doj: D:/djysdk/djysrc/bsp/cpu_peripheral/Sharc21469/pci/fsl_law.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/bsp/cpu_peripheral/pci/fsl_pci_init.doj: D:/djysdk/djysrc/bsp/cpu_peripheral/Sharc21469/pci/fsl_pci_init.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/bsp/cpu_peripheral/pci/law.doj: D:/djysdk/djysrc/bsp/cpu_peripheral/Sharc21469/pci/law.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/bsp/cpu_peripheral/pci/p1020_serdes.doj: D:/djysdk/djysrc/bsp/cpu_peripheral/Sharc21469/pci/p1020_serdes.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


