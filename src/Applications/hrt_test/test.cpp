#include "test.hpp"
#include <platform/log.h>
#include <zephyr/kernel.h>

hrt_test::~hrt_test()
{
    fml_info("=== HRTCall Test Destroyed ===\n");
}

void hrt_test::run()
{
    fml_info("=== HRTCall Test Suite ===\n");

    // 测试1: 单次延迟调用 (call_after)
    fml_info("\n[Test 1] call_after - 100ms delay\n");
    timer.test_call_after(100000);
    k_sleep(K_MSEC(200));

    // 测试2: 单次短延迟
    fml_info("\n[Test 2] call_after - 10ms delay\n");
    timer.test_call_after(10000);
    k_sleep(K_MSEC(50));

    // 测试3: 周期调用 (call_loop)
    fml_info("\n[Test 3] call_loop - 50ms period (观察5次)\n");
    timer.test_call_loop(50000);
    k_sleep(K_MSEC(300));
    timer.call_cancel();

    // 测试4: 高频周期调用
    fml_info("\n[Test 4] call_loop - 10ms period (观察10次)\n");
    timer.test_call_loop(10000);
    k_sleep(K_MSEC(120));
    timer.call_cancel();

    // 测试5: 测试 is_pending
    fml_info("\n[Test 5] is_pending() test\n");
    fml_info("Before call_after: is_pending = %d\n", timer.is_pending());
    timer.test_call_after(50000);
    fml_info("After call_after: is_pending = %d\n", timer.is_pending());
    k_sleep(K_MSEC(60));
    fml_info("After expiry: is_pending = %d\n", timer.is_pending());

    // 测试6: 取消测试
    fml_info("\n[Test 6] call_cancel() test\n");
    timer.test_call_after(100000);
    fml_info("Scheduled 100ms alarm\n");
    k_sleep(K_MSEC(20));
    fml_info("Cancelling after 20ms...\n");
    timer.call_cancel();
    k_sleep(K_MSEC(100));
    fml_info("Waited 100ms more, callback should not have fired\n");

    fml_info("\n=== All Tests Complete ===\n");
}

FML_REGISTER_APP(hrt_test);
