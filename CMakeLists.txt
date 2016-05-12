cmake_minimum_required(VERSION 2.8.4)
project(Zookeeper)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++14")
find_library(HYPERVISOR_LIBRARY Hypervisor)
find_package(Boost 1.60.0 COMPONENTS regex)
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB zookeeper_SRC "*.cpp")
add_executable(zookeeper ${zookeeper_SRC})
target_link_libraries(zookeeper ${HYPERVISOR_LIBRARY} ${Boost_LIBRARIES})
