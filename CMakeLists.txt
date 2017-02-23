cmake_minimum_required (VERSION 2.8.1)

project (proto-gen-proto2ts)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -W -Wall -Wextra -Weffc++")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

include_directories(
  ${CMAKE_SOURCE_DIR}/include
  )

add_executable (protoc-gen-proto2ts
  ${CMAKE_SOURCE_DIR}/src/main.cpp
  )

target_link_libraries (protoc-gen-proto2ts
 protoc
  protobuf
  )
