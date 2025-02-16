#include "os.h"

void panic(char *s)
{
	printf("panic: ");
	printf(s);
	printf("\n");
	while(1){};
}

void user_task0(void)
{
	printf("Task 0: Created!\r\n");
	while (1) {
		printf("Task 0: Running...\r\n");
		task_delay(1000);
		task_yield();
	}
}

void user_task1(void)
{
	printf("Task 1: Created!\r\n");
	while (1) {
		printf("Task 1: Running...\r\n");
		task_delay(1000);
		task_yield();
	}
}

void os_main(void)
{
	task_create(user_task0);
	task_create(user_task1);
}
