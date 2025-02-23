#include "types.h"
#include "riscv.h"
#include "platform.h"
#include "os.h"

#define INTERRUPT_REG(reg)  ((volatile uint32_t*)(INTERRUPT + reg))

typedef enum{
	UART1_INTR_MAP_REG = 0x0058,
	SYSTIMER_TARGET0_INT_MAP_REG = 0x0094,
	CPU_INTR_FROM_CPU_0_MAP_REG = 0x00C8,
	INTR_STATUS_0_REG  = 0x00F8,
	INTR_STATUS_1_REG  = 0X00FC,
	CPU_INT_ENABLE_REG = 0x0104,
	CPU_INT_TYPE_REG   = 0x0108,
	CPU_INT_CLEAR_REG  = 0x010c,
	CPU_INT_EIP_STATUS_REG = 0x0110,
	CPU_INT_PRI_1_REG  = 0x0118,
	CPU_INT_PRI_2_REG  = 0x011c,
	CPU_INT_PRI_3_REG  = 0x0120,
	CPU_INT_PRI_5_REG  = 0x0128,
	CPU_INT_PRI_7_REG  = 0x0130,
	CPU_INT_PRI_8_REG  = 0x0134,
	CPU_INT_THRESH_REG = 0X0194
}INTERRUPT_Typedef;


void interrupt_init(){
	uint32_t mie = r_mstatus();	
	w_mstatus(0);
	
	//set int type
	(*INTERRUPT_REG(CPU_INT_TYPE_REG)) &= (~(1 << UART1_CPU_IRQ));
	(*INTERRUPT_REG(CPU_INT_TYPE_REG)) &= (~(1 << SYSTIMER_CPU_IRQ));
	(*INTERRUPT_REG(CPU_INT_TYPE_REG)) &= (~(1 << SW_CPU_IRQ));
	//map the external int into cpu int
	(*INTERRUPT_REG(UART1_INTR_MAP_REG)) &= (~(0x1f));
	(*INTERRUPT_REG(UART1_INTR_MAP_REG)) = UART1_CPU_IRQ;
	(*INTERRUPT_REG(SYSTIMER_TARGET0_INT_MAP_REG)) &= (~(0x1f));
	(*INTERRUPT_REG(SYSTIMER_TARGET0_INT_MAP_REG)) = SYSTIMER_CPU_IRQ;
	(*INTERRUPT_REG(CPU_INTR_FROM_CPU_0_MAP_REG)) &= (~(0x1f));
	(*INTERRUPT_REG(CPU_INTR_FROM_CPU_0_MAP_REG)) = SW_CPU_IRQ;
	//set priority
	(*INTERRUPT_REG(CPU_INT_PRI_7_REG)) = UART1_CPU_IRQ;
	(*INTERRUPT_REG(CPU_INT_PRI_5_REG)) = SYSTIMER_CPU_IRQ;
	(*INTERRUPT_REG(CPU_INT_PRI_8_REG)) = SW_CPU_IRQ;
	//set threshold
	(*INTERRUPT_REG(CPU_INT_THRESH_REG)) = 1;
	//enable cpu int
	(*INTERRUPT_REG(CPU_INT_ENABLE_REG)) |= (1 << UART1_CPU_IRQ);
	(*INTERRUPT_REG(CPU_INT_ENABLE_REG)) |= (1 << SYSTIMER_CPU_IRQ);
	(*INTERRUPT_REG(CPU_INT_ENABLE_REG)) |= (1 << SW_CPU_IRQ);

	w_mstatus(mie);
}	
	
int interrupt0_claim(){
	uint32_t irq = (*INTERRUPT_REG(INTR_STATUS_0_REG));
	return irq;
}

int interrupt1_claim(){
	uint32_t irq = (*INTERRUPT_REG(INTR_STATUS_1_REG));
	return irq;
}

void interrupt_complete(uint32_t irq){
	if(irq & (1 << UART1_INTR)){
		(*INTERRUPT_REG(CPU_INT_CLEAR_REG)) |= (1 << UART1_CPU_IRQ);
	}else if(irq & (1 << SYSTIMER_INTR)){
		(*INTERRUPT_REG(CPU_INT_CLEAR_REG)) |= (1 << SYSTIMER_CPU_IRQ);
	}else if(irq & (1 << SW_INTR)){
		(*INTERRUPT_REG(CPU_INT_CLEAR_REG)) |= (1 << SW_CPU_IRQ);
	}
}
