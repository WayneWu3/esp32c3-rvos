extern void uart_init(void);
extern void uart_puts(char *s);
extern void wdt_disable(void);
extern void page_init(void);
extern void page_test(void);
extern void printf();
void task_delay(volatile int count)
{
        count *= 50000;
        while (count--);
}
void start_kernel(void){
    wdt_disable();
    uart_init();
    task_delay(100);
    uart_puts("Hello, RVOS!\r\n");
    printf("Hello,printf!\r\n");
    page_init();
    page_test();

    while(1){
    };
}
