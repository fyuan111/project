#pragma once

/**
 * @file event.h
 * @brief FML Event Group Abstraction Layer
 * @version 1.0.0
 */

#include <stdint.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============ 类型定义 ============

/**
 * @brief Event bits type
 */
typedef uint32_t event_bits_t;

// ============ 平台适配层 ============

#if defined(FML_ZEPHYR)
#include <zephyr/kernel.h>

typedef struct k_event f_event_t;

#define __fml_event_init(event) k_event_init(event)
#define __fml_event_post(event, events) k_event_post(event, events)
#define __fml_event_set(event, events) k_event_set(event, events)
#define __fml_event_clear(event, events) k_event_clear(event, events)

#elif defined(FML_LINUX)
#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    event_bits_t bits;
} f_event_t;

int __fml_event_init_impl(f_event_t *event);
int __fml_event_post_impl(f_event_t *event, event_bits_t events);
int __fml_event_set_impl(f_event_t *event, event_bits_t events);
int __fml_event_clear_impl(f_event_t *event, event_bits_t events);
void __fml_event_destroy_impl(f_event_t *event);

#define __fml_event_init(event) __fml_event_init_impl(event)
#define __fml_event_post(event, events) __fml_event_post_impl(event, events)
#define __fml_event_set(event, events) __fml_event_set_impl(event, events)
#define __fml_event_clear(event, events) __fml_event_clear_impl(event, events)

#else
#error "Unsupported platform"
#endif

// ============ C API ============

/**
 * @brief Initialize event object
 *
 * @param event Pointer to event object
 * @return 0 on success, negative errno on failure
 */
int fml_event_init(f_event_t *event);

/**
 * @brief Destroy event object
 *
 * @param event Pointer to event object
 */
void fml_event_destroy(f_event_t *event);

/**
 * @brief Post (set) event bits - ISR safe
 *
 * Performs OR operation with existing bits.
 *
 * @param event  Pointer to event object
 * @param events Event bits to set
 * @return 0 on success, negative errno on failure
 *
 * @note This function is ISR-safe
 */
static inline int fml_event_post(f_event_t *event, event_bits_t events)
{
    if (!event) return -EINVAL;
    __fml_event_post(event, events);
    return 0;
}

/**
 * @brief Set event bits (overwrite) - ISR safe
 *
 * @param event  Pointer to event object
 * @param events New event bits value
 * @return 0 on success, negative errno on failure
 */
static inline int fml_event_set(f_event_t *event, event_bits_t events)
{
    if (!event) return -EINVAL;
    __fml_event_set(event, events);
    return 0;
}

/**
 * @brief Clear event bits
 *
 * @param event  Pointer to event object
 * @param events Event bits to clear
 * @return 0 on success, negative errno on failure
 */
static inline int fml_event_clear(f_event_t *event, event_bits_t events)
{
    if (!event) return -EINVAL;
    __fml_event_clear(event, events);
    return 0;
}

/**
 * @brief Wait for any event bits (blocking)
 *
 * @param event  Pointer to event object
 * @param events Event bits to wait for
 * @return Received event bits on success, 0 on error
 */
event_bits_t fml_event_wait_any(f_event_t *event, event_bits_t events);

/**
 * @brief Wait for event bits with timeout
 *
 * @param event      Pointer to event object
 * @param events     Event bits to wait for
 * @param timeout_ms Timeout in milliseconds (0 = no wait, UINT32_MAX = forever)
 * @return Received event bits on success, 0 on timeout/error
 */
event_bits_t fml_event_wait_timeout(f_event_t *event, event_bits_t events, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

// ============ C++ API ============

#ifdef __cplusplus
namespace fml
{
    /**
     * @brief Initialize event object
     */
    inline int event_init(f_event_t *event)
    {
        return fml_event_init(event);
    }

    /**
     * @brief Destroy event object
     */
    inline void event_destroy(f_event_t *event)
    {
        fml_event_destroy(event);
    }

    /**
     * @brief Post event bits (ISR-safe)
     */
    inline int event_post(f_event_t *event, event_bits_t events)
    {
        return fml_event_post(event, events);
    }

    /**
     * @brief Set event bits (ISR-safe)
     */
    inline int event_set(f_event_t *event, event_bits_t events)
    {
        return fml_event_set(event, events);
    }

    /**
     * @brief Clear event bits
     */
    inline int event_clear(f_event_t *event, event_bits_t events)
    {
        return fml_event_clear(event, events);
    }

    /**
     * @brief Wait for any event bits
     */
    inline event_bits_t event_wait_any(f_event_t *event, event_bits_t events)
    {
        return fml_event_wait_any(event, events);
    }

    /**
     * @brief Wait for event bits with timeout
     */
    inline event_bits_t event_wait_timeout(f_event_t *event, event_bits_t events, uint32_t timeout_ms)
    {
        return fml_event_wait_timeout(event, events, timeout_ms);
    }

} // namespace fml
#endif
