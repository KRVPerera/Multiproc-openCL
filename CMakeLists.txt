cmake_minimum_required(VERSION 3.16)

project(MultiprocOpenCL
        VERSION 0.0.1
        LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
include_directories(include)
include_directories(${lodepng_SOURCE_DIR})
add_subdirectory(lib)

include_directories(${lodepng_SOURCE_DIR}/include)

set (Sources
        main.c
        ${lodepng_SOURCE_DIR}
        src/pngloader.c
)
#message(STATUS "lodepng_SOURCE_DIR: ${lodepng_SOURCE_DIR}")
set (Headers
        ${lodepng_SOURCE_DIR}
)

find_package(OpenCL REQUIRED)

add_executable(${PROJECT_NAME} ${Sources})

target_include_directories(${PROJECT_NAME} PUBLIC ${Headers})

target_link_libraries(${PROJECT_NAME} OpenCL::OpenCL lodepng)


enable_testing()

#target_link_libraries(tests Catch2::Catch2)

add_subdirectory(learning)

add_subdirectory(tests)
#endif()




