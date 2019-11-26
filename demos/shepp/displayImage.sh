#!/usr/bin/env bash

cd "$(dirname $0)"

Fname="shepp"

../../IO-Utils/displayImage.py --imgparams ${Fname}_old --images recon/$Fname
