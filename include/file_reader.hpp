/**
 * @file file_reader.hpp
 * @author Dav1nGen (davicheng1114@gmail.com)
 * @brief
 * @version 0.2
 * @date 2025-08-14
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
// C++ standard library header file
#include <assert.h>
#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

// Third-party library headers
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/core/persistence.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <spdlog/spdlog.h>

class FileReader {
 public:
  /**
   * @brief Construct a new file reader object
   *
   * @param File_ path
   */
  explicit FileReader(const std::string& file_path)
      : file_path_(file_path), is_open_(false) {
    fs_.open(file_path_, cv::FileStorage::READ);
    if (!fs_.isOpened()) {
      std::string error_msg = "File \"" + file_path + "\" open failed.";
      std::cerr << error_msg << "\n";
      throw std::runtime_error(error_msg);
    }
    is_open_ = true;
  }

  /**
   * @brief Destroy the file reader object
   *
   */
  ~FileReader() {
    if (is_open_ && fs_.isOpened()) {
      fs_.release();
    }
  }

  /**
   * @brief Read the value of the specified key from the file.
   *
   * @tparam T
   * @param key
   * @return T
   */
  template <typename T>
  T Read(const std::string& key) {
    if (!is_open_ || !fs_.isOpened()) {
      throw std::runtime_error("FileReader is not open");
    }

    const char* cstr = key.c_str();
    if (fs_[cstr].empty()) {
      throw std::runtime_error("Key: \"" + key + "\" not found in the file.");
    }
    T value;
    fs_[cstr] >> value;
    return value;
  }

 private:
  const std::string file_path_;
  cv::FileStorage fs_;
  bool is_open_ = false;
};

class FileWriter {
 public:
  /**
   * @brief Construct a new file writer object
   *
   * @param file_path
   */
  explicit FileWriter(const std::string& file_path)
      : file_path_(file_path), is_open_(false) {
    fs_.open(file_path_, cv::FileStorage::WRITE);
    if (!fs_.isOpened()) {
      std::string error_msg = "File \"" + file_path + "\" open failed.";
      SPDLOG_ERROR("File {} open failed",file_path_);
      throw std::runtime_error(error_msg);
    }
    is_open_ = true;
  }

  /**
   * @brief Destroy the file writer object
   *
   */
  ~FileWriter() {
    if (is_open_ && fs_.isOpened()) {
      fs_.release();
    }
  }

  /**
   * @brief Write the value with the specified key to the file.
   *
   * @tparam T
   * @param key
   * @param value
   */
  template <typename T>
  void Write(const std::string& key, const T& value) {
    if (!is_open_ || !fs_.isOpened()) {
      throw std::runtime_error("FileWriter is not open");
    }

    fs_ << key << value;
  }

 private:
  cv::FileStorage fs_;
  std::string file_path_;
  bool is_open_ = false;
};

class HotReloadFileReader {
 public:
  explicit HotReloadFileReader(const std::string& file_path,
                               std::chrono::milliseconds check_interval =
                                   std::chrono::milliseconds(1000))
      : file_path_(file_path),
        check_interval_(check_interval),
        running_(false) {}

  ~HotReloadFileReader() { Stop(); }

  void Start() {
    if (running_.load()) {
      return;
    }
    running_.store(true);
    LoadConfig();
    monitor_thread_ =
        std::thread(&HotReloadFileReader::MonitorFileChanges, this);
  }

  void Stop() {
    if (running_.load()) {
      running_.store(false);
      if (monitor_thread_.joinable()) {
        monitor_thread_.join();
      }
    }
  }

  template <typename T>
  T Read(const std::string& key) const {
    std::lock_guard<std::mutex> lock(config_mutex_);
    auto it = config_cache_.find(key);
    if (it == config_cache_.end()) {
      throw std::runtime_error("Key '" + key + "' not found in config");
    }
    return ParseValue<T>(it->second);
  }

 private:
  template <typename T>
  T ParseValue(const std::string& value) const {
    if constexpr (std::is_same_v<T, std::string>) {
      if (value.front() == '"' && value.back() == '"') {
        return value.substr(1, value.length() - 2);
      }
      return value;
    } else if constexpr (std::is_same_v<T, bool>) {
      std::string lower_value = value;
      std::transform(lower_value.begin(), lower_value.end(),
                     lower_value.begin(), ::tolower);
      return lower_value == "true" || lower_value == "1";
    } else if constexpr (std::is_same_v<T, int>) {
      return std::stoi(value);
    } else if constexpr (std::is_same_v<T, float>) {
      return std::stof(value);
    } else if constexpr (std::is_same_v<T, double>) {
      return std::stod(value);
    } else {
      static_assert(std::is_same_v<T, void>, "Unsupported type");
      return T{};
    }
  }

  void LoadConfig() {
    try {
      std::lock_guard<std::mutex> lock(config_mutex_);

      if (!std::filesystem::exists(file_path_)) {
        std::cerr << "Warning: Config file does not exist: " << file_path_
                  << std::endl;
        return;
      }

      std::ifstream file(file_path_);
      if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file: " << file_path_
                  << std::endl;
        return;
      }

      config_cache_.clear();
      std::string line;
      while (std::getline(file, line)) {
        // 跳过注释和空行
        if (line.empty() || line[0] == '#' || line.find("---") == 0 ||
            line.find("%YAML") == 0) {
          continue;
        }

        // 查找冒号分隔符
        size_t colon_pos = line.find(':');
        if (colon_pos == std::string::npos) {
          continue;
        }

        std::string key = line.substr(0, colon_pos);
        std::string value = line.substr(colon_pos + 1);

        // 去除前后空格
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);

        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        if (!key.empty() && !value.empty()) {
          config_cache_[key] = value;
        }
      }

      last_write_time_ = std::filesystem::last_write_time(file_path_);
      // std::cout << "Config file reloaded: \n  " << file_path_ << std::endl;
      SPDLOG_INFO("Config file reloaded: {}",file_path_);
    } catch (const std::exception& e) {
      std::cerr << "Error loading config: " << e.what() << std::endl;
    }
  }

  void MonitorFileChanges() {
    while (running_.load()) {
      try {
        if (std::filesystem::exists(file_path_)) {
          auto current_write_time =
              std::filesystem::last_write_time(file_path_);
          if (current_write_time != last_write_time_) {
            LoadConfig();
          }
        }
      } catch (const std::exception& e) {
        std::cerr << "Error monitoring file: " << e.what() << std::endl;
      }

      std::this_thread::sleep_for(check_interval_);
    }
  }

  std::string file_path_;
  std::chrono::milliseconds check_interval_;
  std::atomic<bool> running_;
  std::thread monitor_thread_;

  mutable std::mutex config_mutex_;
  std::unordered_map<std::string, std::string> config_cache_;
  std::filesystem::file_time_type last_write_time_;
};
