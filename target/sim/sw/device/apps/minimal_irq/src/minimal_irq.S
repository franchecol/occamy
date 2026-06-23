// Minimal device payload for Occamy single-cluster simulation.
// Hart 1 raises a host software interrupt, all harts park afterwards.

.section .text
.globl _start

_start:
    // Read this Snitch hart's hardware ID.
    csrr    a0, mhartid

    // Keep the proof single-worker: only hart 1 signals the host.
    li      t0, 1
    bne     a0, t0, park

    // Global CLINT MSIP base.  Writing 1 here interrupts CVA6 hart 0.
    li      t1, 0x04000000
    li      t2, 1
    sw      t2, 0(t1)

    // Ensure the MMIO interrupt write is visible before parking.
    fence   iorw, iorw

park:
    // Nothing else to do in M0; all Snitch harts sleep forever.
    wfi
    j       park
