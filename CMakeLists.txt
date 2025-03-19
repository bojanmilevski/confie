cmake_minimum_required(VERSION 3.31)
project(confie VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

if(DEBUG)
  add_definitions(-DDEBUG)
endif()

add_subdirectory(src)
