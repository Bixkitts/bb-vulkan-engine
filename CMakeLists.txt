cmake_minimum_required(VERSION 3.0)

project(Opengl-test)

include_directories(src)
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(glTest ${SOURCES})

target_link_options(glTest PRIVATE -pthread)
target_compile_options(glTest PRIVATE -g)
