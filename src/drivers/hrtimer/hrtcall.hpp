#pragma once

#include "hrt.hpp"
#include <stdint.h>

// 类型定义
using abstime = uint64_t; // 绝对时间（微秒）
using time_t = uint32_t;  // 相对时间（微秒）

/**
 * @brief 高精度定时器回调基类
 *
 * 为工作队列提供高精度调度功能
 * 用户需要继承此类并实现 expiry_call() 和 stop_call()
 */
class hrtcall : public hrt
{
public:
    hrtcall()
    {
        if (!init())
            fml_error("hrt init failed\n");
    }

    ~hrtcall()
    {
        // call_cancel();
    }

    hrtcall(const hrtcall &) = delete;
    hrtcall &operator=(const hrtcall &) = delete;

    /**
     * @brief 在绝对时间点调用
     * @param time 绝对时间（微秒）
     * @param arg 传递给 expiry_call 的参数
     */
    void call_at(abstime time, void *arg = nullptr);

    /**
     * @brief 延迟指定时间后调用
     * @param delay 延迟时间（微秒）
     * @param arg 传递给 expiry_call 的参数
     */
    void call_after(time_t delay, void *arg = nullptr);

    /**
     * @brief 周期性调用
     * @param period 周期时间（微秒）
     * @param arg 传递给 expiry_call 的参数
     */
    void call_loop(time_t period, void *arg = nullptr);

    /**
     * @brief 取消定时调用
     */
    void call_cancel();

    /**
     * @brief 检查是否有待执行的调用
     */
    bool is_pending() const { return is_scheduled.load(); }

protected:

    /**
     * @brief 用户重写：定时器到期时的实际操作
     * @param arg 用户传入的参数
     * @warning 在ISR中执行！只能做轻量级操作
     */
    virtual void expiry_call(void *arg) = 0;

    /**
     * @brief 用户重写：停止定时器时的清理操作
     * @param arg 用户传入的参数
     */
    virtual void stop_call(void *arg) {};

private:

    /**
     * @brief 定时器到期回调（在ISR中执行）
     * 调度 expiry_call，处理周期重复逻辑
     */
    void func_for_expiry() final;

    fml::atomic<void *> call_arg{nullptr}; // 用户参数（原子）
    fml::atomic<time_t> period{0};         // 周期时间（0=单次，原子）
    fml::atomic_bool is_scheduled{false};  // 是否已调度（原子）

    // 下次触发的绝对时间（只在ISR中访问，不需要原子）
    uint64_t next_call_time{0};
};
