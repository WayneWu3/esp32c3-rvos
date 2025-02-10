    .global _start

    .text

_start:
    la  sp, 0x3fcd0000

    j   start_kernel

.end