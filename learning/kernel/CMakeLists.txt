set(This KernelTest)

set(SOURCES test.c)
add_executable(${This} ${SOURCES})

target_link_libraries(${This} PUBLIC Catch2::Catch2WithMain  OpenCL::OpenCL)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test.cl ${CMAKE_CURRENT_BINARY_DIR}/test.cl COPYONLY)

add_test(
        NAME ${This}
        COMMAND ${This}
)