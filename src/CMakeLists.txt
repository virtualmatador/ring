cmake_minimum_required(VERSION 3.13)
add_library("ring" OBJECT "ring.hpp")
set_property(TARGET "ring" PROPERTY CXX_STANDARD 20)
target_include_directories("ring" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
