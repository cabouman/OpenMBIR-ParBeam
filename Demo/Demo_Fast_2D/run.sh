#!/usr/bin/env bash

BIN="../../bin"
Fname="shepp"

#Generate System Matrix
#<Executable File Name>  -i <InputFileName>[.imgparams] -j <InputFileName>[.sinoparams] -m <OutputFileName>[.2Dsysmatrix]

$BIN/Gen_SysMatrix_2D -i params/$Fname -j params/$Fname -m data/$Fname


#Compute MBIR Reconstruction
#./<Executable File Name> -i <InputFileName>[.imgparams] -j <InputFileName>[.sinoparams]  -k <InputFileName>[.reconparams] \
# -m <InputFileName>[.2Dsysmatrix] -s <InputFileName>[.2Dsinodata] -w <InputFileName>[.2Dweightdata] -r <OutputFileName>[.2Dimgdata] \
# Additional option (for initial image): -v <InitialImageFileName>[.2Dimgdata]

$BIN/mbir_2D -i params/$Fname -j params/$Fname -k params/$Fname -m data/$Fname -s data/$Fname -w data/$Fname -r $Fname
