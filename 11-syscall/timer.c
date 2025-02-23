#include "platform.h"
#include "types.h"
#include "os.h"

/* interval ~= 1s */
#define TIMER_INTERVAL CLINT_TIMEBASE_FREQ

static uint32_t _tick = 0;

#define MAX_TIMER 10
static struct timer timer_list[MAX_TIMER];

typedef enum {
	CONF = 0x0000,
	UNIT0_OP = 0x0004,
	UNIT0_LOAD_HI = 0x000c,
	UNIT0_LOAD_LO = 0x0010,
	UNIT0_VALUE_HI = 0x0040,
	UNIT0_VALUE_LO = 0x0044,
	UNIT0_LOAD = 0x005c,
	TARGET0_HI = 0x001c,
	TARGET0_LO = 0x0020,
	TARGET0_CONF = 0x0034,
	COMP0_LOAD = 0x0050,
	INT_ENA = 0x0064,
	INT_RAW = 0x0068,
	INT_CLR = 0x006c,
	INT_ST = 0x0070
}SYSTIMER_Typedef;

#define SYSTIMER_REG(reg)   ((volatile uint32_t *)(SYSTIMER+reg))

struct timer *timer_create(void (*handler)(void *arg), void *arg, uint32_t timeout)
{
	/* TBD: params should be checked more, but now we just simplify this */
	if (NULL == handler || 0 == timeout) {
		return NULL;
	}

	/* use lock to protect the shared timer_list between multiple tasks */
	spin_lock();

	struct timer *t = &(timer_list[0]);
	for (int i = 0; i < MAX_TIMER; i++) {
		if (NULL == t->func) {
			break;
		}
		t++;
	}
	if (NULL != t->func) {
		spin_unlock();
		return NULL;
	}

	t->func = handler;
	t->arg = arg;
	t->timeout_tick = _tick + timeout;

	spin_unlock();

	return t;
}

void timer_delete(struct timer *timer)
{
	spin_lock();

	struct timer *t = &(timer_list[0]);
	for (int i = 0; i < MAX_TIMER; i++) {
		if (t == timer) {
			t->func = NULL;
			t->arg = NULL;
			break;
		}
		t++;
	}

	spin_unlock();
}

/* this routine should be called in interrupt context (interrupt is disabled) */
static inline void timer_check()
{
	struct timer *t = &(timer_list[0]);
	for (int i = 0; i < MAX_TIMER; i++) {
		if (NULL != t->func) {
			if (_tick >= t->timeout_tick) {
				t->func(t->arg);

				/* once time, just delete it after timeout */
				t->func = NULL;
				t->arg = NULL;

				break;
			}
		}
		t++;
	}
}

void timer_init(){
	struct timer *t = &(timer_list[0]);
	for (int i = 0; i < MAX_TIMER; i++) {
		t->func = NULL; /* use .func to flag if the item is used */
		t->arg = NULL;
		t++;
	}

	//select comp0
	*(SYSTIMER_REG(TARGET0_CONF)) &= ~(1 << 31);
	//set period
	*(SYSTIMER_REG(TARGET0_CONF)) &= ~(0x3ffffff);
	*(SYSTIMER_REG(TARGET0_CONF)) |= 0x2dc6c00;
	//load period
	*(SYSTIMER_REG(COMP0_LOAD)) |= (1 << 0);
	//set period mode
	*(SYSTIMER_REG(TARGET0_CONF)) &= ~(1 << 30);
	*(SYSTIMER_REG(TARGET0_CONF)) |= (1 << 30);
	//enable comp0
	*(SYSTIMER_REG(CONF)) |= (1 << 24);
	//enable int
	*(SYSTIMER_REG(INT_ENA)) |= (1 << 0);
}

void timer_read(){
	*(SYSTIMER_REG(UNIT0_OP)) |= (1 << 30);
	while(((*SYSTIMER_REG(UNIT0_OP)) & (1 << 29)) == 0);
	printf("systimer lo : %lx\n",*SYSTIMER_REG(UNIT0_VALUE_LO));
	printf("systimer hi : %lx\n",*SYSTIMER_REG(UNIT0_VALUE_HI));
}

void timer_isr(){
	*(SYSTIMER_REG(INT_CLR)) |= (1 << 0);
	_tick++;
	printf("tick: %d\n", _tick);

	timer_check();

	schedule();
}




