# Multiproc-openCL
This repository is for the learning purpose of GPGPU with OpenCL.

## Performance comparison
- values are not averaged and are taken from the first run. They are not checked to be within 95% confidence interval. 
We are currently working on it.

pre operations times are populated in `data/out/profile_data_X_MT.txt` or `data/out/profile_data_X.txt` where X is the image number.

| Operation   | Single Thread (μs) | Multi Thread (μs) | Speedup | OpenCL without memory copy times (μs) |
|-------------|--------------------|-------------------|---------|---------------------------------------|
| Image Read  | 558094.500         | -                 | -                                     |
| Image Save  | 79159.695          | -                 | -                                     |
| Resize      | 3889.900           | 2670.600          |         | 1029.800                              |
| Grayscale   | 4911.400           | 1168.300          |         | 226.210                               |
| Blur        | 192531.391         | 33329.398         |         | 1151.520                              |
| Disparity   | 587603.438         | 168107.750        |         | 163.880                               |
| Cross Check | 3536.050           | 1220.300          |         | 520.560                               |
| Occulsion   | 5636.250           | 1218.000          |         | 709.610                               |



## Folder structure

- learning
    - platform: Get platforms and devices
    - context: Set the context and get the reference count
    - program: Create program and build the program
    - kernel: Create kernels and search for a specific kernel by name
    - command-queue: Create the command queue and enque kernel execution commands
    - buffer: Create buffer and sub-buffer, get info
    - mapNcopy: Mapping and copying buffer objects
    - hello-world: First kernal program

- project
    - matAdd: Added two matrixes using C and OpenCL.
    - opencl_flow_ex3: OpenCL implementation of Image resizing, gray scaling and applied gaussian blur.

## Implementation locations
- OpenCL and C matrix addition : `project/matAdd/`
- OpenCL resize, grayscale, gaussian filter : `project/opencl_flow_ex3`
- save image (C) : `void saveImage(const char *filename, Image *image)` in `src/pngloader/pngloader.c`
- read image (C) : `Image *readImage(const char *filename)` in `src/pngloader/pngloader.c`
- resize (C) : `Image *resizeImage(Image *input)` in `src/pngloader/pngloader.c`
- grayscale scale (C) : `Image *grayScaleImage(Image *input)` in `src/pngloader/pngloader.c` 
- occlusion_filling (C) : `occulsion_filling.c`
- cross_checking (C): `cross_checking.c`
- 5x5 filter (C) :
    - `float applyFilterToNeighboursFloat(float *neighbours, unsigned char *filter, int size)` in `src/pngloader/pngloader.c`
    - `getGaussianFilter` in `driver.c`

## How to run

- clone the repository
- create a build folder inside the repository

### To Build

```bash
  mkdir build
  cd build
  cmake ..
  make -j
```
- Output images will be saved in `data/out` folder

### Run C single thread implementation
- After building the project run.

`
./Multiproc-openCL single
`

### Run C multithreaded implementation
- After building the project run.

`
./Multiproc-openCL mp
`

### Run opencl implementation
- After building the project run.

`
./Multiproc-openCL opencl
`

## Outputs

- One of the Inputs 2940x2016
![Input 1](data/sample/im0.png)


- Resized and grayscale 735x504
![Scaled down grayscale](docs/image_0_bw.png)


- Blur filter added 735x504
![Blurred](docs/image_0_bw_blurred.png)


- Left disparity 735x504
![Blurred](docs/image_left_disparity.png)


- Cross check 735x504
![Blurred](docs/image_cross_checking_LEFT.png)


- Occulsion filled 735x504
![Output](docs/image_occulsion_filed_LEFT.png)

## Reference

- OpenCL in Action by Matthew Scarpino