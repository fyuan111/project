# 工作队列延迟性能测试

## 测试目的

测量 FML 工作队列系统的调度延迟，包括：
1. ISR 执行时间
2. 线程调度延迟（ISR结束到任务执行）
3. 总延迟（ISR触发到任务执行）

## 构建和运行

### 方法1：使用 west（推荐）

```bash
# 进入 zephyr 环境
cd ~/zephyrproject
source .venv/bin/activate

# 构建测试（替换为你的开发板）
west build -p -b your_board_name fml/test/workqueue_latency

# 烧录
west flash

# 查看串口输出
minicom -D /dev/ttyUSB0 -b 115200
```

### 方法2：使用你的 flash.sh 脚本

```bash
# 修改 flash.sh 支持测试目录
./flash.sh -d fml/test/workqueue_latency
```

## 预期输出

```
=== 工作队列延迟测试初始化 ===
Running TESTSUITE workqueue_latency
===================================================================
START - test_basic_latency

=== 测试：基础调度延迟 ===

=== 延迟测试结果 (样本数: 100) ===

ISR 执行时间:
  平均: 2 us
  最小: 2 us
  最大: 3 us
  抖动: 1 us

线程调度延迟 (ISR结束 → 任务执行):
  平均: 10 us
  最小: 9 us
  最大: 11 us
  抖动: 2 us

总延迟 (ISR触发 → 任务执行):
  平均: 12 us
  最小: 11 us
  最大: 13 us
  抖动: 2 us

✅ 所有性能指标测试通过！

 PASS - test_basic_latency in 1.520 seconds
===================================================================
TESTSUITE workqueue_latency succeeded
```

## 性能基准

| 指标 | 目标值 | 说明 |
|------|--------|------|
| ISR 执行 | < 5 us | 中断服务例程执行时间 |
| 调度延迟 | < 15 us | 线程切换和调度开销 |
| 总延迟 | < 20 us | 端到端响应时间 |
| ISR 抖动 | < 5 us | ISR 执行时间稳定性 |
| 调度抖动 | < 10 us | 调度延迟稳定性 |

## 调整测试参数

编辑 `src/test_latency.cpp`：

```cpp
// 修改采样数量
#define SAMPLE_COUNT 100  // 改为 200, 500 等

// 修改任务周期
LatencyTestItem() : WorkItem(0, 10) {}  // 10ms 改为其他值
```

## 故障排查

1. **编译失败**：检查 CMakeLists.txt 中的路径是否正确
2. **测试未运行**：确保 `CONFIG_ZTEST=y` 在 prj.conf 中
3. **延迟异常高**：检查系统负载、中断优先级配置
