#define LOAD		lw
#define STORE		sw
#define SIZE_REG	4

	.equ SAVE_REGS, 28
	.equ CONTEXT_SIZE, (SAVE_REGS * 4)

# Save all General-Purpose(GP) registers to context.
# struct context *base = &ctx_task;
# base->ra = ra;
# ......
# These GP registers to be saved don't include gp
# and tp, because they are not caller-saved or
# callee-saved. These two registers are often used
# for special purpose. For example, in RVOS, 'tp'
# (aka "thread pointer") is used to store hartid,
# which is a global value and would not be changed
# during context-switch.
.macro reg_save base
	STORE ra,   0*SIZE_REG(\base)
	STORE sp,   1*SIZE_REG(\base)
	STORE t0,   4*SIZE_REG(\base)
	STORE t1,   5*SIZE_REG(\base)
	STORE t2,   6*SIZE_REG(\base)
	STORE s0,   7*SIZE_REG(\base)
	STORE s1,   8*SIZE_REG(\base)
	STORE a0,   9*SIZE_REG(\base)
	STORE a1,  10*SIZE_REG(\base)
	STORE a2,  11*SIZE_REG(\base)
	STORE a3,  12*SIZE_REG(\base)
	STORE a4,  13*SIZE_REG(\base)
	STORE a5,  14*SIZE_REG(\base)
	STORE a6,  15*SIZE_REG(\base)
	STORE a7,  16*SIZE_REG(\base)
	STORE s2,  17*SIZE_REG(\base)
	STORE s3,  18*SIZE_REG(\base)
	STORE s4,  19*SIZE_REG(\base)
	STORE s5,  20*SIZE_REG(\base)
	STORE s6,  21*SIZE_REG(\base)
	STORE s7,  22*SIZE_REG(\base)
	STORE s8,  23*SIZE_REG(\base)
	STORE s9,  24*SIZE_REG(\base)
	STORE s10, 25*SIZE_REG(\base)
	STORE s11, 26*SIZE_REG(\base)
	STORE t3,  27*SIZE_REG(\base)
	STORE t4,  28*SIZE_REG(\base)
	STORE t5,  29*SIZE_REG(\base)
	# we don't save t6 here, due to we have used
	# it as base, we have to save t6 in an extra step
	# outside of reg_save
.endm

# restore all General-Purpose(GP) registers from the context
# except gp & tp.
# struct context *base = &ctx_task;
# ra = base->ra;
# ......
.macro reg_restore base
	LOAD ra,   0*SIZE_REG(\base)
	LOAD sp,   1*SIZE_REG(\base)
	LOAD t0,   4*SIZE_REG(\base)
	LOAD t1,   5*SIZE_REG(\base)
	LOAD t2,   6*SIZE_REG(\base)
	LOAD s0,   7*SIZE_REG(\base)
	LOAD s1,   8*SIZE_REG(\base)
	LOAD a0,   9*SIZE_REG(\base)
	LOAD a1,  10*SIZE_REG(\base)
	LOAD a2,  11*SIZE_REG(\base)
	LOAD a3,  12*SIZE_REG(\base)
	LOAD a4,  13*SIZE_REG(\base)
	LOAD a5,  14*SIZE_REG(\base)
	LOAD a6,  15*SIZE_REG(\base)
	LOAD a7,  16*SIZE_REG(\base)
	LOAD s2,  17*SIZE_REG(\base)
	LOAD s3,  18*SIZE_REG(\base)
	LOAD s4,  19*SIZE_REG(\base)
	LOAD s5,  20*SIZE_REG(\base)
	LOAD s6,  21*SIZE_REG(\base)
	LOAD s7,  22*SIZE_REG(\base)
	LOAD s8,  23*SIZE_REG(\base)
	LOAD s9,  24*SIZE_REG(\base)
	LOAD s10, 25*SIZE_REG(\base)
	LOAD s11, 26*SIZE_REG(\base)
	LOAD t3,  27*SIZE_REG(\base)
	LOAD t4,  28*SIZE_REG(\base)
	LOAD t5,  29*SIZE_REG(\base)
	LOAD t6,  30*SIZE_REG(\base)
.endm

# Something to note about save/restore:
# - We use mscratch to hold a pointer to context of current task
# - We use t6 as the 'base' for reg_save/reg_restore, because it is the
#   very bottom register (x31) and would not be overwritten during loading.
#   Note: CSRs(mscratch) can not be used as 'base' due to load/restore
#   instruction only accept general purpose registers.


#trap_vector:

# void switch_to(struct context *next);
# a0: pointer to the context of the next task
.global switch_to
.balign 0x100
switch_to:
	# switch mscratch to pointer to the context of the next task
	csrw mscratch, a0
	# set mepc to the pc of the next task
	LOAD	a1, 31*SIZE_REG(a0)
	csrw	mepc, a1

	# Restore all GP registers
	#Use t6 to point to the context of the new task
	mv	t6, a0
	reg_restore t6
	mret

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
        reg_save t6

        # Save the actual t6 register, which we swapped into
        # mscratch
        mv      t5, t6                  # t5 points to the context of current task
        csrr    t6, mscratch            # read t6 back from mscratch
        STORE   t6, 30*SIZE_REG(t5)     # save t6 with t5 as base

	csrr	a0, mepc
	STORE	a0, 31*SIZE_REG(t5)

        # Restore the context pointer into mscratch
        csrw    mscratch, t5

        # call the C trap handler in trap.c
        csrr    a0, mepc
        csrr    a1, mcause
	csrr	a2, mscratch
        call    trap_handler

        # trap_handler will return the return address via a0.
        csrw    mepc, a0

        # restore context(registers).
        csrr    t6, mscratch
        reg_restore t6

        mret
    .size  _panic_handler, .-_panic_handler

    /* This is the interrupt handler.
     * It is declared as a weak function, and can be overridden.
     */
    .global _interrupt_handler
    .weak _interrupt_handler
    .type _interrupt_handler, @function
_interrupt_handler:
        # save context(registers).
        csrrw   t6, mscratch, t6        # swap t6 and mscratch
        reg_save t6

        # Save the actual t6 register, which we swapped into
        # mscratch
        mv      t5, t6                  # t5 points to the context of current task
        csrr    t6, mscratch            # read t6 back from mscratch
        STORE   t6, 30*SIZE_REG(t5)     # save t6 with t5 as base

	# Restore the context of current task
	csrr	a0, mepc
	STORE	a0, 31*SIZE_REG(t5)

        # Restore the context pointer into mscratch
        csrw    mscratch, t5

        # call the C trap handler in trap.c
        csrr    a0, mepc
        csrr    a1, mcause
        jal    trap_handler

        # trap_handler will return the return address via a0.
        csrw    mepc, a0
        # restore context(registers).
        csrr    t6, mscratch
        reg_restore t6

        mret

#    csrr    a0, mepc
#    csrr    a1, mcause
#    call trap_handler
#    mret
    .size  _interrupt_handler, .-_interrupt_handler
.end


