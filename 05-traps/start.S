    .global _start

    .text

_start:
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
