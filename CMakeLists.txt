###########################
### 1.Set CMake options ###
###########################
cmake_minimum_required(VERSION 3.14)
project(cpptool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
  message(STATUS "Build type not specified, defaulting to Debug")
endif()

# Set sanitizers options
option(ENABLE_SANITIZER_ADDRESS "Enable AddressSanitizer" OFF)
option(ENABLE_SANITIZER_UNDEFINED "Enable UndefinedBehaviorSanitizer" OFF)
option(ENABLE_SANITIZER_THREAD "Enable ThreadSanitizer" OFF)


#############################
### 2.Set project options ###
#############################
find_package(OpenCV REQUIRED)
find_package(fmt REQUIRED)

add_definitions(-DCONFIG_DIR="${CMAKE_SOURCE_DIR}/config/")

file(GLOB SRC
  ${CMAKE_SOURCE_DIR}/src/*.c
  ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(
  ${PROJECT_NAME}
  ${SRC}
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  ${OPENCV_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(
  ${PROJECT_NAME}
  ${OpenCV_LIBS}
  fmt::fmt
)


############################
### 3.Debug mode options ###
############################
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Configuring Debug build")

  # Close optimization and add debug symbols
  target_compile_options(
    ${PROJECT_NAME}
    PRIVATE
    -O0 -g
  )

  function(enable_sanitizers target)
    # Reserve frame pointer and add debug symbols
    target_compile_options(
      ${target}
      PRIVATE
      -fno-omit-frame-pointer
      -g
    )

    if(ENABLE_SANITIZER_ADDRESS)
      target_compile_options(${target} PRIVATE -fsanitize=address)
      target_link_options(${target} PRIVATE -fsanitize=address)
    endif()

    if(ENABLE_SANITIZER_UNDEFINED)
      target_compile_options(${target} PRIVATE -fsanitize=undefined)
      target_link_options(${target} PRIVATE -fsanitize=undefined)
    endif()

    if(ENABLE_SANITIZER_THREAD)
      target_compile_options(${target} PRIVATE -fsanitize=thread)
      target_link_options(${target} PRIVATE -fsanitize=thread)
    endif()
  endfunction()
  enable_sanitizers(${PROJECT_NAME})
endif()


##############################
### 4.Release mode options ###
##############################
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  message(STATUS "Configuring Release build")

  # Release mode use O3 optimization
  target_compile_options(
    ${PROJECT_NAME}
    PRIVATE
    -O3 -march=native -DNDEBUG
  )
endif()