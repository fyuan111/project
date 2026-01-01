#pragma once

#include <stdint.h>
#include <type_traits>

namespace fml {

/**
 * @brief 环形时间比较工具
 *
 * 用于处理定时器计数器溢出场景（如32位counter从 0xFFFFFFFF 溢出到 0x00000000）
 *
 * 原理：利用有符号整数的补码特性
 * 例如：当前时间 0xFFFFFFFF，未来时间 0x00000001
 *      (int32_t)(0x00000001 - 0xFFFFFFFF) = 2 > 0
 *
 * 适用范围：时间差 < 2^(N-1)（对于 N 位无符号整数）
 *          32位：最大时间差 2^31 ticks
 *          16位：最大时间差 2^15 ticks
 *
 * @tparam T 时间类型（uint32_t, uint16_t 等）
 *
 * @example
 * uint32_t now = 0xFFFFFFFE;
 * uint32_t future = 0x00000002;
 * bool is_future = TimeRing<uint32_t>::less(now, future);  // true
 */
template<typename T>
class TimeRing
{
public:
    // 自动推导对应的有符号类型
    using signed_t = typename std::conditional<
        sizeof(T) == 4, int32_t,
        typename std::conditional<sizeof(T) == 2, int16_t, int8_t>::type
    >::type;

    /**
     * @brief 判断时间 a 是否早于 b（a < b）
     * @param a 时间点 A
     * @param b 时间点 B
     * @return true 如果 a 在 b 之前（环形时间意义下）
     */
    static constexpr bool less(T a, T b) noexcept
    {
        return static_cast<signed_t>(a - b) < 0;
    }

    /**
     * @brief 判断时间 a 是否早于或等于 b（a <= b）
     */
    static constexpr bool less_eq(T a, T b) noexcept
    {
        return static_cast<signed_t>(a - b) <= 0;
    }

    /**
     * @brief 判断时间 a 是否晚于 b（a > b）
     */
    static constexpr bool greater(T a, T b) noexcept
    {
        return static_cast<signed_t>(a - b) > 0;
    }

    /**
     * @brief 判断时间 a 是否晚于或等于 b（a >= b）
     */
    static constexpr bool greater_eq(T a, T b) noexcept
    {
        return static_cast<signed_t>(a - b) >= 0;
    }

    /**
     * @brief 计算时间差（b - a）
     * @warning 仅当 a <= b 时结果有效
     * @return 从 a 到 b 的时间差
     */
    static constexpr T diff(T a, T b) noexcept
    {
        return b - a;
    }
};

// ============ 类型别名 ============

using TimeRing32 = TimeRing<uint32_t>;  // 32位时间环
using TimeRing16 = TimeRing<uint16_t>;  // 16位时间环
using TimeRing8  = TimeRing<uint8_t>;   // 8位时间环

} // namespace fml
