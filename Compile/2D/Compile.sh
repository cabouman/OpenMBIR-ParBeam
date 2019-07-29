#!/usr/bin/env bash

echo -e "\n"

echo "#=============================================================#"
echo "# Compiling 2-D Parallel Beam CT Reconstruction Code Package  #"
echo "#=============================================================#"

cd ../..

echo -e "\n======== Compiling Part 1 - System Matrix Generation ... ========= \n"
(cd ./SourceCode/2D/Generate_SystemMatrix && make)

echo -e "\n====== Compiling Part 2 - Model-based Iterative Reconstruction ... ========\n"
(cd ./SourceCode/2D/MBIR && make)

echo -e "\n===== Code Package compilation is now complete =========\n"


