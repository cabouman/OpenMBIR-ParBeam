# OpenMBIR-ParBeam 
BASELINE MBIR RECONSTRUCTION SOFTWARE FOR 2D and 3D PARALLEL-BEAM CT

This is a reference implementation of MBIR, but it is not parallel or optimized for speed.
A much faster version of this parallel beam MBIR algorithm is available at:
  https://github.com/HPImaging/sv-mbirct

## Software Requirements
1. gcc compiler
2. OpenMP API
3. Python 3 or later
4. Python Packages
   * numpy
   * matplotlib
 

## Instructions

1) Install the required software

2) Compile the code. From the main directory run `cd src && make && cd ~-`. The executables will be stored in the `bin` folder. 
   
3) Run the demos
   * In the `demos` folder there are fast and slow 2D and 3D demos
   * To run a demo execute the respective `runDemo.sh` script.
   * To display the result run the `displayResult.sh` script.
   
   The `displayResult.sh` script makes use of the python IO Utilities in `IO-Utils`. You can use these utilities to read the images into a python numpy array.


## Reconstructing Your Own Data

1) To be
2) added later ...



