#define HOTRELOAD_ONLY
#include "file_reader.hpp"
#include <iostream>

int main() {
  try {
    // 创建配置文件读取器
    HotReloadFileReader config(
        "/home/dav1ngen/my_documents/dav1ngen_common/config/config.yaml");

    // 启动配置加载
    config.Start();

    std::cout << "=== 配置文件内容 ==="
              << "\n";

    while (true) {
      std::string app_name = config.GetValue("app_name");
      std::cout << app_name << "\n";
    }

    config.Stop();

  } catch (const std::exception &e) {
    std::cerr << "程序运行出错: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
