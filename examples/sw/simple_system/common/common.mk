# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

COMMON_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

COMMON_SRCS = $(wildcard $(COMMON_DIR)/*.c)
INCS := -I$(COMMON_DIR)

# ARCH = rv32im # to disable compressed instructions
ARCH ?= rv32imf
# ARCH ?= rv32im_zfinx #to use Zfinx extension

ifdef PROGRAM
PROGRAM_C := $(PROGRAM).c
endif

SRCS = $(COMMON_SRCS) $(PROGRAM_C) $(EXTRA_SRCS)

C_SRCS = $(filter %.c, $(SRCS))
ASM_SRCS = $(filter %.S, $(SRCS))

CC = riscv32-corev-elf-gcc

CROSS_COMPILE = $(patsubst %-gcc,%-,$(CC))
OBJCOPY ?= $(CROSS_COMPILE)objcopy
OBJDUMP ?= $(CROSS_COMPILE)objdump

LINKER_SCRIPT ?= $(COMMON_DIR)/link.ld
CRT ?= $(COMMON_DIR)/crt0.S
# CFLAGS ?= -march=$(ARCH) -mabi=ilp32 -static -mcmodel=medany -Wall -g -Os\
	# -fvisibility=hidden -nostdlib -nostartfiles -ffreestanding $(PROGRAM_CFLAGS)
# CFLAGS ?= -march=$(ARCH) -mabi=ilp32 -static -mcmodel=medany -Wall -O3 -ffast-math\
	# -fvisibility=hidden -nostdlib -nostartfiles -ffreestanding $(PROGRAM_CFLAGS)
CFLAGS ?= -march=$(ARCH) -mabi=ilp32f -static -mcmodel=medlow -mtune=sifive-3-series \
  -O3 -falign-functions=16 -funroll-all-loops -finline-functions -falign-jumps=4 -nostdlib -nostartfiles -ffreestanding -mstrict-align

# Remove f from -mabi for Zfinx:
# CFLAGS ?= -march=$(ARCH) -mabi=ilp32 -static -mcmodel=medlow -mtune=sifive-3-series \
#  -O3 -falign-functions=16 -funroll-all-loops -finline-functions -falign-jumps=4 -nostdlib -nostartfiles -ffreestanding -mstrict-align

OBJS := ${C_SRCS:.c=.o} ${ASM_SRCS:.S=.o} ${CRT:.S=.o}
DEPS = $(OBJS:%.o=%.d)

ifdef PROGRAM
OUTFILES := $(PROGRAM).elf $(PROGRAM).vmem $(PROGRAM).bin
else
OUTFILES := $(OBJS)
endif

all: $(OUTFILES)

ifdef PROGRAM
$(PROGRAM).elf: $(OBJS) $(LINKER_SCRIPT)
	$(CC) $(CFLAGS) -T $(LINKER_SCRIPT) $(OBJS) -o $@ $(LIBS)

.PHONY: disassemble
disassemble: $(PROGRAM).dis
endif

%.dis: %.elf
	$(OBJDUMP) -fhSD $^ > $@

# Note: this target requires the srecord package to be installed.
# XXX: This could be replaced by objcopy once
# https://sourceware.org/bugzilla/show_bug.cgi?id=19921
# is widely available.
%.vmem: %.bin
	srec_cat $^ -binary -offset 0x0000 -byte-swap 4 -o $@ -vmem

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

%.o: %.c
	$(CC) $(CFLAGS) -MMD -c $(INCS) -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) -MMD -c $(INCS) -o $@ $<

clean:
	$(RM) -f $(OBJS) $(DEPS)

distclean: clean
	$(RM) -f $(OUTFILES)
