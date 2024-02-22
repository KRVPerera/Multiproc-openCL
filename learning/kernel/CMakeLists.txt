set(This kernel)

set(SOURCES test.c)
add_library(${This} ${SOURCES})

target_link_libraries(${This} PUBLIC Catch2::Catch2WithMain OpenCL::OpenCL)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test.cl ${CMAKE_CURRENT_BINARY_DIR}/test.cl COPYONLY)

add_executable(KernelTest KernelTest.cpp)
target_link_libraries(KernelTest PUBLIC Catch2::Catch2WithMain OpenCL::OpenCL ${This})
add_test(
        NAME KernelTest
        COMMAND KernelTest
)