cmake_minimum_required(VERSION 3.2)

project(bb-vulkan-engine)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src/engine)
file(GLOB_RECURSE SOURCES "src/engine/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

if (UNIX)
target_link_options(${PROJECT_NAME} PRIVATE -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi)
endif (UNIX)

target_compile_options(${PROJECT_NAME} PRIVATE -std=c++17 $<$<CONFIG:Debug>:-O0;-g3;-ggdb>)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>: DEBUG>)

add_subdirectory(lib/bb-net-lib)
target_link_libraries(${PROJECT_NAME} PRIVATE bbnetlib)

add_subdirectory(lib/glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

add_subdirectory(lib/glm)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(OpenGL_GL_PREFERENCE GLVND)


find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

