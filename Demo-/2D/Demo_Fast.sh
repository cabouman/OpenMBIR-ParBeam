#!/usr/bin/env bash

cd "$(dirname $0)"

echo " #================================================================#"
echo " #    DEMO : 2D PARALLEL-BEAM CT MBIR Reconstruction SOFTWARE     #"
echo " # by Pengchong Jin, Xiao Wang,  Venkatesh Sridhar and C.A.Bouman #"
echo " #                  Purdue University                             #"
echo " #================================================================#"

echo

echo " ==================== Example 0 ======================================="
echo "   Reconstruct from Shepp-Logan Phantom Sinogram Data                  "
echo " ======================================================================"

cd ../../
./Data/2D/Demo_Fast/run.sh

echo
echo " ================== End example 0 ====================================="

