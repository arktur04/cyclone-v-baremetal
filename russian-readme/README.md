# cyclone-v-baremetal
An example of a simple bare-metal application for the ARM Cortex A9 processor (Altera Cyclone V HPS)

### Сборка bare-metal приложения для процессора ARM на плате SoCkit

Все действия происходят в ОС Debian Jessie. Можно использовать Windows с соответствующими поправками.

### 1. Подготовка

1.1. Устанавливаем бесплатную DS-5 Altera Edition и полную DS-5 (при установке запускаем всё через sudo, пути установки вводим вручную). Пусть DS-5 Altera Edition установлена в папку /home/user/intelFPGA, полная DS-5 в папку /home/user/DS-5.

### 2. Запуск IDE

2.1. Запускаем сначала скрипт /home/user/intelFPGA/16.1/embedded/embedded_command_shell.sh, потом в том же окне терминала переходим в папку /home/user/DS-5/bin и запускаем IDE командой $ ./eclipse &.
Запуск скрипта нужен для установки переменных окружения. Можно устанавливать переменные окружения при запуске системы, для этого нужно вписать в файл /home/user/.profile:
export QUARTUS_ROOTDIR=/home/user/intelFPGA/16.1/qprogrammer
export PATH=$PATH:/usr/local/gcc-arm-none-eabi-5_4-2016q3/bin/:/home/user/intelFPGA/16.1/embedded/host_tools/mentor/gnu/arm/baremetal/bin/

? Возможно, понадобятся и другие переменные, например QSYS_ROOTDIR.

Также будет нужно запускать Quartus Prime и программатор Quartus. Quartus Prime: /home/user/intelFPGA/16.1/quartus/bin/quartus, программатор: /home/user/intelFPGA/16.1/qprogrammer/bin/quartus_pgmw.

### 3. Подключение SoCkit

3.1. Подключаем SoCkit к компьютеру к разъёмам USB Blaster (JTAG) и USB to UART.

3.2. Включаем плату и вводим команду:
```sh 
$ ls /dev/bus/usb/001
 001  002  003
```
То есть номер интерфейса JTAG - 003. Устанавливаем ему права записи:
```sh
 $ sudo chmod 666 /dev/bus/usb/001/003
```
Это нужно делать после каждого включения платы.

3.3. Настройка терминала TTY. В качестве терминала можно использовать, например, команду
```sh
screen /dev/ttyUSB0 115200
```
### 4. Сборка Preloader (u-boot-spl)

4.1. Скачать sockit_ghrd_16.0.tar.gz и распаковать в отдельную папку (например, /home/user/ghrd-16/sockit_ghrd).

4.2. Запускаем quartus, открываем файл /home/user/ghrd-16/sockit_ghrd/soc_system.qpf.

4.3. Запускаем Qsys (Tools/Qsys). Открываем файл /home/user/ghrd-16/sockit_ghrd/soc_system.qsys. Выполняем команду Generate/Generate HDL. Выходим из Qsys.

4.4. В quartus запускаем Assembler.

4.5. Запускаем bsp-editor: /home/user/intelFPGA/16.1/embedded/host_tools/altera/preloadergen/bsp-editor. Создаём новый проект: File/New HPS BSP. В поле Preloader setting directory указываем /home/user/ghrd-16/sockit_ghrd/hps_isw_handoff/soc_system_hps_0.

4.6. Выполняем настройки preloader. На вкладке Advanced: Watchdog_enable можно выключить, Semihosting можно включить, если мы хотим видеть отладочные сообщения через jtag.После настройки нажимаем Generate и выходим.

4.7. Можно изменить сообщения, выводимые на экран при загрузке. Например, в файле software/spl_bsp/uboot-socfpga/board/altera/socfpga/socfpga_cyclone5.c можно заменить надпись "BOARD : Altera SOCFPGA Cyclone V Board" на другую:
```c
/*
 * Print Board information
 */
int checkboard(void)
{
#ifdef CONFIG_SOCFPGA_VIRTUAL_TARGET
 puts("BOARD : Altera VTDEV5XS1 Virtual Board\n");
#else
 //puts("BOARD : Altera SOCFPGA Cyclone V Board\n");
 puts("ARM preloader build by 32-bit.me\n");
#endif
 return 0;
}
```

4.8. Переменная окружения SOCEDS_DEST_ROOT должна быть установлена в /home/user/intelFPGA/16.1/embedded. Если нет, то устанавливаем:
```sh
export SOCEDS_DEST_ROOT=/home/user/intelFPGA/16.1/embedded
```
Переходим в папку /home/user/ghrd-16/sockit_ghrd/software/spl_bsp и запускаем make. Make может завершиться ошибкой "make: mkpimage: Команда не найдена", но должен быть собран файл u-boot-spl (/home/user/ghrd-16/sockit_ghrd/software/spl_bsp/uboot-socfpga/spl/u-boot-spl).
4.9. Проверка preloader-а

4.9.1. Для запуска preloader загружать конфигурацию fpga (файл .sof) не нужно. Запускаем DS-5. Открываем Run/Debug configurations.

4.9.2.В поле Connection должен быть установлен CV SoCKit 1-1. Если нет, нажимаем на Browse и устанавливаем. Если возникает ошибка, выполняем пп. 3.1-3.2.

4.9.3. На вкладке Files в поле Application on host to download указываем файл u-boot-spl: /home/user/ghrd-16/sockit_ghrd/software/spl_bsp/uboot-socfpga/spl/u-boot-spl. Включаем галочку Load symbols. Поле Files оставляем пустым.

4.9.4. На вкладке Debugger должен быть отключен запуск скрипта отладки.

4.9.5. Запускаем терминал (screen) и нажимаем на кнопку Debug. Происходит загрузка preloader в память, и отладчик готов к запуску. Нажимаем кнопку Continue (F8), выводится сообщение:
```
U-Boot SPL 2013.01.01 (Jun 24 2017 - 19:49:33)
ARM preloader build by 32-bit.me
CLOCK: EOSC1 clock 25000 KHz
CLOCK: EOSC2 clock 25000 KHz
CLOCK: F2S_SDR_REF clock 0 KHz
CLOCK: F2S_PER_REF clock 0 KHz
CLOCK: MPU clock 925 MHz
CLOCK: DDR clock 400 MHz
CLOCK: UART clock 100000 KHz
CLOCK: MMC clock 50000 KHz
CLOCK: QSPI clock 370000 KHz
RESET: COLD
SDRAM: Initializing MMR registers
SDRAM: Calibrating PHY
SEQ.C: Preparing to start memory calibration
SEQ.C: CALIBRATION PASSED
SDRAM: 1024 MiB
ALTERA DWMMC: 0
Card did not respond to voltage select!
spl: mmc init failed: err - -17
### ERROR ### Please RESET the board ###
```
Это означает, что preloader успешно запускается.

4.9.6. После сеанса отладки нажать на кнопку Disconnect From Target и Remove Connection.

### 5. Сборка примера Bare-Metal

5.1. Скачиваем проект ArrowSocKit_BareMetal_GNU.zip. Устанавливаем переменные окружения (п. 2.1), запускаем DS-5, импортируем проект из архива (File/Import/Existing Project into Workspace, Select archile file). Распакованный проект должен появиться в папке /home/user/DS-5-Workspace/BareMetalBoot-GNU.

5.2. Можно собрать проект командой make, однако он не будет собираться в исходном виде.

5.3. Копируем папку /home/user/intelFPGA/16.1/embedded/ip/altera/hps/altera_hps/hwlib в /home/user/DS-5-Workspace/. В ней находятся папки src и include, в каждой из них есть папки soc_cv_av и soc_a10. Папки soc_a10 можно удалить. Также в папку проекта (/home/user/DS-5-Workspace/BareMetalBoot-GNU) нужно скопировать файлы libcs3.a, libcs3arm.a, libcs3unhosted.a из /home/user/intelFPGA/16.1/embedded/host_tools/mentor/gnu/arm/baremetal/arm-altera-eabi/lib. Скопировать arm-names.inc из /home/user/intelFPGA/16.1/embedded/host_tools/mentor/gnu/arm/baremetal/arm-altera-eabi/lib/cortex-a9/ в /home/user/DS-5-Workspace/BareMetalBoot-GNU/

5.4. В Makefile меняем CROSS_COMPILE := arm-none-eabi- на CROSS_COMPILE := arm-altera-eabi-. Также меняем путь к hwlib: HWLIBS_ROOT := /home/user/DS-5-Workspace/hwlib

5.5. Теперь запускаем make и читаем все сообщения об ошибках. Если make не находит какой-либо символ, то нужно найти его в папке hwlib/src/soc_cv_av и в hwlib/include/soc_cv_av и скопировать в hwlib/src и в hwlib/include соответственно. Готовый проект можно скачать в этом репозитории. После того, как все файлы будут найдены, make будет завершаться с ошибкой "mkimage: not found", но это неважно, так как нам нужен только файл test.axf.

5.6. Запуск приложения

5.6.1. Для запуска приложения нужно загрузить конфигурацию fpga, которая была сгенерирована при сборке preloader (файл .sof). Запускаем программатор (п.2.1). Справа от кнопки Hardware Setup должна быть надпись: CV SoCKit [1-1]. Если её нет, нужно установить права записи на порт jtag (см. выше) и нажать на Hardware Setup.

5.6.2. Нажать на Auto Detect, можно указать произвольный пункт в окне. Появится конфигурация из двух устройств: указанного (например, 5CSEBA6) и SOCVHPS. Удаляем первое устройство (выделить и нажать Delete). Нажимаем кнопку Add File и открываем файл конфигурации (/home/user/ghrd-16/sockit_ghrd/output_files/soc_system.sof). Справа от SOCVHPS появляется новое устройство. Его нужно переместить мышью на первое место в цепочке. После этого нажимаем Start и прошиваем устройство.

5.6.3. В DS-5 открываем Debug Configuration, вкладку Files. В строке Application on host to download указываем файл u-boot-spl, который мы скомпилировали ранее. Галочку Load Symbols убираем. В строке Files указываем файл test.axf. На вкладке Debugger ставим галку на пункте Run target initialization debugger script и указываем скрипт debug-unhosted.ds в папке проекта. В самом файле редактируем путь к preloader:
```
# Load the SPL preloader into memory.
#
loadfile "/home/user/ghrd-16/sockit_ghrd/software/spl_bsp/uboot-socfpga/spl/u-boot-spl" 0x0
```
Запускаем терминал TTY (screen) и нажимаем Debug, запускается сначала preloader, затем загружается приложение и дебаггер устанавливается на начало программы. Нажимаем кнопку запуска (Continue). В окне терминала должен быть вывод preloader (см. выше), а затем следующее:
```
Disabled interrupts because preloader enabled ECC interrupts.
Global Timer value at startup = 0x000000005A10B609
CPU frequency is 925000000 hertz.
DDR frequency is 400000000 hertz.
MPU peripheral frequency is 231250000 hertz.
MB ZERO remapped to SDRAM.
L3 interconnect remapped as inaccessible and SDRAM.
Interrupt controller system API initialized.
MMU initialized and enabled.
L1 and L2 cache system initialized and enabled.
GIC interrupts enabled.
PIT started.
PIT interrupt registered and enabled.
GPIOs initialized.
UART0 initialized.
L3 interconnect remapped as accessible and SDRAM.
Bridges initialized.
Boot completed.

Boot milestones:
New buffer !ESTONE :       GT TICKCOUNT :  INCREMENTAL :   CUMULATIVE
        initial GT : 0x000000005A10B609 :      0.000us :      0.000us
 frequency capture : 0x000000005A119D58 :    256.065us :    256.065us
 initial remapping : 0x000000005A11C936 :     48.562us :    304.627us
interrupt sys init : 0x000000005A139FED :    521.094us :    825.721us
          mmu init : 0x000000005A62D774 :  22449.985us :  23275.706us
        cache init : 0x000000005A65CF96 :    841.488us :  24117.194us
interrupts enabled : 0x000000005A65D0D7 :      1.388us :  24118.582us
         pit start : 0x000000005A65DD2E :     13.661us :  24132.242us
       pit enabled : 0x000000005A65DF67 :      2.461us :  24134.703us
         gpio init : 0x000000005A65FC12 :     31.736us :  24166.439us
        uart0 init : 0x000000005A66084D :     13.539us :  24179.978us
       fpga config : 0x0000000000000000 : 79769704095969360.000us : 79769704095993552.000us
    last remapping : 0x000000005A660981 : 6558428.303us :  24181.310us
       bridge init : 0x000000005A6611B6 :      9.085us :  24190.396us
          complete : 0x000000005A661222 :      0.467us :  24190.863us
Hello world, 32bit-me!
```
