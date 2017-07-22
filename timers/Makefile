#
# Copyright Altera 2013
# All Rights Reserved.
#


SOCEDS_ROOT ?= $(SOCEDS_DEST_ROOT)
HWLIBS_ROOT := /home/user/DS-5-Workspace/hwlib

OBJ := main.o startup.o

CFLAGS := -g -O0 -mfloat-abi=soft -march=armv7-a -mtune=cortex-a9 -mcpu=cortex-a9 -Wall -Wstrict-prototypes -std=c99 -fdata-sections -ffunction-sections -I/home/user/DS-5-Workspace/hwlib/include


ASFLAGS :=  -march=armv7-a -mcpu=cortex-a9

CFLAGS += \
	-DSUPPRESS_BOOT_MESSAGES=0 \
	-DSUPPRESS_SEMIHOSTING=1
LINKER_SCRIPT := altera-socfpga-unhosted.ld

CROSS_COMPILE := arm-altera-eabi-
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)g++
NM := $(CROSS_COMPILE)nm
OD := $(CROSS_COMPILE)objdump
OC := $(CROSS_COMPILE)objcopy
AS := $(CROSS_COMPILE)as
# this will cahnge for system to system.  just needed to make the mkimage wrapper.  If you are using it after uboot it is not needed. 
MKI := $(SOCEDS_ROOT)/examples/hardware/cv_soc_devkit_ghrd/software/spl_bsp/uboot-socfpga/tools/mkimage 

RM := rm -rf
CP := cp -f


ELF := test.axf
BIN := test.bin
SPL := u-boot-spl.axf
#SPL := u-boot-spl

HWLIBS_SRCS := $(notdir $(wildcard $(HWLIBS_ROOT)/src/hwmgr/*.c))
HWLIBS_OBJDIR := hwlibs_obj
HWLIBS_OBJS := $(patsubst %.c, %.o, $(addprefix $(HWLIBS_OBJDIR)/, $(HWLIBS_SRCS)))
HWLIBS_LIB := libhwlibs.a

.PHONY: all
all: $(ELF) $(SPL)

.PHONY:
clean:
	$(RM) $(ELF) $(OBJ) *.objdump *.map *.rbf  $(SPL) $(HWLIBS_OBJDIR) $(HWLIBS_LIB)

.PHONY: hwlibs_lib
hwlibs_lib: $(HWLIBS_LIB)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s Makefile
	$(AS) $(ASFLAGS) -c $< -o $@

$(SPL): $(SOCEDS_ROOT)/examples/hardware/cv_soc_devkit_ghrd/software/preloader/uboot-socfpga/spl/u-boot-spl
	$(CP) $< $@
	$(OD) -d $@ > $@.objdump
	
$(ELF): $(OBJ) $(LINKER_SCRIPT) $(HWLIBS_LIB)
	$(LD) -T$(LINKER_SCRIPT)  $(OBJ) -lhwlibs -o $@
	$(OD) -d $@ > $@.objdump
	$(NM) $@ > $@.map
	$(OC) -O binary $(ELF) $(BIN)
	$(MKI) -A arm -O u-boot -T standalone -C none -a 0x2100000 -e 0 -n "hello" -d $(BIN)  $(BIN).img
#------------------------------------------------------------------------------
$(HWLIBS_LIB): $(HWLIBS_OBJS)
	rm -f $@
	$(AR) -r $@ $(HWLIBS_OBJS)

$(HWLIBS_OBJS): $(HWLIBS_OBJDIR)/%.o: $(HWLIBS_ROOT)/src/hwmgr/%.c | $(HWLIBS_OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(HWLIBS_OBJDIR):
	mkdir $@
	
