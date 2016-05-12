cmake_minimum_required(VERSION 2.8.4)
project(Zookeeper)

find_library(HYPERVISOR_LIBRARY Hypervisor)
find_package(Boost 1.60.0 COMPONENTS regex)
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB zookeeper_SRC "*.cpp")
add_executable(zookeeper
        Box.cpp
        Cpu.cpp
        Debugger.cpp
        Hypercall.cpp
        HypercallDispatch.cpp
        IOManager.cpp
        main.cpp
        PageManager.cpp
        ThreadManager.cpp
        Xbe.cpp
)
target_link_libraries(zookeeper ${HYPERVISOR_LIBRARY} ${Boost_LIBRARIES})
set_target_properties(zookeeper PROPERTIES COMPILE_FLAGS "-std=c++14")

add_subdirectory(NightBeliever)

configure_file(test1.xbe ${CMAKE_CURRENT_BINARY_DIR}/test1.xbe COPYONLY)