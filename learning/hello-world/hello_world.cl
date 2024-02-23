__kernel void hello_world(__global char16 *msg) {
    *msg = (char16) ( 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', '!', '!', '!', '\0' );
}