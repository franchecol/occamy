// Minimal device payload for Occamy single-cluster simulation.
// Hart 1 raises a host software interrupt, all harts park afterwards.

.section .text
.globl _start

_start:
    csrr    a0, mhartid
    li      t0, 1
    bne     a0, t0, park

    li      t1, 0x04000000
    li      t2, 1
    sw      t2, 0(t1)
    fence   iorw, iorw

park:
    wfi
    j       park
