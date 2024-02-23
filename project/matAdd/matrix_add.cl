__kernel void matrix_add(__global const int *a, __global const int *b, __global int *result) {
    int gid_0 = get_global_id(0);
    int gid_1 = get_global_id(1);

    int size = get_global_size(0);

    int index = gid_1 * size + gid_0;
    result[index] = a[index] + b[index];
}
