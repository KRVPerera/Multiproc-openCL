cmake_minimum_required(VERSION 3.16)

project(MultiprocOpenCL
        VERSION 0.1.0
        LANGUAGES C CXX)

find_package(OpenCL REQUIRED)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wno-deprecated-declarations -pedantic")
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

#
#target_compile_definitions(catch-test PRIVATE CATCH_CONFIG_MAIN CATCH_CONFIG_FAST_COMPILE CATCH_CONFIG_DISABLE_MATCHERS)
#target_precompile_headers(catch-test PRIVATE Catch2/single_include/catch2/catch.hpp)

configure_file(config.h.in config.h)

include_directories(include)
include_directories(include/opencl)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(utils STATIC src/util.c include/util.h)
add_library(img_loader STATIC src/pngloader.c include/pngloader.h)
target_link_libraries(img_loader PUBLIC lodepng)

add_library(logger STATIC src/logger.c include/logger.h)

add_executable(${PROJECT_NAME} main.c src/driver.c src/zncc_c_imp.c src/cross_checking.c src/occlusion_filling.c)
#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} OpenCL::OpenCL img_loader utils opencl_flow_ex3_lib logger)


add_test(
        NAME ${PROJECT_NAME}
        COMMAND ${PROJECT_NAME}
)

enable_testing()

add_subdirectory(lib)
add_subdirectory(learning)
add_subdirectory(tests)
add_subdirectory(project)