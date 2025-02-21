#include "os.h"

extern void trap_test();
void start_kernel(void){
    wdt_disable();
    task_delay(100);

    uart_init();
    uart_puts("Hello, RVOS!\r\n");

    printf("Hello,printf!\r\n"); 

    page_init();
    page_test();

    //sched_init();
    trap_init();
    trap_test();

    os_main();
    schedule();

    uart_puts("Would not go here!\r\n");
    
    while(1){
    };
}
