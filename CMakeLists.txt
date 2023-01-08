CMAKE_MINIMUM_REQUIRED(VERSION 3.3)
project(bb-vulkan-engine 
    DESCRIPTION "A shared object library for rendering shaders in a window"
    VERSION 0.2
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(testbin)
add_library(${PROJECT_NAME} SHARED)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(testbin PRIVATE ${PROJECT_NAME})

add_subdirectory(src)

if (UNIX)
install(TARGETS ${PROJECT_NAME} DESTINATION /usr/lib)
install(FILES include/bb-engine.h DESTINATION include)

target_link_options(${PROJECT_NAME} PRIVATE -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi)
endif (UNIX)

target_compile_options(${PROJECT_NAME} PRIVATE -std=c++17 -fvisibility=hidden $<$<CONFIG:Debug>:-O0;-g3;-ggdb>)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>: DEBUG>)

add_subdirectory(lib/bb-net-lib)
target_link_libraries(${PROJECT_NAME} PRIVATE bbnetlib)

add_subdirectory(lib/glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(testbin PRIVATE glfw)

add_subdirectory(lib/glm)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(OpenGL_GL_PREFERENCE GLVND)


find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

