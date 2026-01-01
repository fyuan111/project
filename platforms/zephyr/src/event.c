/**
 * @file event.c
 * @brief FML Event Group - Zephyr Platform Implementation
 */

#include <platform/event.h>
#include <platform/event_platform.h>

// ============ Zephyr 平台实现 ============

int __fml_event_init_platform(f_event_t *event)
{
    k_event_init(event);
    return 0;
}

void __fml_event_destroy_platform(f_event_t *event)
{
    // Zephyr 的 k_event 不需要显式销毁
    (void)event;
}

event_bits_t __fml_event_wait_any_platform(f_event_t *event, event_bits_t events)
{
    return k_event_wait(event, events, true, K_FOREVER);
}

event_bits_t __fml_event_wait_timeout_platform(f_event_t *event, event_bits_t events, uint32_t timeout_ms)
{
    k_timeout_t timeout;

    if (timeout_ms == 0) {
        timeout = K_NO_WAIT;
    } else if (timeout_ms == UINT32_MAX) {
        timeout = K_FOREVER;
    } else {
        timeout = K_MSEC(timeout_ms);
    }

    return k_event_wait(event, events, true, timeout);
}
