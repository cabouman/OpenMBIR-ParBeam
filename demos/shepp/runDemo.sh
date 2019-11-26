#!/usr/bin/env bash

cd "$(dirname $0)"

BIN="../../bin"
Fname="shepp"

#Generate System Matrix
#<Executable File Name>  -i <InputFileName>[.imgparams] -j <InputFileName>[.sinoparams] -m <OutputFileName>[.2Dsysmatrix]

$BIN/Gen_SysMatrix_3D -i $Fname -j $Fname -m $Fname


#Compute MBIR Reconstruction
#./<Executable File Name> -i <InputFileName>[.imgparams] -j <InputFileName>[.sinoparams]  -k <InputFileName>[.reconparams] \
# -m <InputFileName>[.2Dsysmatrix] -s <InputFileName>[.2Dsinodata] -w <InputFileName>[.2Dweightdata] -r <OutputFileName>[.2Dimgdata] \
# Additional option (for initial image): -t <InitialImageFileName>[.2Dimgdata]

$BIN/mbir_3D -i $Fname -j $Fname -k $Fname -m $Fname -s sino/$Fname -w weight/$Fname -r recon/$Fname
