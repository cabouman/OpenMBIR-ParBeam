#!/usr/bin/env bash

cd "$(dirname $0)"

BIN="../../bin"
Fname="afrl"


../../IO-Utils/displayImage.py --imgparams $Fname --images recon/$Fname
