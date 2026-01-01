#include "../time_ring.hpp"
#include "test_utils.hpp"

using namespace fml;

// ============ 测试用例 ============

/**
 * @brief 测试基本比较（无溢出）
 */
bool test_basic_comparison()
{
    uint32_t a = 100;
    uint32_t b = 200;

    TEST_ASSERT(TimeRing32::less(a, b), "100 should be less than 200");
    TEST_ASSERT(TimeRing32::less_eq(a, b), "100 should be less_eq than 200");
    TEST_ASSERT(!TimeRing32::greater(a, b), "100 should not be greater than 200");
    TEST_ASSERT(TimeRing32::greater(b, a), "200 should be greater than 100");

    return true;
}

/**
 * @brief 测试时间溢出场景
 */
bool test_overflow()
{
    uint32_t past = 0xFFFFFFFE;    // 溢出前2个tick
    uint32_t future = 0x00000002;  // 溢出后2个tick

    TEST_ASSERT(TimeRing32::less(past, future), "Past should be less than future across overflow");
    TEST_ASSERT(TimeRing32::greater(future, past), "Future should be greater than past across overflow");

    return true;
}

/**
 * @brief 测试边界条件
 */
bool test_edge_cases()
{
    uint32_t t1 = 0xFFFFFFFF;
    uint32_t t2 = 0x00000000;

    TEST_ASSERT(TimeRing32::less(t1, t2), "0xFFFFFFFF should be less than 0x00000000");

    // 测试相等
    uint32_t t3 = 1000;
    uint32_t t4 = 1000;
    TEST_ASSERT(TimeRing32::less_eq(t3, t4), "Equal times should satisfy less_eq");
    TEST_ASSERT(TimeRing32::greater_eq(t3, t4), "Equal times should satisfy greater_eq");
    TEST_ASSERT(!TimeRing32::less(t3, t4), "Equal times should not satisfy less");
    TEST_ASSERT(!TimeRing32::greater(t3, t4), "Equal times should not satisfy greater");

    return true;
}

/**
 * @brief 测试时间差计算
 */
bool test_diff()
{
    uint32_t a = 100;
    uint32_t b = 200;

    uint32_t diff = TimeRing32::diff(a, b);
    TEST_ASSERT_EQ(diff, 100, "Diff should be 100");

    // 跨溢出的时间差
    uint32_t past = 0xFFFFFFF0;   // 距离溢出16个tick
    uint32_t future = 0x00000010; // 溢出后16个tick
    uint32_t diff2 = TimeRing32::diff(past, future);
    TEST_ASSERT_EQ(diff2, 32, "Diff across overflow should be 32");

    return true;
}

/**
 * @brief 测试16位时间环
 */
bool test_16bit_time_ring()
{
    uint16_t a = 0xFFFE;
    uint16_t b = 0x0002;

    TEST_ASSERT(TimeRing16::less(a, b), "16-bit: 0xFFFE < 0x0002");
    TEST_ASSERT(TimeRing16::greater(b, a), "16-bit: 0x0002 > 0xFFFE");

    uint16_t diff = TimeRing16::diff(a, b);
    TEST_ASSERT_EQ(diff, 4, "16-bit: diff should be 4");

    return true;
}

/**
 * @brief 测试大跨度时间（接近最大范围）
 */
bool test_large_span()
{
    // 32位：最大有效范围是 2^31 ticks
    uint32_t now = 0;
    uint32_t far_future = 0x7FFFFFFF;  // 2^31 - 1

    TEST_ASSERT(TimeRing32::less(now, far_future), "Should handle 2^31-1 span");

    // 超过 2^31 会出现反转（这是设计限制，不是bug）
    uint32_t too_far = 0x80000001;
    // 从 0 看 0x80000001，已经超过最大范围，会被误判为"过去"
    TEST_ASSERT(TimeRing32::less(too_far, now), "Beyond 2^31 wraps around (design limitation)");

    return true;
}

// ============ 主函数 ============

int main()
{
    int total = 0, passed = 0, failed = 0;

    printf("\n========== TimeRing Unit Tests ==========\n\n");

    RUN_TEST(test_basic_comparison);
    RUN_TEST(test_overflow);
    RUN_TEST(test_edge_cases);
    RUN_TEST(test_diff);
    RUN_TEST(test_16bit_time_ring);
    RUN_TEST(test_large_span);

    TEST_SUMMARY();

    return (failed == 0) ? 0 : 1;
}
