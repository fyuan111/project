#pragma once

/**
 * @file event_platform.h
 * @brief FML Event Group - Platform Interface (Internal)
 *
 * @note This header is for internal use only.
 *       Platform implementations should implement these functions.
 */

#include <platform/event.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============ 平台实现接口 ============

/**
 * @brief Platform-specific event initialization
 * @note Implemented by platforms/zephyr or platforms/linux
 */
int __fml_event_init_platform(f_event_t *event);

/**
 * @brief Platform-specific event destruction
 * @note Implemented by platforms/zephyr or platforms/linux
 */
void __fml_event_destroy_platform(f_event_t *event);

/**
 * @brief Platform-specific wait for any event bits
 * @note Implemented by platforms/zephyr or platforms/linux
 */
event_bits_t __fml_event_wait_any_platform(f_event_t *event, event_bits_t events);

/**
 * @brief Platform-specific wait with timeout
 * @note Implemented by platforms/zephyr or platforms/linux
 */
event_bits_t __fml_event_wait_timeout_platform(f_event_t *event, event_bits_t events, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif
