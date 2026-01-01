
#ifndef _task_h_
#define _task_h_

#include <platform/define.h>
// TODO: 工作队列未完成
// #include <shared_thread/workqueue_manager.hpp>

#if defined(FML_ZEPHYR)

#define SCHED_PRIORITY_DEFAULT 8
#define SCHED_PRIORITY_MAX 0
#define SCHED_PRIORITY_MIN K_LOWEST_THREAD_PRIO
#define CONFIG_FML_MAX_PRIORITY (SCHED_PRIORITY_MIN + 1)

#elif defined(FML_LINUX)

#include <sched.h>
#include <pthread.h>
#define SCHED_PRIORITY_DEFAULT (((sched_get_priority_max(SCHED_FIFO) - sched_get_priority_min(SCHED_FIFO)) / 2) + sched_get_priority_min(SCHED_FIFO))
#define SCHED_PRIORITY_MAX sched_get_priority_max(SCHED_FIFO)
#define SCHED_PRIORITY_MIN sched_get_priority_min(SCHED_FIFO)
#define CONFIG_FML_MAX_PRIORITY 100
#endif

#if defined(FML_ZEPHYR) // 0 —— 15
#define WORK_QUEUE_PRIORITY (SCHED_PRIORITY_MAX + SCHED_PRIORITY_MIN) / 2

// 线程优先级范围:
#define THREAD_HIGHEST_PRIORITY SCHED_PRIORITY_MAX
#define THREAD_HIGH_PRIORITY (SCHED_PRIORITY_MAX + 5)
#define THREAD_MID_PRIORITY (SCHED_PRIORITY_MAX + 10)
#define THREAD_LOW_PRIORITY (SCHED_PRIORITY_MAX + 13)
#define THREAD_LOWEST_PRIORITY (SCHED_PRIORITY_MAX + 15)

// 工作队列优先级范围
#define WORK_QUEUE_HIGH_PRIORITY (WORK_QUEUE_PRIORITY - 5)
#define WORK_QUEUE_MID_PRIORITY WORK_QUEUE_PRIORITY
#define WORK_QUEUE_LOW_PRIORITY (WORK_QUEUE_PRIORITY + 3)

#elif defined(FML_LINUX)
#define WORK_QUEUE_HIGH_PRIORITY SCHED_PRIORITY_DEFAULT

#define THREAD_PRIORITY (SCHED_PRIORITY_MAX - 5)
#endif

/**
 * 静态定义线程，
 */
#define fml_task_define K_THREAD_DEFINE

BEGIN_DECLS

/**
 * 线程定义
 * @param task_name
 * @param priority(The task priority you want to set)
 * @param stack_size(only custom thread)
 */
typedef struct
{
    const char *task_name;
    uint8_t priority;
    size_t stack_size;

} f_thread_t;

END_DECLS

#ifdef __cplusplus
#include <platform/common/atomic.hpp>

// 前向声明
extern "C" int fml_task_create();

// class Task
// {
// public:
//     static uint8_t task_spawn(f_thread_t *task)
//     {
//         if (task == nullptr)
//             return -1;

//         if (task->type == thread)
//         {
//             fml_task_create();
//         }
//         else if (task->type == work_queue)
//         {
//             // TODO: join_work_queue(task->priority);
//         }

//         return fml::globe_tid.fetch_add(1);
//     }

// protected:
//     Task() {}
//     ~Task() {}
// };
#endif

BEGIN_DECLS

int fml_task_create();

int fml_task_delete();

/* The num is lower ,then the priority is higher */
static inline uint8_t fml_priority_convert(int priority)
{
#ifdef FML_ZEPHYR
    return priority;
#elif defined(FML_LINUX)
    return (CONFIG_FML_MAX_PRIORITY - priority - 1);
#endif
}


#define f_sleep     k_sleep
#define f_usleep(x) k_usleep(x)
#define f_msleep(x) k_msleep(x)

END_DECLS

#endif // _task_h_