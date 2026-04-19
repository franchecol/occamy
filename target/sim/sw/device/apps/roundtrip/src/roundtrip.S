// Minimal data-path proof for Occamy single-cluster simulation.
// Hart 1 increments a 16-word host buffer whose pointer is passed
// through comm_buffer.usr_data_ptr, then signals the host.

.section .text
.globl _start

_start:
    csrr    a0, mhartid
    li      t0, 1
    bne     a0, t0, park

    // soc_ctrl_scratch_2 holds the host communication buffer pointer.
    lui     t1, 0x2000
    lw      t2, 0x1c(t1)
    lw      t3, 4(t2)

    li      t4, 16
    li      t5, 1

loop_words:
    lw      t6, 0(t3)
    add     t6, t6, t5
    sw      t6, 0(t3)
    addi    t3, t3, 4
    addi    t4, t4, -1
    bnez    t4, loop_words

    li      t1, 0x04000000
    li      t2, 1
    sw      t2, 0(t1)
    fence   iorw, iorw

park:
    wfi
    j       park
