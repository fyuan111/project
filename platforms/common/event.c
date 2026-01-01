/**
 * @file event.c
 * @brief FML Event Group - Common Implementation
 */

#include <platform/event.h>
#include <platform/event_platform.h>

// ============ 通用 API 实现 ============

int fml_event_init(f_event_t *event)
{
    if (!event)
        return -EINVAL;
    return __fml_event_init_platform(event);
}

void fml_event_destroy(f_event_t *event)
{
    if (!event)
        return;
    __fml_event_destroy_platform(event);
}

event_bits_t fml_event_wait_any(f_event_t *event, event_bits_t events)
{
    if (!event)
        return 0;
    return __fml_event_wait_any_platform(event, events);
}

event_bits_t fml_event_wait_timeout(f_event_t *event, event_bits_t events, uint32_t timeout_ms)
{
    if (!event)
        return 0;
    return __fml_event_wait_timeout_platform(event, events, timeout_ms);
}
