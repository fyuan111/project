#include <drivers/hrtimer/hrt.hpp>
#include <zephyr/kernel.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <platform/log.h>
#include <fml_config/hrt.h>

// 静态成员定义
const struct device *hrt::counter_dev = nullptr;

void on_expiry(const struct device *dev, uint8_t chan_id,
               uint32_t ticks, void *user_data)
{
    hrt *obj = static_cast<hrt *>(user_data);

    if (obj)
        obj->func_for_expiry();
}

bool hrt::set_alarm(uint32_t delay_us)
{
    auto *counter_dev = static_cast<const struct device *>(dev.z_dev);
    uint32_t ticks = counter_us_to_ticks(counter_dev, delay_us);

    struct counter_alarm_cfg cfg = {
        .callback = reinterpret_cast<counter_alarm_callback_t>(alarm_cache.callback_ptr),
        .ticks = ticks,
        .user_data = alarm_cache.user_data,
        .flags = 0};

    return (counter_set_channel_alarm(counter_dev, dev.channel_id, &cfg) == 0);
}

bool hrt::set_alarm_at(uint64_t absolute_us)
{
    auto *counter_dev = static_cast<const struct device *>(dev.z_dev);

    // 获取当前时间（ticks）
    uint32_t current_ticks;
    counter_get_value(counter_dev, &current_ticks);

    uint32_t target_ticks = counter_us_to_ticks(counter_dev, absolute_us);

    // 如果目标时间已经过去，设置最小延迟（立即触发）
    uint32_t delay_ticks = (target_ticks > current_ticks) ? (target_ticks - current_ticks) : 1;

    struct counter_alarm_cfg cfg = {
        .callback = reinterpret_cast<counter_alarm_callback_t>(alarm_cache.callback_ptr),
        .ticks = delay_ticks,
        .user_data = alarm_cache.user_data,
        .flags = 0};

    return (counter_set_channel_alarm(counter_dev, dev.channel_id, &cfg) == 0);
}

void hrt::set_alarm_at_for_isr(uint32_t absolute_ticks, uint32_t ticks)
{
    auto *counter_dev = static_cast<const struct device *>(dev.z_dev);

    struct counter_alarm_cfg cfg = {
        .callback = reinterpret_cast<counter_alarm_callback_t>(alarm_cache.callback_ptr),
        .ticks = (absolute_ticks - ticks),
        .user_data = alarm_cache.user_data,
        .flags = 0};

    counter_set_channel_alarm(counter_dev, dev.channel_id, &cfg);

}

bool hrt::init(uint8_t channel)
{
    if (!check_hrt() || dev.z_dev == nullptr)
    {
        fml_error("[HRT] check_hrt failed\n");
        return false;
    }

    f_mutex lock(&hrt_mutex);

    counter_dev = static_cast<const struct device *>(dev.z_dev);

    if (counter_start(counter_dev) != 0)
    {
        fml_error("[HRT] Failed to start counter\n");
        return false;
    }

    dev.freq = counter_get_frequency(counter_dev);
    dev.channel_id = channel;

    // 缓存不变的 alarm 参数，减少 set_alarm 开销
    alarm_cache.callback_ptr = reinterpret_cast<void *>(on_expiry);
    alarm_cache.user_data = this;

    device_ok.store(true); // 初始化成功，设置可用标志

    return true;
}

int hrt::stop()
{
    if (!dev.z_dev)
        return -1;
    auto *counter_dev = static_cast<const struct device *>(dev.z_dev);
    device_ok.store(false);
    return counter_stop(counter_dev);
}

int hrt::start()
{
    if (!dev.z_dev)
        return -1;

    auto *counter_dev = static_cast<const struct device *>(dev.z_dev);
    int ret = counter_start(counter_dev);

    if (ret == 0)
    {
        device_ok.store(true); // 启动成功，恢复可用状态
    }

    return ret;
}

bool hrt::check_hrt()
{
    const struct device *counter_dev = DEVICE_DT_GET(DT_CHILD(DT_NODELABEL(time_tR_NODE), counter));

    if (!device_is_ready(counter_dev))
    {
        fml_error("[HRT] Device not ready\n");
        return false;
    }

    dev.z_dev = (void *)counter_dev;
    return true;
}

uint64_t hrt::get_current_us()
{
    auto *counter_dev = static_cast<const struct device *>(dev.z_dev);
    uint32_t ticks;
    counter_get_value(counter_dev, &ticks);

    return counter_ticks_to_us(counter_dev, ticks);
}

uint64_t hrt::get_current_ns()
{
    auto *counter_dev = static_cast<const struct device *>(dev.z_dev);
    uint32_t ticks;
    counter_get_value(counter_dev, &ticks);

    return counter_ticks_to_ns(counter_dev, ticks);
}

uint32_t hrt::get_current_ticks()
{
    auto *counter_dev = static_cast<const struct device *>(dev.z_dev);
    uint32_t ticks;
    counter_get_value(counter_dev, &ticks);
    return ticks;
}

uint64_t hrt::get_current_ticks_64()
{
    auto *counter_dev = static_cast<const struct device *>(dev.z_dev);
    uint64_t ticks = 0;
    int ret = counter_get_value_64(counter_dev, &ticks);

    if (ret == -ENOSYS) {
        // 驱动不支持 64位，回退到 32位
        uint32_t ticks_32 = 0;
        counter_get_value(counter_dev, &ticks_32);
        return static_cast<uint64_t>(ticks_32);
    } else if (ret != 0) {
        fml_error("[HRT] counter_get_value_64 failed: %d\n", ret);
        return 0;
    }

    return ticks;
}

bool hrt::cancel_alarm()
{
    if (!device_ok.load())
        return false;

    auto *counter_dev = static_cast<const struct device *>(dev.z_dev);
    int ret = counter_cancel_channel_alarm(counter_dev, dev.channel_id);

    if (ret != 0)
    {
        return false;
    }

    return true;
}

void printf_hrt_info(hrt *timer)
{
    if (timer == nullptr || !timer->is_available())
        return;

    fml_info("HRT info: available=%s", timer->is_available() ? "yes" : "no");
}