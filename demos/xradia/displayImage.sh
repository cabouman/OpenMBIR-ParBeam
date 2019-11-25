#!/usr/bin/env bash

cd "$(dirname $0)"

Fname="xradia"

../../IO-Utils/displayImage.py --imgparams ${Fname}_old --images recon/$Fname
