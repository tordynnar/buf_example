cmake_minimum_required(VERSION 3.16)
project(echo_server_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../cpp_package cpp_package_build)

add_executable(echo_server_cpp main.cpp)
target_link_libraries(echo_server_cpp PRIVATE demo_proto_bindings)
