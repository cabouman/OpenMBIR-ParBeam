#!/usr/bin/env bash

cd "$(dirname $0)"

BIN="../../bin"
Fname="xradia"


../../IO-Utils/displayImage.py --imgparams $Fname --images recon/$Fname
