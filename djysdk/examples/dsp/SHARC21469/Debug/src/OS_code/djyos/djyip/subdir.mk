################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/djysdk/djysrc/djyos/djyip/arp.c \
D:/djysdk/djysrc/djyos/djyip/easyftp.c \
D:/djysdk/djysrc/djyos/djyip/enet.c \
D:/djysdk/djysrc/djyos/djyip/enet_cmd.c \
D:/djysdk/djysrc/djyos/djyip/ethernet.c \
D:/djysdk/djysrc/djyos/djyip/icmp.c \
D:/djysdk/djysrc/djyos/djyip/ip.c \
D:/djysdk/djysrc/djyos/djyip/ping.c \
D:/djysdk/djysrc/djyos/djyip/socket.c \
D:/djysdk/djysrc/djyos/djyip/socket_helper.c \
D:/djysdk/djysrc/djyos/djyip/tcp.c \
D:/djysdk/djysrc/djyos/djyip/udp.c 

OBJS += \
./src/OS_code/djyos/djyip/arp.doj \
./src/OS_code/djyos/djyip/easyftp.doj \
./src/OS_code/djyos/djyip/enet.doj \
./src/OS_code/djyos/djyip/enet_cmd.doj \
./src/OS_code/djyos/djyip/ethernet.doj \
./src/OS_code/djyos/djyip/icmp.doj \
./src/OS_code/djyos/djyip/ip.doj \
./src/OS_code/djyos/djyip/ping.doj \
./src/OS_code/djyos/djyip/socket.doj \
./src/OS_code/djyos/djyip/socket_helper.doj \
./src/OS_code/djyos/djyip/tcp.doj \
./src/OS_code/djyos/djyip/udp.doj 

C_DEPS += \
./src/OS_code/djyos/djyip/arp.d \
./src/OS_code/djyos/djyip/easyftp.d \
./src/OS_code/djyos/djyip/enet.d \
./src/OS_code/djyos/djyip/enet_cmd.d \
./src/OS_code/djyos/djyip/ethernet.d \
./src/OS_code/djyos/djyip/icmp.d \
./src/OS_code/djyos/djyip/ip.d \
./src/OS_code/djyos/djyip/ping.d \
./src/OS_code/djyos/djyip/socket.d \
./src/OS_code/djyos/djyip/socket_helper.d \
./src/OS_code/djyos/djyip/tcp.d \
./src/OS_code/djyos/djyip/udp.d 


# Each subdirectory must supply rules for building sources it contributes
src/OS_code/djyos/djyip/arp.doj: D:/djysdk/djysrc/djyos/djyip/arp.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/djyos/djyip/easyftp.doj: D:/djysdk/djysrc/djyos/djyip/easyftp.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/djyos/djyip/enet.doj: D:/djysdk/djysrc/djyos/djyip/enet.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/djyos/djyip/enet_cmd.doj: D:/djysdk/djysrc/djyos/djyip/enet_cmd.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/djyos/djyip/ethernet.doj: D:/djysdk/djysrc/djyos/djyip/ethernet.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/djyos/djyip/icmp.doj: D:/djysdk/djysrc/djyos/djyip/icmp.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/djyos/djyip/ip.doj: D:/djysdk/djysrc/djyos/djyip/ip.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/djyos/djyip/ping.doj: D:/djysdk/djysrc/djyos/djyip/ping.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/djyos/djyip/socket.doj: D:/djysdk/djysrc/djyos/djyip/socket.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/djyos/djyip/socket_helper.doj: D:/djysdk/djysrc/djyos/djyip/socket_helper.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/djyos/djyip/tcp.doj: D:/djysdk/djysrc/djyos/djyip/tcp.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OS_code/djyos/djyip/udp.doj: D:/djysdk/djysrc/djyos/djyip/udp.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k.exe -c -file-attr ProjectName="SHARC21469" -proc ADSP-21469 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -I"D:\djysdk\djyos_dsp_project\examples\SHARC21469\system" -I"D:/djysdk/djyos_dsp_project/examples/SHARC21469/src/OS_prjcfg/include" -I"D:\djysdk\djysrc\djyos\include" -I"D:\djysdk\djysrc\djyos\gui\include" -I"D:\djysdk\djysrc\djyos\djyip\include" -I"D:\djysdk\djysrc\djyos\driver\pci" -I"D:\djysdk\djysrc\lib\include" -I"D:\djysdk\djysrc\djyos\djyfs" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\include" -I"D:\djysdk\djysrc\bsp\arch\dsp\sharc\21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\include" -I"D:\djysdk\djysrc\bsp\cpu_peripheral\Sharc21469\pci" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


