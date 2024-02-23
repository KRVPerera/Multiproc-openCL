cmake_minimum_required(VERSION 3.16)

project(MultiprocOpenCL
        VERSION 0.0.1
        LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

add_subdirectory(lib)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

message("lodepng_SOURCE_DIR: ${lodepng_SOURCE_DIR}")
include_directories(include)
include_directories(${lodepng_SOURCE_DIR})

set (Sources main.c src/pngloader.c)

set (Headers)

find_package(OpenCL REQUIRED)

add_executable(${PROJECT_NAME} ${Sources})

target_link_libraries(${PROJECT_NAME} OpenCL::OpenCL lodepng)
target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)

enable_testing()

add_subdirectory(learning)
add_subdirectory(tests)




