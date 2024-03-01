set(This zncc_test)

set(SOURCES zncc_c.c)
add_executable(${This} ${SOURCES})

target_link_libraries(${This} PUBLIC OpenCL::OpenCL img_loader)
target_include_directories(${This} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

configure_file(config_zncc_c.h.in config_zncc_c.h)

add_test(
        NAME ${This}
        COMMAND ${This}
)