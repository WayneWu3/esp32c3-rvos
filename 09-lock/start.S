    .global _start

    .text

_start:

    lui x15,0x3fc85
    csrw mscratch,x15

    la  sp, _stack_end

    la a0, _data_start
    la a1, _data_lma
    la a2, _data_size
    call memcpy

    la	a0, _bss_start
    la	a2, _bss_end
    sub	a2, a2, a0
    li	a1, 0
    call memset

    /* Set MTVEC */
    la      t0, _vector_table
    ori     t0, t0, 1
    csrrw   zero, mtvec, t0

    j   start_kernel

.end
