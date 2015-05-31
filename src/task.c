#include "task.h"
#include "kconfig.h"
#include "kernel.h"
#include "stm32f10x.h"
#include "stm32_p103.h"
#include "RTOSConfig.h"

#include "syscall.h"

#include <stddef.h>
#include <ctype.h>
#include <string.h>
#include "string.h"
#include "task.h"
#include "memory-pool.h"
#include "path.h"
#include "pipe.h"
#include "fifo.h"
#include "mqueue.h"
#include "block.h"
#include "romdev.h"
#include "event-monitor.h"
#include "romfs.h"
#include <stddef.h>

extern unsigned int tick_count;

/* System resources */
extern struct task_control_block tasks[TASK_LIMIT];
extern unsigned int stacks[TASK_LIMIT][STACK_SIZE];
extern char memory_space[MEM_LIMIT];
extern struct file *files[FILE_LIMIT];
extern struct file_request requests[TASK_LIMIT];
extern struct list ready_list[PRIORITY_LIMIT + 1];  /* [0 ... 39] */
extern struct event events[EVENT_LIMIT];
extern size_t current_task;
extern size_t task_count;
extern struct task_control_block *current_tcb;
extern struct memory_pool memory_pool;
extern struct event_monitor event_monitor;
extern struct list *list;

int prv_priority = PRIORITY_DEFAULT;
unsigned int *init_task(unsigned int *stack, void (*start)())
{
	stack += STACK_SIZE - 18; /* End of stack, minus what we're about to push */
	stack[8] = (unsigned int)start;
	stack[16] = (unsigned int)start;
	stack[17] =	(unsigned int)0x01000000;
	return stack;
}

void task_create(int priority, void *func){

    tasks[task_count].stack = (void*)init_task(stacks[task_count], func);
    tasks[task_count].pid = task_count;
    tasks[task_count].priority = priority;
    list_init(&tasks[task_count].list);
    list_push(&ready_list[tasks[task_count].priority], &tasks[task_count].list);
    task_count++;

}

