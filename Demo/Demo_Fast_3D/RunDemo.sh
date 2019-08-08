#!/usr/bin/env bash

cd "$(dirname $0)"

BIN="../../bin"
Fname="afrl"

#Generate System Matrix
#<Executable File Name>  -i <InputFileName>[.imgparams] -j <InputFileName>[.sinoparams] -m <OutputFileName>[.3Dsysmatrix]

$BIN/Gen_SysMatrix_3D -i $Fname -j $Fname -m $Fname


#Compute MBIR Reconstruction
#./<Executable File Name> -i <InputFileName>[.imgparams] -j <InputFileName>[.sinoparams]  -k <InputFileName>[.reconparams] \
# -m <InputFileName>[.3Dsysmatrix] -s <InputFileName>[.3Dsinodata] -w <InputFileName>[.3Dweightdata] -r <OutputFileName>[.3Dimgdata] \
# Additional option (for initial image): -v <InitialImageFileName>[.3Dimgdata]

$BIN/mbir_3D -i $Fname -j $Fname -k $Fname -m $Fname -s sino/$Fname -w weight/$Fname -r recon/$Fname
