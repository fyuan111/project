#pragma once

#include <stdint.h>

#if defined(FML_ZEPHYR)
#include <zephyr/kernel.h>

typedef struct k_sem f_sem_t;

#define __fml_sem_init(sem, init, limit) k_sem_init(sem, init, limit)
#define __fml_sem_take_blocking(sem) k_sem_take(sem, K_FOREVER)
#define __fml_sem_take_timeout(sem, ms) k_sem_take(sem, (ms) == 0 ? K_NO_WAIT : K_MSEC(ms))
#define __fml_sem_give(sem) k_sem_give(sem)
#define __fml_sem_count_get(sem) k_sem_count_get(sem)
#define __fml_sem_destroy(sem) ((void)0)

#elif defined(FML_LINUX)
#include <semaphore.h>
#include <time.h>
#include <errno.h>

typedef sem_t f_sem_t;

#define __fml_sem_init(sem, init, limit) sem_init(sem, 0, init)
#define __fml_sem_take_blocking(sem) sem_wait(sem)
#define __fml_sem_give(sem) sem_post(sem)
#define __fml_sem_destroy(sem) sem_destroy(sem)

static inline int __fml_sem_take_timeout(sem_t *sem, uint32_t timeout_ms)
{
    if (timeout_ms == 0)
    {
        return sem_trywait(sem);
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000)
    {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }
    return sem_timedwait(sem, &ts);
}

static inline uint32_t __fml_sem_count_get(sem_t *sem)
{
    int val;
    sem_getvalue(sem, &val);
    return (uint32_t)val;
}

#endif

namespace fml
{

    /**
     * @brief 初始化信号量
     * @param sem 信号量指针
     * @param initial_count 初始计数值
     * @param limit 最大计数值（Linux 忽略）
     * @return 0 成功，-1 失败
     */
    inline int sem_init(f_sem_t *sem, uint32_t initial_count, uint32_t limit)
    {
        if (!sem)
            return -1;
        return __fml_sem_init(sem, initial_count, limit);
    }

    /**
     * @brief 等待信号量（阻塞）
     * @param sem 信号量指针
     * @return 0 成功，-1 失败
     */
    inline int sem_take(f_sem_t *sem)
    {
        return __fml_sem_take_blocking(sem);
    }

    /**
     * @brief 等待信号量（超时）
     * @param sem 信号量指针
     * @param timeout_ms 超时时间（毫秒），0=立即返回
     * @return 0 成功，-EAGAIN 超时，-1 其他错误
     */
    inline int sem_take_timeout(f_sem_t *sem, uint32_t timeout_ms)
    {
        return __fml_sem_take_timeout(sem, timeout_ms);
    }

    /**
     * @brief 释放信号量
     * @param sem 信号量指针
     * @return 0 成功，-1 失败
     */
    inline int sem_give(f_sem_t *sem)
    {
        __fml_sem_give(sem);
        return 0;
    }

    /**
     * @brief 获取信号量当前计数
     * @param sem 信号量指针
     * @return 当前计数值
     */
    inline uint32_t sem_count_get(f_sem_t *sem)
    {
        if (!sem)
            return 0;
        return __fml_sem_count_get(sem);
    }

    /**
     * @brief 销毁信号量
     * @param sem 信号量指针
     */
    inline void sem_destroy(f_sem_t *sem)
    {
        if (sem)
        {
            __fml_sem_destroy(sem);
        }
    }

} // namespace fml
