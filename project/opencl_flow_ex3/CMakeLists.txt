set(This opencl_flow_ex3_lib)

set(SOURCES opencl_flow_ex3.c)

#add_executable(${This} ${SOURCES})
#target_link_libraries(${This} PUBLIC OpenCL::OpenCL img_loader)
#target_include_directories(${This} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

add_library(${This} STATIC ${SOURCES})
target_link_libraries(${This} PUBLIC OpenCL::OpenCL img_loader)
target_include_directories(${This} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

configure_file(config_im_to_g.h.in config_im_to_g.h)

add_test(
        NAME ${This}
        COMMAND ${This}
)