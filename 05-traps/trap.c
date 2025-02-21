#include "os.h"

extern void trap_vector(void);

void trap_init()
{
	/*
	 * set the trap-vector base-address for machine-mode
	 */
	w_mtvec((reg_t)trap_vector);
}

void trap_readreg(){
	reg_t mscratch = r_mscratch();
	printf("mscratch = %lx\n",mscratch);
	for(int i = 0;i < 32;i++){
		printf("no.%d = %lx\n",i,*(volatile uint32_t*)(mscratch+i*4));
	}
}

reg_t trap_handler(reg_t epc, reg_t cause)
{
	reg_t return_pc = epc;
	reg_t cause_code = cause & MCAUSE_MASK_ECODE;
	
	trap_readreg();
	if (cause & MCAUSE_MASK_INTERRUPT) {
		/* Asynchronous trap - interrupt */
		switch (cause_code) {
		case 3:
			printf("software interruption!\n");
			break;
		case 7:
			printf("timer interruption!\n");
			break;
		case 11:
			printf("external interruption!\n");
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

void trap_test()
{
	/*
	 * Synchronous exception code = 7
	 * Store/AMO access fault
	 */
//    int b = *(int *)0xFFFFFFF;
//	printf("b = %d\n",b);
//	*(int *)0x00000000 = 100;
	printf("test\n");
	w_marchid(0);
	printf("test\n");
	int a = 1/0;
	int c = 0/1;
	int d = *(int *)0xFFFFFFFFFFFF;
	int e = *(char*)0x42000001;
	/*
	 * Synchronous exception code = 5
	 * Load access fault
	 */
	int b = *(int *)0x00000000;

	printf("Yeah! I'm return back from trap!\n");
}

