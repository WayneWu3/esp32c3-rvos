#include "os.h"

extern void trap_test();

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
		task_delay(300);
		printf("Task 0: Running...\r\n");

		//trap_test();
		task_yield();
	}
}

void user_task1(void)
{
	printf("Task 1: Created!\r\n");
	while (1) {
		task_delay(300);
		printf("Task 1: Running...\r\n");
		task_yield();
	}
}

void os_main(void)
{
	task_create(user_task0);
	task_create(user_task1);
	printf("task_create done!\n");
}
