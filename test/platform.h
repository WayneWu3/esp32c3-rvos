#ifndef  __PLATFORM_H
#define __PLATFORM_H

#define LENGTH_RAM	320*1024
#define LENGTH_IROM	4*1024*1024
#define LENGTH_DROM	4*1024*1024

#define SYSREG  0x600C0000L
#define GPIO    0x60004000L
#define IO_MUX  0x60009000L
#define UART1   0x60010000L
#define RTC	    0x60008000L
#define TIMG0	0x6001F000L
#define TIMG1	0x60020000L
#define INTERRUPT 0x600C2000L
#define SYSTIMER  0x60023000

#define UART1_INTR	22
#define UART1_CPU_IRQ	7

#define SYSTIMER_INTR	  5
#define SYSTIMER_CPU_IRQ  5

#define SW_INTR		18
#define SW_CPU_IRQ  	8

#endif
