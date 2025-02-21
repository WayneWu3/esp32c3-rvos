#include "types.h"
#include "platform.h"
#include "os.h"

#define UART_REG(reg)   ((volatile uint32_t*)(UART1 + (reg << 2)))
#define GPIO_REG(reg)   ((volatile uint32_t*)(GPIO + reg))
#define IO_MUX_REG(reg)  ((volatile uint32_t*)(IO_MUX + (reg << 2)))
#define SYS_REG(reg)    ((volatile uint32_t*)(SYSREG + (reg << 2)))
#define TIMG0_REG(reg)   ((volatile uint32_t*)(TIMG0 + (reg << 2)))
#define TIMG1_REG(reg)   ((volatile uint32_t*)(TIMG1 + (reg << 2)))
#define RTC_REG(reg)   ((volatile uint32_t*)(RTC + (reg << 2)))

typedef enum
{
    BT_SELECT = 0x0000,
    OUT = 0x0004,
    OUT_W1TS = 0x0008,
    OUT_W1TC = 0x000C,
    ENABLE = 0x0020,
    ENABLE_W1TS = 0x0024,
    ENABLE_W1TC = 0x0028,
    STRAP = 0x0038,
    IN = 0x003C,
    STATUS_GPIO = 0x0044,
    STATUS_W1TS = 0x0048,
    STATUS_W1TC = 0x004C,
    PCPU_INT = 0x005C,
    STATUS_NEXT = 0x014C,
    CLOCK_GATE_GPIO = 0x062C
}GPIO_TypeDef;

typedef enum{
    GPIO4 = 5,
    GPIO5 = 6
}IO_MUX_Typedef;

typedef enum
{
    FIFO = 0,
    INT_RAW  = 1,
    INT_ST = 2,
    INT_ENA = 3,
    INT_CLR = 4,
    CLKDIV = 5,
    RX_FILT = 6,
    STATUS = 7,
    CONF0 = 8,
    CONF1 = 9,
    LOWPULSE = 10,
    HIGHPULSE = 11,
    RXD_CNT = 12,
    FLOW_CONF = 13,
    SLEEP_CONF = 14,
    SWFC_CONF0 = 15,
    SWFC_CONF1 = 16,
    TXBRK_CONF = 17,
    IDLE_CONF = 18,
    RS485_CONF = 19,
    AT_CMD_PRECNT = 20,
    AT_CMD_POSTCNT = 21,
    AT_CMD_GAPTOUT = 22,
    AT_CMD_CHAR = 23,
    MEM_CONF = 24,
    MEM_TX_STATUS = 25,
    MEM_RX_STATUS = 26,
    FSM_STATUS = 27,
    POSPULSE = 28,
    NEGPULSE = 29,
    CLK_CONF = 30,
    DATA = 31,
    ID = 32,
}UART_TypeDef;

typedef enum
{
    CPU_PERI_CLK_EN = 0,
    CPU_PERI_RST_EN = 1,
    CPU_PER_CONF = 2,
    PERIP_CLK_EN0 = 4,
    PERIP_CLK_EN1 = 5,
    PERIP_RST_EN0 = 6,
    PERIP_RST_EN1 = 7,
    CPU_INTR_FROM_CPU_O = 10,
    CPU_INTR_FROM_CPU_1 = 11,
    CPU_INTR_FROM_CPU_2 = 12,
    CPU_INTR_FROM_CPU_3 = 13,
    CLOCK_GATE = 21,
    SYSCLK_CONF = 22
}SYSREG_TypeDef;

typedef enum{
    DIG_ISO_REG = 35,
    WDTCONFIGO_REG = 36,
    WDTWPROTECT_REG = 42,
    SWD_CONF_REG = 43,
    SWD_WPROTECT_REG = 44, SW_CPU_STALL_REG = 45 }RTC_TypeDef;

typedef enum{
    RTCCALICFG_REG = 18,
    REGCLK_REG = 63
}TIMG_TypeDef;


#define uart_read(reg)      (*(UART_REG(reg)))
#define uart_write(reg,v)   (*(UART_REG(reg)) = (v))

#define sysreg_read(reg)      (*(SYS_REG(reg)))
#define sysreg_write(reg,v)   (*(SYS_REG(reg)) = (v))

#define rtc_read(reg)      (*(RTC_REG(reg)))
#define rtc_write(reg,v)   (*(RTC_REG(reg)) = (v))

#define timg0_read(reg)      (*(TIMG0_REG(reg)))
#define timg0_write(reg,v)   (*(TIMG0_REG(reg)) = (v))

#define timg1_read(reg)      (*(TIMG1_REG(reg)))
#define timg1_write(reg,v)   (*(TIMG1_REG(reg)) = (v))

void uart_init(){

    //CPU_CLK = PLL_CLK,APB_CLK=80MHz
    *(SYS_REG(SYSCLK_CONF)) &= (~(0xfff));
    *(SYS_REG(SYSCLK_CONF)) |= 0x401;

    //enable uart clock
    *(SYS_REG(PERIP_CLK_EN0)) |= ((1 << 24) | (1 << 5));

    //reset uart
    *(SYS_REG(PERIP_RST_EN0)) &= (~(1 << 5));
    *(UART_REG(CLK_CONF)) |= (1 << 23);
    *(SYS_REG(PERIP_RST_EN0)) |= (1 << 5);
    *(SYS_REG(PERIP_RST_EN0)) &= (~(1 << 5));
    *(UART_REG(CLK_CONF)) &= (~(1 << 23));
    *(UART_REG(ID)) &= (~(1 << 30));
    while((uart_read(ID) & (0x01 << 31)) != 0);

    //uart configuration    UART_INPUT_CORE = APB_CLK = 80MHz baudrate = 115200
    *(UART_REG(CLK_CONF)) = ( (*(UART_REG(CLK_CONF)) & (~(0x3 << 20)))| (1 << 20));
    *(UART_REG(CLKDIV)) |= 0x7002b6;
    *(UART_REG(CLK_CONF)) &= (~(0xff << 12));//No Div

    //BINUM = 8, PARITY_EN = DISABLE, STOP_BIT = 1
    *(UART_REG(CONF0)) &= (~(1 << 1));
    *(UART_REG(CONF0)) |= 0x1c;
    *(UART_REG(IDLE_CONF)) &= (~(0x3ff << 10));
    *(UART_REG(CONF1)) &= (~(1 << 20));
    *(UART_REG(ID)) |= (1 << 31);
    //rxfifo rst
    *(UART_REG(CONF0)) |= (1 << 17);
    *(UART_REG(CONF0)) &= (~(1 << 17));
    //txfifo rst
    *(UART_REG(CONF0)) |= (1 << 18);
    *(UART_REG(CONF0)) &= (~(1 << 18));

    *(UART_REG(CONF1)) &= (~(0x1ff << 9));
    *(UART_REG(INT_ENA)) &= (~(1 << 1)); 

    *(UART_REG(CONF1)) &= (~(0x1ff));
    *(UART_REG(CONF1)) |= 0x10;
    *(UART_REG(INT_ENA)) |= (1 << 0); 

    //GPIO func select
    *(IO_MUX_REG(GPIO4)) &= (~(0x7 << 12));
    *(IO_MUX_REG(GPIO4)) |= (1<< 12);

    *(IO_MUX_REG(GPIO5)) &= (~(0x7 << 12));
    *(IO_MUX_REG(GPIO5)) |= (1<< 12);
    *(IO_MUX_REG(GPIO5)) |= (1<< 8);
    *(IO_MUX_REG(GPIO5)) |= (1<< 9);

    //GPIO Pin connect to uart peripheral
    *(GPIO_REG(0x0554+4*4)) &= (~(0xff));
    *(GPIO_REG(0x0554+4*4)) |= (0x9);

    *(GPIO_REG(0x0154+4*9)) &= (~(0x7f));
    *(GPIO_REG(0x0154+4*9)) |= (0x5);
    *(GPIO_REG(0x0154+4*9)) |= (1 << 6);
    //use peripheral output enable signal
    *(GPIO_REG(0x0554+4*9)) |= (1 << 6);
}

void uart_gets(void){
	uint8_t buf[32],i = 0;
	int cnt = (*UART_REG(STATUS)) & 0X3ff;
	if(cnt != 0){
		for(i = 0;i < cnt;i++)
			*(buf+i) = (*UART_REG(FIFO)) & 0xff;
		*(buf+i) = '\0';
		printf("recv :%s\n",buf);
	}
	*(UART_REG(CONF0)) |= (1 << 17);
	*(UART_REG(CONF0)) &= (~(1 << 17));
	return;
}

void uart_putc(char ch){
    //write data
    *(UART_REG(FIFO)) = (((*(UART_REG(FIFO))) & (~(0xff))) | ((int)(ch)));
    //while( (((uart_read(STATUS)) & (0x3ff << 16)) == 0) && ((((uart_read(FSM_STATUS)) & (0x0f << 4)) == 0)));
}

void uart_puts(char *s){
    while(*s){
        uart_putc(*s++);
    }
}

void uart_isr(){
	while(1){
		*(UART_REG(INT_CLR)) |= (1 << 0);
		uart_gets();
		break;
	}
}

void wdt_disable(void){
    *(RTC_REG(WDTWPROTECT_REG)) = 0x50d83aa1;
    *(RTC_REG(DIG_ISO_REG)) = 0;
    *(RTC_REG(WDTCONFIGO_REG)) = 0; 
    
    *(RTC_REG(SWD_WPROTECT_REG)) = 0x8F1D312A;
    *(RTC_REG(SWD_CONF_REG)) |= (1 << 31);
    *(RTC_REG(SW_CPU_STALL_REG)) = 0;

    *(TIMG0_REG(REGCLK_REG)) &= (~(1 << 9));
    *(TIMG0_REG(RTCCALICFG_REG)) = 0;
    *(TIMG1_REG(RTCCALICFG_REG)) = 0;
}
