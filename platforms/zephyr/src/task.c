
#include <zephyr/kernel.h>
#include <platform/mutex.hpp>

// 定义全局 app_mutex
K_MUTEX_DEFINE(app_mutex);

int fml_task_create(struct k_thread *new_thread,
                    k_thread_stack_t *stack,
                    size_t	stack_size,
                    k_thread_entry_t entry,
                    void *p1,
                    void *p2,
                    void *p3,
                    int	prio,
                    uint32_t options,
                    k_timeout_t	delay)
{
    k_tid_t pid = k_thread_create(new_thread,stack,
                                    stack_size,entry,
                                    p1,p2,p3,
                                    prio,options,
                                    delay);
                               
    return (int)(intptr_t)pid;
}
