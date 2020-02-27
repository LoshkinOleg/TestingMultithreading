cmake_minimum_required(VERSION 3.14)
project(TestingMultithreading)

set(CMAKE_CXX_STANDARD 17)

add_executable(executable main.cpp)

target_link_libraries(executable pthread)

