#pragma once

#ifdef __cplusplus

#include <stdint.h>

namespace fml
{

    template <typename T>
    class atomic
    {
    public:
        atomic() = default;

        explicit atomic(T value) : _value(value) {}

        /**
         * Atomically read the current value
         */
        inline T load(int order = __ATOMIC_SEQ_CST) const
        {
            return __atomic_load_n(&_value, order);
        }

        /**
         * Atomically store a value
         */
        inline void store(T value, int order = __ATOMIC_SEQ_CST)
        {
            __atomic_store_n(&_value, value, order);
        }

        /**
         * Atomically add a number and return the previous value
         */
        inline T fetch_add(T num, int order = __ATOMIC_SEQ_CST)
        {
            return __atomic_fetch_add(&_value, num, order);
        }

        /**
         * Atomically subtract a number and return the previous value
         */
        inline T fetch_sub(T num, int order = __ATOMIC_SEQ_CST)
        {
            return __atomic_fetch_sub(&_value, num, order);
        }

        /**
         * Atomic OR with a number
         */
        inline T fetch_or(T num, int order = __ATOMIC_SEQ_CST)
        {
            return __atomic_fetch_or(&_value, num, order);
        }

        /**
         * Atomic AND with a number
         */
        inline T fetch_and(T num, int order = __ATOMIC_SEQ_CST)
        {
            return __atomic_fetch_and(&_value, num, order);
        }

        /**
         * Atomic XOR with a number
         */
        inline T fetch_xor(T num, int order = __ATOMIC_SEQ_CST)
        {
            return __atomic_fetch_xor(&_value, num, order);
        }

        /**
         * Atomic NAND with a number
         */
        inline T fetch_nand(T num, int order = __ATOMIC_SEQ_CST)
        {
            return __atomic_fetch_nand(&_value, num, order);
        }

        /**
         * Atomic compare and exchange operation.
         *
         * This compares the contents of _value with the contents of *expected.
         * If equal, writes desired into _value and returns true.
         * If not equal, writes current _value into *expected and returns false.
         *
         * @param expected Pointer to expected value (updated on failure)
         * @param desired New value to write if comparison succeeds
         * @param success_order Memory order on success
         * @param failure_order Memory order on failure
         * @return true if exchange succeeded, false otherwise
         */
        inline bool compare_exchange(T *expected, T desired,
                                     int success_order = __ATOMIC_SEQ_CST,
                                     int failure_order = __ATOMIC_SEQ_CST)
        {
            return __atomic_compare_exchange_n(&_value, expected, desired,
                                               false, success_order, failure_order);
        }

    private:
        T _value{};
    };
    using atomic_int = atomic<int>;
    using atomic_int32_t = atomic<int32_t>;
    using atomic_uint = atomic<unsigned int>;
    using atomic_bool = atomic<bool>;

} // namespace fml

#endif // __cplusplus

/*

| 内存顺序          | 强度  | 用途                   | 性能  |
| __ATOMIC_RELAXED | 最弱 | 只保证原子性，不保证顺序     最快
| __ATOMIC_CONSUME | 弱   | 数据依赖顺序               快
| __ATOMIC_ACQUIRE | 中   | 读操作屏障                 中
| __ATOMIC_RELEASE | 中   | 写操作屏障                 中
| __ATOMIC_ACQ_REL | 强   | 读写屏障                  慢
| __ATOMIC_SEQ_CST | 最强  | 全局顺序一致性            最慢

*/