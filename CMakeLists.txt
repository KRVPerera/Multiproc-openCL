cmake_minimum_required(VERSION 3.19)

project(MultiprocOpenCL
        VERSION 0.1.0
        LANGUAGES C CXX)

find_package(OpenCL REQUIRED)

cmake_host_system_information(RESULT N
                              QUERY NUMBER_OF_PHYSICAL_CORES)
add_definitions( -DPROCESSOR_COUNT=${N} )

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-deprecated-declarations -Wno-newline-eof -Wno-strict-prototypes -Wno-format -Wno-sometimes-uninitialized -Wno-maybe-uninitialized")
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
else()
    message("Release mode")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

# set(OpenMP_C_FLAGS "-fopenmp")
# set(OpenMP_C_LIB_NAMES "lomp")

if(WIN32)
    message("https://cmake.org/cmake/help/latest/variable/WIN32.html?highlight=win32")
    find_package(OpenMP COMPONENTS C REQUIRED)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
elseif(APPLE)
    message("https://cmake.org/cmake/help/latest/variable/APPLE.html?highlight=apple")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -fopenmp -I/opt/homebrew/opt/libomp/include")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/opt/homebrew/opt/libomp/lib  -fopenmp -lomp")
elseif(UNIX)
    message("https://cmake.org/cmake/help/latest/variable/UNIX.html?highlight=unix")
    find_package(OpenMP COMPONENTS C REQUIRED)
endif()

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

#target_compile_definitions(catch-test PRIVATE CATCH_CONFIG_MAIN CATCH_CONFIG_FAST_COMPILE CATCH_CONFIG_DISABLE_MATCHERS)
#target_precompile_headers(catch-test PRIVATE Catch2/single_include/catch2/catch.hpp)

configure_file(config.h.in config.h)

include_directories(include)
include_directories(include/opencl)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${OpenCL_INCLUDE_DIRS})

add_library(logger STATIC src/logger.c include/logger.h)
add_library(utils STATIC src/util.c include/util.h)
target_link_libraries(utils PUBLIC m logger learn_openmp)
add_library(img_loader STATIC src/pngloader.c include/pngloader.h)
target_link_libraries(img_loader PUBLIC lodepng)

add_library(occlusion STATIC src/occlusion_filling.c include/occlusion_filling.h)

add_library(learn_openmp STATIC src/calc_pi_mt.c include/calc_pi.h)

add_executable(${PROJECT_NAME} main.c src/driver.c src/zncc_c_imp.c src/cross_checking.c src/occlusion_filling.c)
#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} OpenCL::OpenCL img_loader utils opencl_flow_ex3_lib opencl_flow_ex5_lib opencl_flow_ex6_lib logger occlusion learn_openmp)


add_test(
        NAME ${PROJECT_NAME}
        COMMAND ${PROJECT_NAME}
)

enable_testing()

add_subdirectory(lib)
add_subdirectory(learning)
add_subdirectory(tests)
add_subdirectory(project)
