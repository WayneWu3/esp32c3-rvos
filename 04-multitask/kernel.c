#include "os.h"

void start_kernel(void){
    wdt_disable();
    task_delay(100);

    uart_init();
    uart_puts("Hello, RVOS!\r\n");

    printf("Hello,printf!\r\n"); 

    page_init();
    page_test();

    sched_init();
    os_main();
    schedule();

    uart_puts("Would not go here!\r\n");
    
    while(1){
    };
}
