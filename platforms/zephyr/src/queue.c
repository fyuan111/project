#include <platform/queue.h>
#include <platform/log.h>
#include <zephyr/kernel.h>

K_THREAD_STACK_DEFINE(wq0_stack, 2048);
K_THREAD_STACK_DEFINE(wq1_stack, 2048);

static struct k_thread wq0_thread_data;
static struct k_thread wq1_thread_data;

void queue_init(f_wq_t *workqueue)
{
    k_work_queue_init(workqueue);
}

void queue_run(f_wq_t *workqueue)
{
    k_work_queue_run(workqueue, NULL);
}

void queue_set_and_start(f_wq_t *workqueue, wqset_t *cfg)
{
    if (cfg->prio < 2)
    {
        struct k_thread *thread_data;
        k_thread_stack_t *stack;

        thread_data = cfg->prio == 0 ? &wq0_thread_data : &wq1_thread_data;
        stack = cfg->prio == 0 ? wq0_stack : wq1_stack;

        k_thread_create(thread_data, stack, 2048,
                        cfg->thread_entry, cfg->user_data, NULL, NULL,
                        cfg->prio, 0, K_NO_WAIT);
    }
    else
    {
        cfg->stack = k_thread_stack_alloc(cfg->stack_size, 0);
        if (cfg->stack == NULL)
        {
            fml_error("queue_set_and_start: 栈分配失败！\n");
            return;
        }

        k_work_queue_start(workqueue,
                           cfg->stack,
                           cfg->stack_size,
                           cfg->prio,
                           NULL);
    }
}

bool queue_is_empty(f_wq_t *workqueue)
{
    return true; // TODO: implement
}

int runtime_count(f_wq_t *workqueue)
{
    return 0; // TODO: implement
}

/* =========== workitem =========== */

void workitem_init(fwork_t *workitem, void (*handler)(f_work_t *))
{
    if (workitem->opt == SUBMIT_EVENT)
    {
        k_work_init(&workitem->work, handler);
    }
}

void workitem_delayable_init(fwork_t *workitem, void (*handler)(f_work_t *))
{
    if (workitem == NULL || handler == NULL)
    {
        fml_warn("workitem_delayable_init: null parameter");
        return;
    }
    k_work_init_delayable(&workitem->work_delay, handler);
}

int workitem_reschedule(f_work_delayable_t *workitem, f_wq_t *queue, uint32_t delay_us)
{
    return k_work_reschedule_for_queue(queue, workitem, K_USEC(delay_us));
}

void workitem_push(f_wq_t *workqueue, f_work_t *work)
{
    k_work_submit_to_queue(workqueue, work);
}

void workitem_push_delay(f_wq_t *workqueue, f_work_t *work, f_time_t timeout)
{
    // TODO: implement
}

int workitem_cancel(f_work_t *work)
{
    return k_work_cancel(work);
}
