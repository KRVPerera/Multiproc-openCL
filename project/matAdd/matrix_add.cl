__kernel void matrix_add(__global const float *a, __global const float *b, __global float *result) {
    int gid_0 = get_global_id(0);
    int gid_1 = get_global_id(1);

    int size = get_global_size(0);

    int index = gid_0 * size + gid_1;
    result[index] = a[index] + b[index];
}
