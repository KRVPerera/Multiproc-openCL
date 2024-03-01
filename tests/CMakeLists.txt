list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)

add_executable(neighbour_test NeighbourTests.cpp)
target_link_libraries(neighbour_test PUBLIC Catch2::Catch2WithMain img_loader)
add_test(NAME neighbour_test COMMAND neighbour_test)