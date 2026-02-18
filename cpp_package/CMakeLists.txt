cmake_minimum_required(VERSION 3.16)
project(demo_proto_bindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

file(GLOB_RECURSE PROTO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/gen/*.cc")

add_library(demo_proto_bindings STATIC ${PROTO_SOURCES})

target_include_directories(demo_proto_bindings PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)

target_link_libraries(demo_proto_bindings PUBLIC
    protobuf::libprotobuf
    gRPC::grpc++
    gRPC::grpc++_reflection
)
