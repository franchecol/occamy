// Copyright 2026

#include <stdint.h>

void omp_mailbox_target(uint64_t args_addr) {
    volatile uint32_t *args = (volatile uint32_t *)(uintptr_t)args_addr;
    args[1] = args[0] + 1U;
}
