#!/usr/bin/env bash

DirName="./Data/3D/Demo_Fast"
Fname="afrl"

#***********

#Generate System Matrix
# Command line format :
#./<Executable File Name>  -i <InputFileName>[.imgparams] -j <InputFileName>[.sinoparams] -m <OutputFileName>[.2Dsysmatrix]

# Note : The necessary extensions for certain input files are mentioned above within a "[ ]" symbol
# However, they are NOT to be included as part of the file name in the command line arguments.

args_required="-i $DirName/parameters/$Fname -j $DirName/parameters/$Fname -m $DirName/Reconstruction/$Fname"
./SourceCode/3D/Generate_SystemMatrix/GenerateSystemMatrix3DParallel $args_required


#***********

#MBIR
# Command line format :
#./<Executable File Name> -i <InputFileName>[.imgparams] -j <InputFileName>[.sinoparams]  -k <InputFileName>[.reconparams] \
# -m <InputFileName>[.2Dsysmatrix] -s <InputProjectionsBaseFileName> -w <InputWeightsBaseFileName> -r <OutputImageBaseFileName>
# Additional option (to read in initial image from hard disk): -v <InitialImageBaseFileName>

# Note : The necessary extensions for certain input files are mentioned above within a "[ ]" symbol above
# However, they are NOT to be included as part of the file name in the command line arguments.

# The below instructions pertain to the -s, -w and -r options in the above Command line format:
# A)
# The Sinogram Projections measured for the 3-D volume are organized slice by slice in a single directory.
# All files within this directory must share a common BaseFileName and follow the below format :
# <ProjectionsBaseFileName>_slice<SliceIndex>.2Dsinodata
# , where "SliceIndex" is a non-negative integer indexing each slice and is printed with 4 digits. Eg : 0000 to 9999 is a valid descriptor for "SliceIndex"
# B)
# Similarly, in the case of the Sinogram Weights, the format of for the file-names are :
# <WeightsBaseFileName>_slice<SliceIndex>.2Dweightdata
# C)
# Similarly, the Reconstructed (Output) Image is organized slice by slice. The format for the file-names are :
# <ImageBaseFileName>_slice<SliceIndex>.2Dimgdata


args_MBIR="-i $DirName/parameters/$Fname -j $DirName/parameters/$Fname -k $DirName/parameters/$Fname "\
"-m $DirName/Reconstruction/$Fname -s $DirName/measurements/Projections/$Fname -w $DirName/measurements/Weights/$Fname "\
"-r $DirName/Reconstruction/Image/$Fname"
./SourceCode/3D/MBIR/MBIR_3DCT $args_MBIR
