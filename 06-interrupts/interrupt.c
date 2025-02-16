#include "types.h"
#include "riscv.h"
#include "platform.h"
#include "os.h"

#define INTERRUPT_REG(reg)  ((volatile uint32_t*)(INTERRUPT + reg))

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

	printf("interrupt init start\n");
	uint32_t mie = r_mstatus();	
	printf("interrupt init start\n");
	w_mstatus(0);
	printf("interrupt init start\n");

	(*INTERRUPT_REG(CPU_INT_TYPE_REG)) &= (~(1 << 1));
	printf("interrupt init start\n");
	(*INTERRUPT_REG(UART1_INTR_MAP_REG)) = 1;
	printf("interrupt init start\n");
	(*INTERRUPT_REG(CPU_INT_PRI_1_REG)) = 1;
	printf("interrupt init start\n");
	(*INTERRUPT_REG(CPU_INT_THRESH_REG)) = 0;
	printf("interrupt init start\n");
	(*INTERRUPT_REG(CPU_INT_ENABLE_REG)) = (1 <<1);
	printf("interrupt init start\n");

	w_mstatus(mie);

	printf("interrupt init end\n");
}
	
int interrupt_claim(){
	uint32_t irq = (*INTERRUPT_REG(INTR_STATUS_0_REG));
	return irq;
}

void interrupt_complete(uint32_t irq){
	(*INTERRUPT_REG(UART1_INTR_MAP_REG)) = irq;
	(*INTERRUPT_REG(UART1_INTR_MAP_REG)) = 0;
}
