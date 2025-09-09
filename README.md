# C++ 通用工具库

本仓库包含通用的C++工具模块。

目前包含配置读取器和线程通信器（功能持续更新以及添加）。

## 1. 配置读取器 (`include/file_reader.hpp`)

该头文件提供了三个类用于文件读写和配置管理。

### `FileReader`

一个基于OpenCV `FileStorage` 的简单文件读取器，适用于读取YAML, XML等格式的配置文件。

**使用方法:**

假设有一个 `config.yaml` 文件（首行YAML文件标识符必须加，否则cv::FileStorage将无法识别YAML文件并导致运行期报错），内容如下:
```yaml
%YAML:1.0
---
string_parameter: "hello_world"
int_parameter: 123
```

可以通过以下方式读取：
```cpp
#include "file_reader.hpp"
#include <string>
#include <iostream>

int main() {
    try {
        FileReader reader("config.yaml");
        std::string str_val = reader.Read<std::string>("string_parameter");
        int int_val = reader.Read<int>("int_parameter");

        std::cout << "String Value: " << str_val << std::endl;
        std::cout << "Int Value: " << int_val << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
```
目前支持的数据类型有：`int`, `float`, `double`, `bool`, `std::string`。

### `FileWriter`

与 `FileReader` 配套使用，可以将键值对写入到文件中。

### `HotReloadFileReader`

一个支持热重载的配置文件读取器。它会启动一个后台线程，监控配置文件的修改时间。一旦文件发生变化，它会自动重新加载配置，使得应用程序可以在不重启的情况下应用新的配置。

建议在需要频繁修改参数的项目调试场景下使用，可以减少程序程序重新编译以及重启花费的时间。

**使用方法:**

该示例展示了如何启动热加载功能，并在主循环中持续获取最新的配置值。
```cpp
#include "file_reader.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    // 指定要监控的配置文件
    HotReloadFileReader hot_reader("config.yaml");
    
    // 启动监控线程
    hot_reader.Start();

    std::string string_parameter =
      file_reader.Read<std::string>("string_parameter");

    int int_parameter = file_reader.Read<int>("int_parameter");

    std::thread t([&hot_reload_file_reader, &string_parameter，int_parameter]() {
    while (true) {
      string_parameter =
          hot_reload_file_reader.Read<std::string>("string_parameter");
      int_parameter = hot_reload_file_reader.Read<int>("int_parameter");
      sleep(1);
    }
  });

  t.detach();

  while (true) {
  std::cout << "string_parameter: " << string_parameter2 << "\n"
            << "int_parameter" << int_parameter2 << "\n"；
  sleep(1);
  }
    return 0;
}
```
其中Read模板函数的使用方法与 `FileReader` 类似，但是需要开一个新线程不断循环读取以获取最新配置参数。