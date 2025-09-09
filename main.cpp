#include "file_reader.hpp"
#include <iostream>

int main() {
  try {
    HotReloadFileReader config(
        "/home/dav1ngen/my_documents/dav1ngen_common/config/config.yaml");
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
