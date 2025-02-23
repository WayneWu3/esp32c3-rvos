    /* This is the vector table. MTVEC points here.
     *
     * Use 4-byte intructions here. 1 instruction = 1 entry of the table.
     * The CPU jumps to MTVEC (i.e. the first entry) in case of an exception,
     * and (MTVEC & 0xfffffffc) + (mcause & 0x7fffffff) * 4, in case of an interrupt.
     */
    .balign 0x100
    .global _vector_table
    .type _vector_table, @function
_vector_table:
    .option push
    .option norvc
    j _panic_handler            /* exception handler, entry 0 */
    .rept 31
    j _interrupt_handler        /* 31 identical entries, all pointing to the interrupt handler */
    .endr
    .option pop
    .size _vector_table, .-_vector_table

    /* This should be the exception handler.
     * It doesn't do anything useful yet, but you can set a breakpoint here, to see if an exception happens.
     */
    .global _panic_handler
    .type _panic_handler, @function
_panic_handler:
        # save context(registers).
        csrrw   t6, mscratch, t6        # swap t6 and mscratch
        //reg_save t6

        # Save the actual t6 register, which we swapped into
        # mscratch
        mv      t5, t6                  # t5 points to the context of current task
        csrr    t6, mscratch            # read t6 back from mscratch
        //STORE   t6, 30*SIZE_REG(t5)     # save t6 with t5 as base

        # Restore the context pointer into mscratch
        csrw    mscratch, t5

        # call the C trap handler in trap.c
        csrr    a0, mepc
        csrr    a1, mcause
        call    trap_handler

        # trap_handler will return the return address via a0.
        csrw    mepc, a0

        # restore context(registers).
        csrr    t6, mscratch
        //reg_restore t6

        # return to whatever we were doing before trap.
        mret
    .size  _panic_handler, .-_panic_handler

    /* This is the interrupt handler.
     * It is declared as a weak function, and can be overridden.
     */
    .global _interrupt_handler
    .weak _interrupt_handler
    .type _interrupt_handler, @function
_interrupt_handler:
    mret
    .size  _interrupt_handler, .-_interrupt_handler
