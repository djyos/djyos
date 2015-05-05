powerpc-eabispe-objdump -h -S %1.elf > %1.lst
powerpc-eabispe-objcopy -O binary %1.elf %1.bin
powerpc-eabispe-size --format=berkeley %1.elf
