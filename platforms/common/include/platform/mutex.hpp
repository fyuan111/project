#pragma once

#include "define.h"

BEGIN_DECLS
#if defined(FML_ZEPHYR)
#include <zephyr/kernel.h>

typedef struct k_mutex mutex_t;

#define mutex_define(name) K_MUTEX_DEFINE(name)
#define mutex_init(m) k_mutex_init(m)
#define mutex_lock(m) k_mutex_lock(m, K_FOREVER)
#define mutex_unlock(m) k_mutex_unlock(m)
#define mutex_destroy(m)

#elif defined(FML_LINUX)
#include <pthread.h>

typedef pthread_mutex_t mutex_t;

#define mutex_define(name) \
    pthread_mutex_t name = PTHREAD_MUTEX_INITIALIZER

#define mutex_init(m) pthread_mutex_init(m, NULL)
#define mutex_lock(m) pthread_mutex_lock(m)
#define mutex_unlock(m) pthread_mutex_unlock(m)
#define mutex_destroy(m) pthread_mutex_destroy(m)

#endif
END_DECLS

#ifdef __cplusplus
class f_mutex
{
public:
    explicit f_mutex(mutex_t *_mutex) : mutex(_mutex)
    {
        if (mutex != nullptr)
            mutex_lock(mutex);
    }

    static void init(mutex_t *_mutex)
    {
        mutex_init(_mutex);
    }

    ~f_mutex()
    {
        if (mutex != nullptr)
            mutex_unlock(mutex);
    }

    f_mutex(const f_mutex &) = delete;
    f_mutex &operator=(const f_mutex &) = delete;

private:
    mutex_t *mutex;
};

#endif