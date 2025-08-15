cmake_minimum_required(VERSION 3.16)
project(HotReloadDemo)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找线程库
find_package(Threads REQUIRED)

# 查找OpenCV库
find_package(OpenCV REQUIRED)

# 包含OpenCV头文件目录
include_directories(${OpenCV_INCLUDE_DIRS})

# 添加可执行文件
add_executable(hot_reload_demo main.cpp)

# 链接库
target_link_libraries(hot_reload_demo 
    Threads::Threads
    ${OpenCV_LIBS}
)

# 编译选项
target_compile_options(hot_reload_demo PRIVATE -Wall -Wextra)

# 如果是Debug模式，添加调试信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(hot_reload_demo PRIVATE -g)
endif()