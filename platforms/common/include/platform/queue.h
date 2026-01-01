#pragma once

#include "define.h"

#if defined(FML_ZEPHYR)
#include <zephyr/kernel.h>

typedef struct k_work f_work_t;
typedef struct k_work_delayable f_work_delayable_t; // 延迟工作项
typedef struct k_work_q f_wq_t;

#elif defined(FML_LINUX)

typedef struct
{
    void *next;
    void (*handler)(void *work);
    void *user_data;
    void *queue;
    volatile uint8_t pending;
} f_work_t;

// Linux平台的延迟工作项（简化实现）
typedef struct
{
    f_work_t work;
    uint64_t delay_us;
} f_work_delayable_t;

typedef struct
{
    f_work_t *head;
    f_work_t *tail;

} f_wq_t;

#endif

typedef uint16_t timeout_t;

/* 选择工作项入队方式 */
typedef enum
{
    SUBMIT_EVENT = 0,
    SUBMIT_DELAY
} mode_t;

typedef enum
{
    none = 0,
    Allow_downgrade = 0x01,
    Periodic = 0x02,
    Alarm = 0x04
} opt_t;

#define none 0x00
#define Allow_downgrade 0x01
#define Periodic 0x02
#define Alarm 0x04

typedef uint8_t opt_t;

/* 设置队列结构体 */
typedef struct
{
    void *stack;
    size_t stack_size;
    int prio;
    void (*thread_entry)(void *p1, void *p2, void *p3); // 自定义线程入口
    void *user_data;                                    // 传递给线程的用户数据
} wqset_t;

/* 工作项结构体集合 */
typedef struct
{
    union
    {
        f_work_t work;
        f_work_delayable_t work_delay;
    };
    opt_t opt;
    timeout_t timeout; // In 100us
} fwork_t;

BEGIN_DECLS

/* =========== workqueue =========== */

/**
 * @brief 初始化队列
 * @param workqueue 队列指针

 */
void queue_init(f_wq_t *workqueue);

/**
 * @brief 开始运行队列
 *
 */
void queue_run(f_wq_t *workqueue);

/**
 * @brief 用于设置队列处理线程的配置
 */
void queue_set_and_start(f_wq_t *workqueue, wqset_t *cfg);

/**
 * @brief 队列是否为空
 *
 * @return 返回true 则队列为空
 */
bool queue_is_empty(f_wq_t *workqueue);

/**
 * @brief 当前在队列的任务数
 *
 * @return int 任务数
 */
int runtime_count(f_wq_t *workqueue);

/* =========== workitem =========== */

void workitem_init(fwork_t *workitem, void (*handler)(f_work_t *));

/**
 * @brief 初始化延迟工作项
 * @param workitem 延迟工作项指针
 * @param handler 回调函数
 */
void workitem_delayable_init(fwork_t *workitem, void (*handler)(f_work_t *));

/**
 * @brief 重调度延迟工作项
 * @param workitem 延迟工作项指针
 * @param queue 工作队列指针
 * @param delay_us 延迟时间（微秒）
 * @return 0成功，负数失败
 */
int workitem_reschedule(f_work_delayable_t *workitem, f_wq_t *queue, uint32_t delay_us);

/**
 * @brief 工作项入队
 *
 */
void workitem_push(f_wq_t *workqueue, f_work_t *work);

/**
 * @brief 延迟入队
 * @param work 待入队任务
 * @param timeout 延迟时间
 */
void workitem_push_delay(f_wq_t *workqueue, f_work_t *work, f_time_t timeout);

/**
 * @brief 工作项出队
 *
 */
int workitem_pop(f_work_t *work);

END_DECLS
