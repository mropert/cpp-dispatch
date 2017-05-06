cmake_minimum_required(VERSION 3.0)
project(cpp-dispatch)

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
   include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
   conan_basic_setup()
endif ()

add_library(cpp-dispatch INTERFACE)
target_include_directories(cpp-dispatch INTERFACE include)

source_group(include FILES include/dispatch/dispatch.hpp)

find_package(GTest)

if (GTest_FOUND)
	enable_testing()
	add_executable(cpp-dispatch-test test/dispatch.cpp)
	target_link_libraries(cpp-dispatch-test PRIVATE cpp-dispatch GTest::GTest GTest::Main)
endif ()
