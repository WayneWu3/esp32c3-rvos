extern void uart_init(void);
extern void uart_puts(char *s);
extern void wdt_disable(void);
void start_kernel(void){
    wdt_disable();
    uart_init();
    uart_puts("Hello, RVOS!\n");

    while(1){
    };
}
