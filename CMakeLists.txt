cmake_minimum_required(VERSION 3.13)
project(CaramelVM)

set(CMAKE_CXX_STANDARD 17)

add_executable(CaramelVM main.cc thirdparty/fmt/format.cc thirdparty/fmt/posix.cc)