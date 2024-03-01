set(This kernel_search_test)

set(SOURCES test.c)
add_executable(${This} ${SOURCES})

target_link_libraries(${This} PUBLIC OpenCL::OpenCL)
target_include_directories(${This} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config_kernel_search.h.in ${CMAKE_CURRENT_BINARY_DIR}/config_kernel_search.h)

add_test(
        NAME ${This}
        COMMAND ${This}
)