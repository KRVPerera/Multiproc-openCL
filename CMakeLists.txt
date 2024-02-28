cmake_minimum_required(VERSION 3.16)

project(MultiprocOpenCL
        VERSION 0.0.1
        LANGUAGES C CXX)

find_package(OpenCL REQUIRED)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wno-deprecated-declarations")

add_subdirectory(lib)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

include_directories(include/pngloader)
include_directories(include/common)


#add_compile_definitions(PROJECT_DIR_ABS_PATH ${PROJECT_SOURCE_DIR})

add_library(img_loader STATIC src/pngloader/pngloader.c include/pngloader/pngloader.h)
target_link_libraries(img_loader PUBLIC lodepng)

configure_file(config.h.in config.h)
add_executable(${PROJECT_NAME} main.c config.h)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} OpenCL::OpenCL img_loader zncc_c)

enable_testing()

add_test(
        NAME ${PROJECT_NAME}
        COMMAND ${PROJECT_NAME}
)

add_subdirectory(learning)
add_subdirectory(tests)
add_subdirectory(project)