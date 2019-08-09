#!/usr/bin/env python3
import os
import sys
import argparse
from readWriteUtils import *

def convertImages2png(fRoot_imgparams, fRoot_images, verbose=False):
    # reads all image files, normalizes [min, max] to [0,1] and saves as 16-bit png's

    fName_imgparams = fRoot_imgparams + '.imgparams'
    imgparams = readImgParams(fRoot_imgparams + '.imgparams')

    fPattern = f'{fRoot_images}_slice[0-9]*.2Dimgdata'
    x, fNames = readImages(fPattern, imgparams['Nx'], imgparams['Ny'])

    x_normalized = (x - x.min())/x.ptp()
    fNames_png = [f'{t}.png' for t in fNames]

    if(verbose):
        print("Writing files ...")
    for i in range(0, len(fNames_png)):
        if(verbose):
            print(f'\t{fNames_png[i]}')
        writeImage2png(x_normalized[i], fNames_png[i])

    return x, fNames, fNames_png


def main():


    parser = argparse.ArgumentParser(description='Convert Reconstruction images to png files')
    parser.add_argument('--imgparams', required=True, dest='imgparams', help='Image parameters file without extension')
    parser.add_argument('--images', required=True, dest='images', help='Image file root without extension')


    args, _ = parser.parse_known_args()


    convertImages2png(args.imgparams, args.images, verbose=True)




if __name__ == '__main__':
    main()
