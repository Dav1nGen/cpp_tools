#include "file_reader.hpp"

namespace FileReaderTest {
void TestFileReader() {
  // FileReader usage:
  // FileReader file_reader("CONFIG_FILE_PATH");
  // T parameter = file_reader.Read<T>("PARAMETER_NAME");
  FileReader file_reader(std::string(CONFIG_DIR) + "config.yaml");

  SPDLOG_INFO("==================FileReader Test===============");
  std::string string_parameter =
      file_reader.Read<std::string>("string_parameter");
  int int_parameter = file_reader.Read<int>("int_parameter");
  bool bool_parameter = file_reader.Read<bool>("bool_parameter");
  double double_parameter = file_reader.Read<double>("double_parameter");
  float float_parameter = file_reader.Read<float>("float_parameter");

  std::cout << "string_parameter: " << string_parameter << "\n"
            << "int_parameter" << int_parameter << "\n"
            << "bool_parameter" << bool_parameter << "\n"
            << "double_parameter" << double_parameter << "\n"
            << "float_parameter" << float_parameter << "\n";
}
}  // namespace FileReaderTest
