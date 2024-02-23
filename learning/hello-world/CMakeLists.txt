set(This HelloTest)

set(SOURCES test.c)
add_executable(${This} ${SOURCES})

target_link_libraries(${This} PUBLIC Catch2::Catch2WithMain  OpenCL::OpenCL)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/hello_world.cl DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_test(
        NAME ${This}
        COMMAND ${This}
)