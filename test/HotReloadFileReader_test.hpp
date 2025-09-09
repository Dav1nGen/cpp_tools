#include "file_reader.hpp"

namespace HotReloadFileReaderTest {
void TestHotReloadFileReader() {
  // HotReloadFileReader usage:
  SPDLOG_INFO("==================HotReloadFileReader Test===============");
  HotReloadFileReader hot_reload_file_reader(std::string(CONFIG_DIR) +
                                             "config.yaml");
  hot_reload_file_reader.Start();
  std::string string_parameter;
  int int_parameter;
  bool bool_parameter;
  double double_parameter;
  float float_parameter;

  std::thread t([&hot_reload_file_reader, &string_parameter, &int_parameter,
                 &bool_parameter, &double_parameter, &float_parameter]() {
    while (true) {
      string_parameter =
          hot_reload_file_reader.Read<std::string>("string_parameter");
      int_parameter = hot_reload_file_reader.Read<int>("int_parameter");
      bool_parameter = hot_reload_file_reader.Read<bool>("bool_parameter");
      double_parameter =
          hot_reload_file_reader.Read<double>("double_parameter");
      float_parameter = hot_reload_file_reader.Read<float>("float_parameter");

      sleep(1);
    }
  });
  t.detach();

  while (true) {
    std::cout << "string_parameter: " << string_parameter << "\n"
              << "int_parameter" << int_parameter << "\n"
              << "bool_parameter" << bool_parameter << "\n"
              << "double_parameter" << double_parameter << "\n"
              << "float_parameter" << float_parameter << "\n";
    sleep(1);
  }
}
}  // namespace HotReloadFileReaderTest
