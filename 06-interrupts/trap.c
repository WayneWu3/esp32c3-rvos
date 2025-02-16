#include "os.h"

extern void _panic_handler(void);
void external_interrupt_handler();
void trap_init()
{
	/*
	 * set the trap-vector base-address for machine-mode
	 */
	//w_mtvec((reg_t)trap_vector);
}

reg_t trap_handler(reg_t epc, reg_t cause)
{
	reg_t return_pc = epc;
	reg_t cause_code = cause & MCAUSE_MASK_ECODE;
	
	if (cause & MCAUSE_MASK_INTERRUPT) {
		/* Asynchronous trap - interrupt */
		switch (cause_code) {
		case 1:
			printf("uart1 interruption! id = %d\n",1);
			external_interrupt_handler();
			break;
		case 3:
			printf("software interruption!\n");
			break;
		case 7:
			printf("timer interruption!\n");
			break;
		case 11:
			printf("external interruption!\n");
			break;
		case 22:
			printf("uart1 interruption! id = %d\n",22);
			external_interrupt_handler();
			break;
		default:
			printf("Unknown async exception! Code = %ld\n", cause_code);
			break;
		}
	} else {
		/* Synchronous trap - exception */
		printf("Sync exceptions! Code = %ld\n", cause_code);
		//panic("OOPS! What can I do!");
		return_pc += 4;
	}

	return return_pc;
}

void external_interrupt_handler(){
	uint32_t irq = interrupt_claim();
	if(irq &= 0x01){
		uart_isr();
	}else if(irq){
		printf("unexpected interrupt irq = %d\n", irq);
	}

	if(irq){
		interrupt_complete(irq);
	}
}

void trap_test()
{
	/*
	 * Synchronous exception code = 7
	 * Store/AMO access fault
	 */
	w_marchid(0);
	/*
	 * Synchronous exception code = 5
	 * Load access fault
	 */
	int b = *(int *)0x00000000;

	printf("Yeah! I'm return back from trap!\n");
}

