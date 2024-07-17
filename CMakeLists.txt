cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(WordAdvisorLib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

add_subdirectory(extern/pybind11)
pybind11_add_module(WordAdvisor WordAdvisorWrapper.cpp)
add_executable(output WordAdvisor.cpp)
target_link_libraries(output PRIVATE Python3::Python)