#include "os.h"

char ch;

void start_kernel(void){
    wdt_disable();
    task_delay(100);

    uart_init();
    uart_puts("Hello, RVOS!\r\n");

    printf("Hello,printf!\r\n"); 

    page_init();
    page_test();

    interrupt_init();
    trap_init();

    sched_init();
    //os_main();
    //schedule();

    uart_puts("Would not go here!\r\n");
    
    while(1){
	    printf("I am alive!\n");
	    if((ch = uart_getc()) != 0){
		printf("uart rx : %c!\n",ch);
		uart_putc(ch);
	    }
	    task_delay(1000);
    };
}
