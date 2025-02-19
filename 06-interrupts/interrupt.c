#include "types.h"
#include "riscv.h"
#include "platform.h"
#include "os.h"

#define INTERRUPT_REG(reg)  ((volatile uint32_t*)(INTERRUPT + reg))
#define UART1_CPU_IRQ	22

typedef enum{
	UART1_INTR_MAP_REG = 0x0058,
	INTR_STATUS_0_REG  = 0x00F8,
	INTR_STATUS_1_REG  = 0X00FC,
	CPU_INT_ENABLE_REG = 0x0104,
	CPU_INT_TYPE_REG   = 0x0108,
	CPU_INT_CLEAR_REG  = 0x010c,
	CPU_INT_EIP_STATUS_REG = 0x0110,
	CPU_INT_PRI_1_REG  = 0x0118,
	CPU_INT_PRI_2_REG  = 0x011c,
	CPU_INT_PRI_3_REG  = 0x0120,
	CPU_INT_THRESH_REG = 0X0194
}INTERRUPT_Typedef;


void interrupt_init(){
	uint32_t mie = r_mstatus();	
	printf("interrupt init start, mie = %lx\n",mie);
	w_mstatus(0);

	printf("interrupt init start, mie = %lx\n",mie);
	(*INTERRUPT_REG(CPU_INT_TYPE_REG)) &= (~(1 << UART1_CPU_IRQ));

	(*INTERRUPT_REG(UART1_INTR_MAP_REG)) &= (~(0x1f));
	(*INTERRUPT_REG(UART1_INTR_MAP_REG)) = UART1_CPU_IRQ;

	(*INTERRUPT_REG(CPU_INT_PRI_3_REG)) = UART1_CPU_IRQ;
	(*INTERRUPT_REG(CPU_INT_THRESH_REG)) = 1;

	(*INTERRUPT_REG(CPU_INT_ENABLE_REG)) = (1 << UART1_CPU_IRQ);

	w_mstatus(mie);
	printf("interrupt init end,mie = %lx\n",r_mstatus());
	printf("interrupt uart1 : %lx\n",*((volatile uint32_t*)(0x600c2000+0x0058)));
	printf("interrupt status 0 : %lx\n",*((volatile uint32_t*)(0x600c2000+0x00f8)));
	printf("interrupt status 1 : %lx\n",*((volatile uint32_t*)(0x600c2000+0x00fc)));
	printf("interrupt cpu int enable : %lx\n",*((volatile uint32_t*)(0x600c2000+0x0104)));
	printf("interrupt cpu int type : %lx\n",*((volatile uint32_t*)(0x600c2000+0x0108)));
	printf("interrupt eip status : %lx\n",*((volatile uint32_t*)(0x600c2000+0x0110)));
	printf("interrupt int thres : %lx\n",*((volatile uint32_t*)(0x600c2000+0x0194)));
}	
	
int interrupt_claim(){
	uint32_t irq = (*INTERRUPT_REG(INTR_STATUS_0_REG));
	return irq;
}

void interrupt_complete(uint32_t irq){
	(*INTERRUPT_REG(CPU_INT_CLEAR_REG)) |= (1 << 3);
	(*INTERRUPT_REG(CPU_INT_CLEAR_REG)) = 0xffffffff;
	w_mstatus(0);
	printf("complete\n");
	//(*INTERRUPT_REG(UART1_INTR_MAP_REG)) = 0;
}
