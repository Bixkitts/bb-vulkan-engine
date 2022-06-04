cmake_minimum_required(VERSION 3.2)

project(Opengl-test)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src)
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(glTest ${SOURCES})

target_link_options(glTest PRIVATE -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi)
target_compile_options(glTest PRIVATE -std=c++17 -O2)

add_subdirectory(lib/bb-net-lib)
target_link_libraries(glTest PRIVATE bbnetlib)

add_subdirectory(lib/glfw)
target_link_libraries(glTest PRIVATE glfw)

add_subdirectory(lib/glm)
target_link_libraries(glTest PRIVATE glm)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)


find_package(OpenGL REQUIRED)
target_link_libraries(glTest PRIVATE OpenGL::GL)

