#!/usr/bin/env bash

DirName="./Data/2D/Demo_Fast"
Fname="shepp"

#******

#Generate System Matrix
#Command line format: ./<Executable File Name>  -i <InputFileName.imgparams> -j <InputFileName.sinoparams> -m <OutputFileName.2Dsysmatrix>

# Note : The necessary extensions for certain input files are mentioned above within a "[ ]" symbol above
# However, they are NOT to be included as part of the file name in the command line arguments

args_required="-i $DirName/parameters/$Fname -j $DirName/parameters/$Fname -m $DirName/Reconstruction/$Fname"
./SourceCode/2D/Generate_SystemMatrix/GenerateSystemMatrix2D $args_required


#******

#MBIR
#Command line format:
#./<Executable File Name> -i <InputFileName>[.imgparams] -j <InputFileName>[.sinoparams]  -k <InputFileName>[.reconparams] \
# -m <InputFileName>[.2Dsysmatrix] -s <InputFileName>[.2Dsinodata] -w <InputFileName>[.2Dweightdata] -r <OutputFileName>[.2Dimgdata] \
# Additional option (for initial image): -v <InitialImageFileName>[.2Dimgdata]

# Note : The necessary extensions for certain input files are mentioned above within a "[ ]" symbol above
# However, they are NOT to be included as part of the file name in the command line arguments

args_MBIR="-i $DirName/parameters/$Fname -j $DirName/parameters/$Fname -k $DirName/parameters/$Fname "\
"-m $DirName/Reconstruction/$Fname -s $DirName/measurements/$Fname -w $DirName/measurements/$Fname "\
"-r $DirName/Reconstruction/$Fname"
./SourceCode/2D/MBIR/MBIR_2DCT $args_MBIR
