#set(THIS test1)
set(This ExampleTests)

set(SOURCES ExampleTests.cpp)
add_executable(${This} ${SOURCES})


#Include(FetchContent)
#FetchContent_Declare(
#        Catch2
#        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#        GIT_TAG        v2.13.1)
#
#FetchContent_MakeAvailable(Catch2)
#find_package(Catch2 REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)
#include(Catch)

target_link_libraries(${This} PUBLIC Catch2::Catch2WithMain)

#catch_discover_tests(${This})

add_test(
    NAME ${This}
    COMMAND ${This}
    )


#add_executable(custom-main-tests test.cpp test-main.cpp)
#target_link_libraries(custom-main-tests PRIVATE Catch2::Catch2)