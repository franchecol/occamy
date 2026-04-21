// Copyright 2026

#include <stdint.h>

#include "host.c"

#define MBOX_DEVICE_START 0x02U
#define MBOX_DEVICE_DONE 0x04U

#define MBOX_WORDS 16U
#define INPUT_WORD 0x12345678U
#define EXPECTED_WORD (INPUT_WORD + 1U)

struct ring_buf {
    uint32_t head;
    uint32_t size;
    uint32_t tail;
    uint32_t element_size;
    uint64_t data_v;
    uint64_t data_p;
};

struct l3_layout {
    uint32_t a2h_rb;
    uint32_t a2h_mbox;
    uint32_t h2a_mbox;
    uint32_t heap;
};

static volatile struct ring_buf a2h_rb __attribute__((aligned(64)));
static volatile struct ring_buf a2h_mbox __attribute__((aligned(64)));
static volatile struct ring_buf h2a_mbox __attribute__((aligned(64)));
static volatile uint32_t a2h_rb_data[MBOX_WORDS] __attribute__((aligned(64)));
static volatile uint32_t a2h_mbox_data[MBOX_WORDS] __attribute__((aligned(64)));
static volatile uint32_t h2a_mbox_data[MBOX_WORDS] __attribute__((aligned(64)));
static volatile struct l3_layout mailbox_layout __attribute__((aligned(64)));
static volatile uint32_t omp_mailbox_args[2] __attribute__((aligned(64)));
static volatile uint32_t device_target_fn = DEVICE_TARGET_FN;

static void rb_init_host(volatile struct ring_buf *rb, volatile uint32_t *data) {
    rb->head = 0;
    rb->tail = 0;
    rb->size = MBOX_WORDS;
    rb->element_size = sizeof(uint32_t);
    rb->data_v = (uintptr_t)data;
    rb->data_p = (uintptr_t)data;
}

static int rb_host_put_word(volatile struct ring_buf *rb, uint32_t word) {
    uint32_t next_head = (rb->head + 1U) % rb->size;
    if (next_head == rb->tail) return -1;
    ((volatile uint32_t *)(uintptr_t)rb->data_v)[rb->head] = word;
    rb->head = next_head;
    fence();
    return 0;
}

static int rb_host_get_word(volatile struct ring_buf *rb, uint32_t *word) {
    if (rb->tail == rb->head) return -1;
    *word = ((volatile uint32_t *)(uintptr_t)rb->data_v)[rb->tail];
    rb->tail = (rb->tail + 1U) % rb->size;
    fence();
    return 0;
}

static void mbox_write(uint32_t word) {
    while (rb_host_put_word(&h2a_mbox, word)) {
        fence();
    }
}

static uint32_t mbox_read(void) {
    uint32_t word = 0;
    while (rb_host_get_word(&a2h_mbox, &word)) {
        fence();
    }
    return word;
}

static void init_mailboxes(void) {
    rb_init_host(&a2h_rb, a2h_rb_data);
    rb_init_host(&a2h_mbox, a2h_mbox_data);
    rb_init_host(&h2a_mbox, h2a_mbox_data);

    mailbox_layout.a2h_rb = (uint32_t)(uintptr_t)&a2h_rb;
    mailbox_layout.a2h_mbox = (uint32_t)(uintptr_t)&a2h_mbox;
    mailbox_layout.h2a_mbox = (uint32_t)(uintptr_t)&h2a_mbox;
    mailbox_layout.heap = 0;
}

static void program_snitches_for_mailbox_runtime(void) {
    *soc_ctrl_scratch_ptr(1) = (uintptr_t)snitch_main;
    *soc_ctrl_scratch_ptr(2) = (uintptr_t)&mailbox_layout;
}

int main(void) {
    if (device_target_fn == 0) return 2;

    set_d_cache_enable(0);

    omp_mailbox_args[0] = INPUT_WORD;
    omp_mailbox_args[1] = 0;
    init_mailboxes();

    reset_and_ungate_quadrants();
    deisolate_all();
    program_snitches_for_mailbox_runtime();

    fence();
    wakeup_snitches_cl();

    mbox_write(MBOX_DEVICE_START);
    mbox_write(device_target_fn);
    mbox_write((uint32_t)(uintptr_t)&omp_mailbox_args[0]);
    mbox_write(0);

    uint32_t done = mbox_read();
    uint32_t cycles = mbox_read();
    uint32_t dma_wait_cycles = mbox_read();
    (void)cycles;
    (void)dma_wait_cycles;

    if (done != MBOX_DEVICE_DONE) return 3;
    if (omp_mailbox_args[1] != EXPECTED_WORD) return 4;

    return 0;
}
