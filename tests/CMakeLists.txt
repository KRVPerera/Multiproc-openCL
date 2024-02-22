#set(THIS test1)
set(This ExampleTests)

set(SOURCES ExampleTests.cpp)

message("This: ${This}")

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v2.13.1)

FetchContent_MakeAvailable(Catch2)

#target_link_libraries(tests Catch2::Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)
include(Catch)
catch_discover_tests(${This})


add_executable(${THIS} ${SOURCES})
target_link_libraries(${THIS} PRIVATE Catch2::Catch2WithMain)

#add_test(
#    NAME ${THIS}
#    COMMAND ${THIS}
#    )


#add_executable(custom-main-tests test.cpp test-main.cpp)
#target_link_libraries(custom-main-tests PRIVATE Catch2::Catch2)