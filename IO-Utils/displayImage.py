#!/usr/bin/env python3

import os
import sys
import argparse



import matplotlib.pyplot as plt
import matplotlib.image as mpimg
from readWriteUtils import *

parser = argparse.ArgumentParser(description='Convert Reconstruction images to png files')
parser.add_argument('--imgparams', required=True, dest='imgparams', help='Image parameters file without extension')
parser.add_argument('--images', required=True, dest='images', help='Image file root without extension')


args, _ = parser.parse_known_args()

x = readImages(args.imgparams, args.images)
img = x[0]
#img = img.reshape(img.shape[1], img.shape[2])
imgplot = plt.imshow(img, cmap='gray')
plt.show()


