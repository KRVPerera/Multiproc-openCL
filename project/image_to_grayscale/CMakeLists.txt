set(This image_to_grayscale)

set(SOURCES test.c)
add_executable(${This} ${SOURCES})

target_link_libraries(${This} PUBLIC Catch2::Catch2WithMain  OpenCL::OpenCL img_loader)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/color_to_gray.cl DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/im0.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_test(
        NAME ${This}
        COMMAND ${This}
)