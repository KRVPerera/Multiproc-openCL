set(This zncc_c)

set(SOURCES zncc_c.c)
add_library(${This} "${SOURCES}")
target_link_libraries(${This} PUBLIC img_loader)

add_test(
        NAME ${This}
        COMMAND ${This}
)