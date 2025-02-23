#include "platform.h"
#include "types.h"
#include "os.h"

typedef enum {
	CONF = 0x0000,
	UNIT0_OP = 0x0004,
	UNIT0_LOAD_HI = 0x000c,
	UNIT0_LOAD_LO = 0x0010,
	UNIT0_VALUE_HI = 0x0040,
	UNIT0_VALUE_LO = 0x0044,
	UNIT0_LOAD = 0x005c,
	TARGET0_HI = 0x001c,
	TARGET0_LO = 0x0020,
	TARGET0_CONF = 0x0034,
	COMP0_LOAD = 0x0050,
	INT_ENA = 0x0064,
	INT_RAW = 0x0068,
	INT_CLR = 0x006c,
	INT_ST = 0x0070
}SYSTIMER_Typedef;

#define SYSTIMER_REG(reg)   ((volatile uint32_t *)(SYSTIMER+reg))

void timer_init(){
	//select comp0
	*(SYSTIMER_REG(TARGET0_CONF)) &= ~(1 << 31);
	//set period
	*(SYSTIMER_REG(TARGET0_CONF)) &= ~(0x3ffffff);
	*(SYSTIMER_REG(TARGET0_CONF)) |= 0x2dc6c00;
	//load period
	*(SYSTIMER_REG(COMP0_LOAD)) |= (1 << 0);
	//set period mode
	*(SYSTIMER_REG(TARGET0_CONF)) &= ~(1 << 30);
	*(SYSTIMER_REG(TARGET0_CONF)) |= (1 << 30);
	//enable comp0
	*(SYSTIMER_REG(CONF)) |= (1 << 24);
	//enable int
	*(SYSTIMER_REG(INT_ENA)) |= (1 << 0);
}

void timer_read(){
	*(SYSTIMER_REG(UNIT0_OP)) |= (1 << 30);
	while(((*SYSTIMER_REG(UNIT0_OP)) & (1 << 29)) == 0);
	printf("systimer lo : %lx\n",*SYSTIMER_REG(UNIT0_VALUE_LO));
	printf("systimer hi : %lx\n",*SYSTIMER_REG(UNIT0_VALUE_HI));
}

void timer_isr(){
	*(SYSTIMER_REG(INT_CLR)) |= (1 << 0);
}




