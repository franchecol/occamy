#include <stdint.h>

#include "host.c"

#define ROUNDTRIP_WORDS 16u

static volatile uint32_t roundtrip_buffer[ROUNDTRIP_WORDS]
    __attribute__((aligned(64)));

static int validate_roundtrip(void) {
    for (uint32_t i = 0; i < ROUNDTRIP_WORDS; ++i) {
        if (roundtrip_buffer[i] != (i + 1u)) return 1;
    }
    return 0;
}

int main(void) {
    for (uint32_t i = 0; i < ROUNDTRIP_WORDS; ++i) {
        roundtrip_buffer[i] = i;
    }

    comm_buffer.usr_data_ptr = (uint32_t)(uintptr_t)&roundtrip_buffer[0];
    fence();

    // Bring the cluster online and program the host/device handshake.
    reset_and_ungate_quadrants();
    deisolate_all();
    enable_sw_interrupts();
    program_snitches();

    // Shared-memory state must be globally visible before the cluster starts.
    fence();
    wakeup_snitches_cl();
    wait_snitches_done();

    return validate_roundtrip();
}
