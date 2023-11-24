cmake_minimum_required(VERSION 3.10)

project(pipeline-task VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(pipeline-task main.cpp)

# directory where the executable will be placed
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
