#!/usr/bin/env python3

import numpy as np
import array
import glob
import png




def read2D(fName, N1, N2):

    fileID = open(fName, 'rb')

    valuesArray = array.array('f')
    valuesArray.fromfile(fileID, N1*N2)

    fileID.close()

    x = np.asarray(valuesArray).astype('float32').reshape([N1, N2])

    return x

def write2D(x, fName):

    fileID = open(fName, "wb")

    valuesArray = array.array('f')
    valuesArray.fromlist(x.astype('float32').flatten('C').tolist())
    valuesArray.tofile(fileID)


def readImgParams(fName):
    with open(fName) as f:
        content = f.readlines()
    content = [x.strip() for x in content] 

    i = 1; par = {}
    par['Nx'] =                 int(    content[i]); i = i+3
    par['Ny'] =                 int(    content[i]); i = i+3
    par['Deltaxy'] =            float(  content[i]); i = i+3
    par['ROIRadius'] =          float(  content[i]); i = i+3
    par['Nz'] =                 int(    content[i]); i = i+3
    par['DeltaZ'] =             float(  content[i]); i = i+3
    par['FirstSliceNumber'] =   int(    content[i]); i = i+3

    return par


def read2DFilePattern(fPattern, N1, N2):
    # This function can accept a file pattern with globbing wildcards.
    # When files file_01.ext, file_02.ext, ... exist, fPattern="file_[0-9]*.ext"
    # will grab them all

    fNames = glob.glob(fPattern)
    N_images = len(fNames);

    x = np.zeros([N_images, N1, N2])

    for i in range(0, N_images):
        x[i,:,:] = read2D(fNames[i], N1, N2)

    return x, fNames

def writeImage2png(image, fName):
    # Assumes image to be in [0,1]
    bits = 16
    image = (image * (2**16)).astype(np.uint16)
    with open(fName, 'wb') as f:
        writer = png.Writer(width=image.shape[1], height=image.shape[0], bitdepth=bits, greyscale=True)
        image2list = image.tolist()
        writer.write(f, image2list)

def readImages(fRoot_imgparams, fRoot_images):
    # reads all image files, normalizes [min, max] to [0,1] and saves as 16-bit png's

    fName_imgparams = fRoot_imgparams + '.imgparams'
    imgparams = readImgParams(fRoot_imgparams + '.imgparams')

    # Note "f" formatting available in Python 3.6 or later
    #fPattern = f'{fRoot_images}_slice[0-9]*.2Dimgdata'
    fPattern = fRoot_images + '_slice[0-9]*.2Dimgdata'
    x, fNames = read2DFilePattern(fPattern, imgparams['Nx'], imgparams['Ny'])


    return x







