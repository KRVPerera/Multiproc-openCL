#include <catch2/catch_test_macros.hpp>

extern "C" {
    int kernelFound(); // Add missing function declaration for factorial
}

TEST_CASE( "Kernel found", "[KERNEL]" ) {
    CHECK( kernelFound() == 0 );
}