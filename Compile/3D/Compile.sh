#!/usr/bin/env bash

cd "$(dirname $0)"
echo -e "\n"

echo "#=============================================================#"
echo "# Compiling 3-D Parallel Beam CT Reconstruction Code Package  #"
echo "#=============================================================#"

cd ../..

echo -e "\n======== Compiling Part 1 - System Matrix Generation ... ========= \n"
(cd ./SourceCode/3D/Generate_SystemMatrix && make)

echo -e "\n====== Compiling Part 2 - Model-based Iterative Reconstruction ... ========\n"
(cd ./SourceCode/3D/MBIR && make)

echo -e "\n===== Code Package compilation is now complete =========\n"


