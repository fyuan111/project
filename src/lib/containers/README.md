# FML Containers Library

独立的数据结构算法库，零依赖，可单独编译和测试。

## 目录结构

```
containers/
├── heap.hpp           # 最小堆（泛型）
├── time_ring.hpp      # 环形时间比较
└── tests/             # 单元测试
    ├── heap_test.cpp
    ├── time_ring_test.cpp
    ├── test_utils.hpp
    └── CMakeLists.txt
```

## 特性

### 1. MinHeap（最小堆）
- **泛型设计**：支持任意类型
- **自定义比较器**：可实现最小堆或最大堆
- **无状态**：所有方法都是静态的
- **用户管理内存**：传入数组指针，无动态分配
- **时间复杂度**：push/pop O(log n)，heapify O(n)

### 2. TimeRing（环形时间比较）
- **处理溢出**：支持 32/16/8 位计数器溢出
- **constexpr**：编译期计算
- **类型安全**：模板实现

## 编译和测试（Linux）

```bash
# 进入测试目录
cd fml/src/lib/containers/tests

# 编译
mkdir build && cd build
cmake ..
make

# 运行测试
./time_ring_test
./heap_test

# 或者使用 CTest
make test
```

## 使用示例

### MinHeap

```cpp
#include <lib/containers/heap.hpp>

// 最小堆（整数指针）
int* heap[10];
int size = 0;

// Push
fml::MinHeap<int*>::push(heap, size, new int(5), 10);
fml::MinHeap<int*>::push(heap, size, new int(3), 10);

// Pop（返回最小值）
int* min = fml::MinHeap<int*>::pop(heap, size);  // 3

// 自定义比较器
struct moment_t {
    uint32_t time;
    int priority;
};

auto cmp = [](moment_t* a, moment_t* b) {
    return a->time < b->time;
};

moment_t* task_heap[32];
int task_size = 0;
fml::MinHeap<moment_t*>::push(task_heap, task_size, m, 32, cmp);
```

### TimeRing

```cpp
#include <lib/containers/time_ring.hpp>

uint32_t now = 0xFFFFFFFE;     // 溢出前
uint32_t future = 0x00000002;  // 溢出后

// 环形比较
bool is_past = fml::TimeRing32::less(now, future);  // true

// 时间差
uint32_t diff = fml::TimeRing32::diff(now, future);  // 4
```

## 设计原则

1. **零依赖**：不依赖任何外部库或平台
2. **可测试**：在 Linux 上快速验证算法正确性
3. **高性能**：无虚函数，无动态分配
4. **类型安全**：模板和 constexpr
5. **可移植**：Zephyr/Linux/Bare-metal 通用

## 集成到 WorkQueue

```cpp
#include <lib/containers/heap.hpp>
#include <lib/containers/time_ring.hpp>

// 使用环形时间比较
auto time_cmp = [](moment_t* a, moment_t* b) {
    return fml::TimeRing32::less(a->moment, b->moment);
};

// 使用最小堆管理任务
moment_t* moments[32];
int heap_size = 0;

// Push 任务
fml::MinHeap<moment_t*>::push(moments, heap_size, task, 32, time_cmp);

// Pop 最近的任务
moment_t* next = fml::MinHeap<moment_t*>::pop(moments, heap_size, time_cmp);
```
