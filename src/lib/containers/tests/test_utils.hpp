#pragma once

#include <stdio.h>

// 简单的测试宏（不依赖外部框架）
#define TEST_ASSERT(condition, msg) \
    do { \
        if (!(condition)) { \
            printf("[FAIL] %s:%d - %s\n", __FILE__, __LINE__, msg); \
            return false; \
        } \
    } while(0)

#define TEST_ASSERT_EQ(a, b, msg) \
    do { \
        if ((a) != (b)) { \
            printf("[FAIL] %s:%d - %s (expected: %d, got: %d)\n", \
                   __FILE__, __LINE__, msg, (int)(b), (int)(a)); \
            return false; \
        } \
    } while(0)

#define RUN_TEST(test_func) \
    do { \
        printf("Running %s...", #test_func); \
        if (test_func()) { \
            printf(" [PASS]\n"); \
            passed++; \
        } else { \
            failed++; \
        } \
        total++; \
    } while(0)

#define TEST_SUMMARY() \
    printf("\n========== Test Summary ==========\n"); \
    printf("Total: %d, Passed: %d, Failed: %d\n", total, passed, failed); \
    printf("==================================\n")
