

 

Attached you have the importable project. Here are some instructions about it, let me know if you need more, and we can talk about it.

 

1. Created project that does the following:

Prints a message each second
Kicks the watchdog each time
 

The project was created with these steps:

Started by importing the HWLIBS sample project from SoCEDS
Updated the makefile like this:
Removed all FPGA-image related commands
Removed all commands that copied the HWLIBS source code to current project folder
Add assembler command rule
Replaced the linker command file ‘altera-socfpga-hosted.ld’ to ‘altera-socfpga-unhosted.ld’
Added objcopy command to create the bin file
Other small changes – you can compare the makefile with the original one to see all changes
Renamed linker command file ‘altera-socfpga-hosted.ld’ to ‘altera-socfpga-unhosted.ld’ and made the following changes
Moved the memory start location from 0x02000000 to 0x02100000 to make some room before the ‘start’ of RAM so that the mkimage header can be loaded by u-boot
Replaced ‘-lcs3hosted‘ with -lcs3unhosted’
Replaced ‘PROVIDE(__cs3_reset = __cs3_reset_generic);’ with ‘PROVIDE(__cs3_reset = __reset);’
Wrote file ‘utility.c’ and ‘utility.h’ with source code for functions to
Write over UART (simple polling, assumes UART was configured properly by the Preloader)
Delay
Kick the watchdog
Renamed the file ‘debug-hosted.ds ‘ to ‘debug-unhosted.ds’ and removed setting semihosting enabled – to allow debugging with the debugger
 

2. Created Preloader based on GHRD, that loads the next stage boot from the QSPI. This also create the mkimage executable.

 

3. Configured the board to boot from QSPI

 

4. Wrote the Preloader to QSPI:

quartus_hps -c 1 -o PV preloader-mkpimage.bin

 

5. Added mkimage header on top of binary executable: 

mkimage.exe -A arm -T standalone -C none -a 0x02100000 -e 0 -n "baremetal image" -d test.bin test-img.bin

 

6. Wrote the application image to QSPI:

quartus_hps -c 1 -o PV -a 0x60000 test-img.bin

 

7. Booted the board by pressing cold reset button:

U-Boot SPL 2012.10 (Jun 03 2013 - 09:11:38)

SDRAM : Initializing MMR registers

SDRAM : Calibrationg PHY

SEQ.C: Preparing to start memory calibration

SEQ.C: CALIBRATION PASSED

SF: Detected N25Q00 with page size 65536, total: 134217728

Hello World: 0!

Hello World: 1!

Hello World: 2!

Hello World: 3!

Hello World: 4!

Hello World: 5!

 

