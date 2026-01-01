# FML Applications

这个目录包含所有的应用模块。

## 如何添加新模块

### 1. 创建模块目录

```bash
mkdir src/Applications/my_app
```

### 2. 创建模块代码

`src/Applications/my_app/my_app.cpp`:

```cpp
#include <platform/common/apps.hpp>
#include <platform/log.h>

class MyApp : public AppBase<MyApp, work_queue_level::none>
{
public:
    void run() override
    {
        fml_info("MyApp is running!");
        while (!should_exit()) {
            // 业务逻辑
            usleep(1000000);
        }
    }

    static int print_usage(const char *reason = nullptr)
    {
        fml_info("Usage: my_app [start|stop|status]");
        return 0;
    }
};

// 模块入口函数
FML_REGISTER_APP(my_app)
```

### 3. 创建 CMakeLists.txt

`src/Applications/my_app/CMakeLists.txt`:

```cmake
fml_register(
    MAIN my_app
    SRCS my_app.cpp
)
```

### 4. 完成！

不需要修改任何其他文件，CMake 会自动发现新模块！

## 编译

### Linux 平台:
```bash
mkdir build && cd build
cmake ..
make
./bin/fml_main my_app start
```

### Zephyr 平台:
```bash
west build -b nrf52840dk_nrf52840
west flash
# 在终端输入: my_app start
```

## 模块列表

- `hello_world` - Hello World 示例
- 添加你的模块...
