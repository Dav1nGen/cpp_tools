#include "file_reader.hpp"

#include "FileReader_test.hpp"
#include "HotReloadFileReader_test.hpp"

int main() {
  FileReaderTest::TestFileReader();
  HotReloadFileReaderTest::TestHotReloadFileReader();
  return 0;
}
