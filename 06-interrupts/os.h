#ifndef __OS_H__
#define __OS_H__

#include "types.h"
#include "riscv.h"
#include "platform.h"

#include <stddef.h>
#include <stdarg.h>

/* uart */
extern void uart_init(void);
extern char uart_getc();
extern void uart_putc(char ch);
extern void uart_puts(char *s);
extern void uart_isr();

/* rtc */
extern void wdt_disable(void);

/* printf */
//extern int  printf(const char* s, ...);
extern void panic(char *s);

/* memory management */
extern void page_init(void);
extern void *page_alloc(int npages);
extern void page_free(void *p);
extern void page_test(void);

/* task management */
struct context {
	/* ignore x0 */
	reg_t ra;
	reg_t sp;
	reg_t gp;
	reg_t tp;
	reg_t t0;
	reg_t t1;
	reg_t t2;
	reg_t s0;
	reg_t s1;
	reg_t a0;
	reg_t a1;
	reg_t a2;
	reg_t a3;
	reg_t a4;
	reg_t a5;
	reg_t a6;
	reg_t a7;
	reg_t s2;
	reg_t s3;
	reg_t s4;
	reg_t s5;
	reg_t s6;
	reg_t s7;
	reg_t s8;
	reg_t s9;
	reg_t s10;
	reg_t s11;
	reg_t t3;
	reg_t t4;
	reg_t t5;
	reg_t t6;
};

extern void sched_init(void);
extern void schedule(void);

extern int  task_create(void (*task)(void));
extern void task_yield();
extern void task_delay(volatile int count);

void os_main(void);

extern void trap_init(void);
extern void trap_test(void);

extern void interrupt_init();
extern int interrupt_claim();
extern void interrupt_complete();
extern void printf();
#endif /* __OS_H__ */
