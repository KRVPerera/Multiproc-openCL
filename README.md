# Multiproc-openCL
This repository is for the learning purpose of GPGPU with OpenCL.

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
    - matAdd: Adding two matrixes
    - matMul: Muliply two matrixes
    - image_to_grayscale: Convert color image to grayscale

## How to run

- clone the repository
- create a build folder inside the repository

### To run the c implemenation

- run the following commands
```bash
  cmake ..
  make
  ./MultiprocOpenCL
```
- Output images will be saved in `data/out` folder

Main program currently use images from `data/sample` folder.

### Run opencl implementation
- After building the project run.

- `ctest -R image_to_grayscale`

- Check the outputs at `build\project\image_to_grayscale`

## Reference

- OpenCL in Action by Matthew Scarpino