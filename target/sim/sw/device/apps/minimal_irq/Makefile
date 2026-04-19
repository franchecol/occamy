# Copyright 2026

APP                := minimal_irq
BUILDDIR           := $(abspath build)
SRC                := $(abspath src/$(APP).S)
RISCV_CC           ?= riscv64-unknown-elf-gcc
RISCV_OBJCOPY      ?= riscv64-unknown-elf-objcopy
RISCV_OBJDUMP      ?= riscv64-unknown-elf-objdump
RISCV_CFLAGS       += -march=rv32im_zicsr
RISCV_CFLAGS       += -mabi=ilp32
RISCV_CFLAGS       += -mno-relax
RISCV_CFLAGS       += -nostdlib
RISCV_CFLAGS       += -nostartfiles
RISCV_CFLAGS       += -static
RISCV_CFLAGS       += -Wl,-Ttext=0
RISCV_CFLAGS       += -Wl,--build-id=none

ELF  := $(BUILDDIR)/$(APP).elf
BIN  := $(BUILDDIR)/$(APP).bin
DUMP := $(BUILDDIR)/$(APP).dump

.PHONY: all clean

all: $(BIN) $(DUMP)

clean:
	rm -rf $(BUILDDIR)

$(BUILDDIR):
	mkdir -p $@

$(ELF): $(SRC) | $(BUILDDIR)
	$(RISCV_CC) $(RISCV_CFLAGS) $< -o $@

$(BIN): $(ELF)
	$(RISCV_OBJCOPY) -O binary $< $@

$(DUMP): $(ELF)
	$(RISCV_OBJDUMP) -D $< > $@
