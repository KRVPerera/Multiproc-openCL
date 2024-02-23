set(This HelloTest)

set(SOURCES test.c)
add_executable(${This} ${SOURCES})

target_link_libraries(${This} PUBLIC Catch2::Catch2WithMain  OpenCL::OpenCL)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/hello_world.cl ${CMAKE_CURRENT_BINARY_DIR}/hello_world.cl COPYONLY)

add_test(
        NAME ${This}
        COMMAND ${This}
)