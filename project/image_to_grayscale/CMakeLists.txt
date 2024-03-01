set(This image_to_grayscale)

set(SOURCES test.c)
add_executable(${This} ${SOURCES})

target_link_libraries(${This} PUBLIC OpenCL::OpenCL img_loader)
target_include_directories(${This} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${PROJECT_SOURCE_DIR}/data/sample/im0.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/data/sample/im1.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config_im_to_g.h.in ${CMAKE_CURRENT_BINARY_DIR}/config_im_to_g.h)

add_test(
        NAME ${This}
        COMMAND ${This}
)