# Copyright 2026

APP                = omp_mailbox
SRCS               = src/omp_mailbox.c
INCL_DEVICE_BINARY = true
DEVICE_TARGET_FN  ?= 0

RISCV_CFLAGS += -DDEVICE_TARGET_FN=$(DEVICE_TARGET_FN)

include ../common.mk
