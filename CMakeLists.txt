cmake_minimum_required(VERSION 3.16)

project(MultiprocOpenCL
        VERSION 0.1.0
        LANGUAGES C CXX)

find_package(OpenCL REQUIRED)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wno-deprecated-declarations -pedantic -Wno-newline-eof -Wno-strict-prototypes")
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
#
#target_compile_definitions(catch-test PRIVATE CATCH_CONFIG_MAIN CATCH_CONFIG_FAST_COMPILE CATCH_CONFIG_DISABLE_MATCHERS)
#target_precompile_headers(catch-test PRIVATE Catch2/single_include/catch2/catch.hpp)

configure_file(config.h.in config.h)

include_directories(include)

add_library(utils STATIC src/util.c include/util.h)
add_library(img_loader STATIC src/pngloader.c include/pngloader.h)
target_link_libraries(img_loader PUBLIC lodepng)

add_executable(${PROJECT_NAME} main.c src/driver.c src/zncc_c_imp.c src/cross_checking.c src/occlusion_filling.c)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} OpenCL::OpenCL img_loader utils)

enable_testing()

add_subdirectory(lib)
add_subdirectory(learning)
add_subdirectory(tests)
add_subdirectory(project)