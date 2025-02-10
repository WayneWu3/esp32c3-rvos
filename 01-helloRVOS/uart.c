#include "types.h"
#include "platform.h"

#define UART_REG(reg)   ((volatile uint32_t*)(UART1 + (reg << 2)))
#define GPIO_REG(reg)   ((volatile uint32_t*)(GPIO + reg))
#define IO_MUX_REG(reg)  ((volatile uint32_t*)(IO_MUX + (reg << 2)))
#define SYS_REG(reg)    ((volatile uint32_t*)(SYSREG + (reg << 2)))

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
    GPIO4 = 5
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

#define uart_read(reg)      (*(UART_REG(reg)))
#define uart_write(reg,v)   (*(UART_REG(reg)) = (v))

#define sysreg_read(reg)      (*(SYS_REG(reg)))
#define sysreg_write(reg,v)   (*(SYS_REG(reg)) = (v))

void uart_init(){

    //CPU_CLK = PLL_CLK,APB_CLK=80MHz
    *(SYS_REG(SYSCLK_CONF)) &= (~(0xfff));
    *(SYS_REG(SYSCLK_CONF)) |= 0x401;

    //enable uart clock
    *(SYS_REG(PERIP_CLK_EN0)) |= (1 << 5);

    //reset uart
    *(UART_REG(CLK_CONF)) |= (1 << 23);
    *(SYS_REG(PERIP_RST_EN0)) |= (1 << 5);
    *(SYS_REG(PERIP_RST_EN0)) &= (~(1 << 5));
    *(UART_REG(CLK_CONF)) &= (~(1 << 23));
    // *(UART_REG(ID)) &= (~(1 << 30));
    // while((uart_read(ID) & (0x01 << 31)) != 0);

    //uart configuration    UART_INPUT_CORE = APB_CLK = 80MHz baudrate = 115200
    *(UART_REG(CLK_CONF)) = ( (*(UART_REG(CLK_CONF)) & (~(0x3 << 20)))| (1 << 20));
    *(UART_REG(CLKDIV)) |= 0x7002b6;
    *(UART_REG(CLK_CONF)) &= (~(0xff << 12));//No Div

    //BINUM = 8, PARITY_EN = DISABLE, STOP_BIT = 1
    *(UART_REG(CONF0)) &= (~(1 << 1));
    *(UART_REG(CONF0)) |= 0x1c;
    *(UART_REG(IDLE_CONF)) &= (~(0x3ff << 10));
    *(UART_REG(CONF1)) &= (~(1 << 20));
    //rxfifo rst
    *(UART_REG(CONF0)) |= (1 << 17);
    *(UART_REG(CONF0)) &= (~(1 << 17));
    //txfifo rst
    *(UART_REG(CONF0)) |= (1 << 18);
    *(UART_REG(CONF0)) &= (~(1 << 18));
    // uart_id |= (0x01 << 31);
    // uart_write(ID,uart_id);

    //GPIO func select
    *(IO_MUX_REG(GPIO4)) &= (~(0x7 << 12));
    *(IO_MUX_REG(GPIO4)) |= (1<< 12);
    //GPIO Pin connect to uart peripheral
    *(GPIO_REG(0x0554+4*4)) &= (~(0xff));
    *(GPIO_REG(0x0554+4*4)) |= (0x9);
    //use peripheral output enable signal
    *(GPIO_REG(0x0554+4*4)) &= (~(1 << 9));
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

