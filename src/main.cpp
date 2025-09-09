#include <cassert>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <thread>

#include "file_reader.hpp"

int main() {
  // FileReader usage:
  // FileReader file_reader("CONFIG_FILE_PATH");
  // T parameter = file_reader.Read<T>("PARAMETER_NAME");
  FileReader file_reader(std::string(CONFIG_DIR) + "config.yaml");

  SPDLOG_INFO("==================FileReader Test===============");
  std::string string_parameter1 =
      file_reader.Read<std::string>("string_parameter");
  int int_parameter1 = file_reader.Read<int>("int_parameter");
  bool bool_parameter1 = file_reader.Read<bool>("bool_parameter");
  double double_parameter1 = file_reader.Read<double>("double_parameter");
  float float_parameter1 = file_reader.Read<float>("float_parameter");

  std::cout << "string_parameter: " << string_parameter1 << "\n"
            << "int_parameter" << int_parameter1 << "\n"
            << "bool_parameter" << bool_parameter1 << "\n"
            << "double_parameter" << double_parameter1 << "\n"
            << "float_parameter" << float_parameter1 << "\n";

  // HotReloadFileReader usage:
  SPDLOG_INFO("==================HotReloadFileReader Test===============");
  HotReloadFileReader hot_reload_file_reader(std::string(CONFIG_DIR) +
                                             "config.yaml");
  hot_reload_file_reader.Start();
  std::string string_parameter2;
  int int_parameter2;
  bool bool_parameter2;
  double double_parameter2;
  float float_parameter2;

  std::thread t([&hot_reload_file_reader, &string_parameter2, &int_parameter2,
                 &bool_parameter2, &double_parameter2, &float_parameter2]() {
    while (true) {
      string_parameter2 =
          hot_reload_file_reader.Read<std::string>("string_parameter");
      int_parameter2 = hot_reload_file_reader.Read<int>("int_parameter");
      bool_parameter2 = hot_reload_file_reader.Read<bool>("bool_parameter");
      double_parameter2 =
          hot_reload_file_reader.Read<double>("double_parameter");
      float_parameter2 = hot_reload_file_reader.Read<float>("float_parameter");

      sleep(1);
    }
  });
  t.detach();

  while (true) {
    std::cout << "string_parameter: " << string_parameter2 << "\n"
              << "int_parameter" << int_parameter2 << "\n"
              << "bool_parameter" << bool_parameter2 << "\n"
              << "double_parameter" << double_parameter2 << "\n"
              << "float_parameter" << float_parameter2 << "\n";
    sleep(1);
  }

  return 0;
}
