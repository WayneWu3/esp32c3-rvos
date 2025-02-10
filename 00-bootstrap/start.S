    .global _start

    .text

_start:
    la  sp, 0x3fce0000

    j   start_kernel

.end