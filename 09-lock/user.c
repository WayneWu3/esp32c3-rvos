#include "os.h"

#define DELAY 1000

#define USE_LOCK

void panic(char *s)
{
	printf("panic: ");
	printf(s);
	printf("\n");
	while(1){};
}

void user_task0(void)
{
	while (1) {
#ifdef USE_LOCK
		spin_lock();
#endif
		printf("Task 0: Begin ... \n");
		for (int i = 0; i < 5; i++) {
			printf("Task 0: Running... \n");
			task_delay(DELAY);
		}
		printf("Task 0: End ... \n");
#ifdef USE_LOCK
		spin_unlock();
#endif
	}
}

void user_task1(void)
{
	printf("Task 1: Created!\n");
	while (1) {
		printf("Task 1: Begin ... \n");
		for (int i = 0; i < 5; i++) {
			printf("Task 1: Running... \n");
			task_delay(DELAY);
		}
		printf("Task 1: End ... \n");
	}
}

/* NOTICE: DON'T LOOP INFINITELY IN main() */
void os_main(void)
{
	task_create(user_task0);
	task_create(user_task1);
}

