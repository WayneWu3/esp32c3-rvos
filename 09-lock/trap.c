#include "os.h"

extern void _panic_handler(void);
void uart1_handler();
void systimer_handler();
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
		case SYSTIMER_CPU_IRQ:
			printf("timer interruption!\n");
			systimer_handler();
			break;
		case UART1_CPU_IRQ:
			printf("uart1 interruption!\n");
			uart1_handler();
			break;
		case SW_CPU_IRQ:
			printf("sw interruption!\n");
			uint32_t irq = interrupt1_claim();
			if(irq & (1 << SW_INTR)){
				software_interrupt_disable();
				interrupt_complete(irq);
				schedule();
			}
			break;
		default:
			printf("Unknown async exception! Code = %ld\n", cause_code);
			break;
		}
	} else {
		/* Synchronous trap - exception */
		printf("Sync exceptions! Code = %ld\n", cause_code);
		panic("OOPS! What can I do!");
		//return_pc += 4;
	}
	return return_pc;
}

void uart1_handler(){
	uint32_t irq = interrupt0_claim();
	if(irq & (1 << UART1_INTR)){
		uart_isr();
	}else if(irq){
		printf("unknown interrupt :%ld\n",irq);
	}
	if(irq){
		interrupt_complete(irq);
	}
}

void systimer_handler(){
	uint32_t irq = interrupt1_claim();
	if(irq & (1 << SYSTIMER_INTR)){
		timer_isr();
	}else if(irq){
		printf("unknown interrupt :%ld\n",irq);
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

