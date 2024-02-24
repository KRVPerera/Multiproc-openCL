set(This imageformatsupport)

set(SOURCES test.c)
add_executable(${This} ${SOURCES})

target_link_libraries(${This} PUBLIC Catch2::Catch2WithMain  OpenCL::OpenCL)

add_test(
        NAME ${This}
        COMMAND ${This}
)