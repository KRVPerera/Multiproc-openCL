set(This kernel_search_test)

set(SOURCES test.c)
add_executable(${This} ${SOURCES})

target_link_libraries(${This} PUBLIC OpenCL::OpenCL)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test.cl DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_test(
        NAME ${This}
        COMMAND ${This}
)