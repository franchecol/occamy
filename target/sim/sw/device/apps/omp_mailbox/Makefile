# Copyright 2026

APP     = omp_mailbox
SRC_DIR = $(abspath src)
LIBOMPTARGET_DEVICE_DIR = $(abspath ../libomptarget_device)

SRCS  = $(LIBOMPTARGET_DEVICE_DIR)/src/main.c
SRCS += $(LIBOMPTARGET_DEVICE_DIR)/src/sw_mailbox.c
SRCS += $(SRC_DIR)/target.c

INCDIRS += $(LIBOMPTARGET_DEVICE_DIR)/src

include ../common.mk
