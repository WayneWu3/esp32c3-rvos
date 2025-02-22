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

    trap_init();
    timer_init();
    interrupt_init();

    //sched_init();
    //os_main();
    //schedule();

    while(1){
	    printf("I am alive!\n");
	    timer_read();
	    task_delay(1000);
    };
}
