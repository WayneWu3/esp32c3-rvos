    .global _start

    .text

_start:
    /* Setup the global pointer, which the ABI assumes points to the
     * __global_pointer$ symbol. */
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

    lui x15,0x3fc83
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

    j   start_kernel

.end
