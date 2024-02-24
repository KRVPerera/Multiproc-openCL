cmake_minimum_required(VERSION 3.16)

project(MultiprocOpenCL
        VERSION 0.0.1
        LANGUAGES C CXX)

find_package(OpenCL REQUIRED)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

add_subdirectory(lib)

# set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

include_directories(include/pngloader)
include_directories(include/common)

set(FOLDER_FILES
        sample/im0.png
        sample/im1.png
        # Add more files as needed
)

# Loop through the files and configure each one
foreach(FILE ${FOLDER_FILES})
    file(COPY ${PROJECT_SOURCE_DIR}/data/${FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

add_library(img_loader SHARED src/pngloader/pngloader.c include/pngloader/pngloader.h)
target_link_libraries(img_loader PUBLIC lodepng)

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} OpenCL::OpenCL img_loader)

enable_testing()

add_test(
        NAME ${PROJECT_NAME}
        COMMAND ${PROJECT_NAME}
)

add_subdirectory(learning)
add_subdirectory(tests)
add_subdirectory(project/image_to_grayscale)
add_subdirectory(project/matAdd)