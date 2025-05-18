# CMake 最低版本要求
cmake_minimum_required(VERSION 3.22)

# 项目名称
project(ParallenAlgorithm)

# 设置C++ 标准
set(CMAKE_CXX_STANDARD 20)

# 指定可执行文件的输出目录（绝对路径）
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 查找所有源文件
file(GLOB SOURCES "src/*.cpp")

# 查找所有头文件
file(GLOB HEADERS "include/*.h")

# 添加可执行文件
add_executable(ParallenAlgorithm ${SOURCES} ${HEADERS})

# 添加头文件搜索路径
target_include_directories(ParallenAlgorithm PUBLIC include)

