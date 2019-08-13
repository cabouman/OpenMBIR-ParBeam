#!/usr/bin/env python3

import numpy as np
from scipy import misc
import sys, os

def preprocess_xiao(numViews, numChannels, numSlices, sliceStart, numSlicesRecon, setName, inputFileBase, dataOutDir, dataFolder, writeView, lAngle, theta_0, theta_end, fileType):

	print("Xiao's preprocess")

	# Set the Directory for the data
	inputDir = dataFolder
	outputDir = os.path.join('.', dataOutDir)
	sinoDir = os.path.join(outputDir, 'measurements', 'Projections')
	weightDir = os.path.join(outputDir, 'measurements', 'Weights')
	paramDir = os.path.join(outputDir, 'parameters')

	print("Creating measurement folder...")
	os.makedirs(sinoDir, exist_ok=True)
	os.makedirs(weightDir, exist_ok=True)

	# Reading data
	print('Reading and processing the raw data...')

	nlcsData = np.zeros((numSlices, numChannels, numViews))
	subsampling_factor = 1;
	variance = 0

	for iv in range(numViews):
		# print("{}".format(iv))

		# Negative Log Part
		fname = inputFileBase + "{idx:04d}.{ft:s}".format(idx = iv+1, ft = fileType)
		data  = misc.imread(os.path.join(inputDir, fname)).astype("int")
		# data[data==0] += 0.1
		logData = -1 * np.log(data)

		# Corrected Part
		# croppedDataset = logData[899:1024, :]
		croppedDataset = logData[(numChannels-int(numChannels/10)):,:]
		# croppedDataset = logData[:int(numChannels/10),:]
		offset = np.mean(np.mean(croppedDataset))
		offsetData = logData - offset

		# Weights Part
		if iv == int(numViews/2):
			variance = np.var(offsetData[(numChannels-int(numChannels/10)):,:])
			# variance = np.var(offsetData[:int(numChannels/10),:])
			print (variance)

		# Sino Part
		nlcsData[:, :, iv] = offsetData

	print("Done")

	# Writing data
	print('Writing data...')

	# for slice_n in range(sliceStart-1, sliceStart+numSlicesRecon-1):
	#
	# sino = counts[:, :, slice_n]
	# weights = np.exp(-1*sino)/variance
	#
	# # write sino files
	# fname = os.path.join(sinoDir, "{name:s}_slice{idx:04d}.2Dsinodata".format(name=setName, idx=slice_n+1))
	# with open(fname, 'wb') as f:
	# 	sino.T.tofile(f)
	#
	# # write weight files
	# fname = os.path.join(weightDir, "{name:s}_slice{idx:04d}.2Dweightdata".format(name=setName, idx=slice_n+1))
	# with open(fname, 'wb') as f:
	# 	weights.T.tofile(f)

	# Sino Part
	for i in range(sliceStart-1, sliceStart+numSlicesRecon-1):
		sfname = os.path.join(sinoDir, "{name:s}_slice{idx:04d}.2Dsinodata".format(name=setName, idx=i+1))
		wfname = os.path.join(weightDir, "{name:s}_slice{idx:04d}.2Dweightdata".format(name=setName, idx=i+1))

		sino = np.squeeze(nlcsData[i-1, :, :])
		subSino = sino[:, ::subsampling_factor]
		subSino = np.transpose(subSino)
		subSino.astype('float32').tofile(sfname)

		# Wght Part
		wght = np.exp(-1 * sino) / variance
		wght = np.transpose(wght)
		wght.astype('float32').tofile(wfname)

	# Write angles file
	if (writeView == True):
		# Generate view angle list
		fname = os.path.join(paramDir, 'ViewAngleList.txt')
		f = open(fname, 'w')
		theta_0 = theta_0/180*np.pi
		theta_end = theta_end/180*np.pi

		angles = np.linspace(theta_0, theta_end, num = numViews/subsampling_factor, endpoint = lAngle, dtype = np.float32)
		for angle in angles:
			f.write(str(angle) + '\n')
		f.close()

	print("Done")


def preprocess(numViews, numChannels, numSlices, sliceStart, numSlicesRecon, setName, inputFileBase, dataOutDir, dataFolder, writeView, lAngle, theta_0, theta_end, fileType):

	print("Jordan's preprocess")

	# Set the Directory for the data
	inputDir = dataFolder
	outputDir = os.path.join('.', dataOutDir)
	sinoDir = os.path.join(outputDir, 'measurements', 'Projections')
	weightDir = os.path.join(outputDir, 'measurements', 'Weights')
	paramDir = os.path.join(outputDir, 'parameters')

	print("Creating measurement folder...")
	os.makedirs(sinoDir, exist_ok=True)
	os.makedirs(weightDir, exist_ok=True)

	# Reading data
	print('Reading raw data..')

	counts = np.zeros([numChannels, numViews, numSlices], dtype = np.uint8)
	sino = np.zeros([numChannels, numViews], dtype = np.float32)
	weights = np.zeros([numChannels, numViews], dtype = np.float32)

	for view_n in range(numViews):
		fname = inputFileBase + "{idx:04d}.{ft:s}".format(idx = view_n+1, ft = fileType)
		view = misc.imread(os.path.join(inputDir, fname), flatten = True).astype(np.uint8)
		# print(view.shape)
		for row_n in range(numSlices):
			counts[:, view_n, row_n] = view[row_n, :].T

	print("Done")

	# Writing data
	print('Writing data..')

	for slice_n in range(sliceStart-1, sliceStart+numSlicesRecon-1):
		for view_n in range(numViews):
			view = counts[:, view_n, slice_n].astype(np.float32)
			temp = np.sort(view, kind = 'mergesort')[::-1]
			maxc = np.median(temp[1:20])
			weights[:, view_n] = 1.0*np.sqrt(view/maxc)
			view[view == 0] = 0.1
			view = -1.0*np.log(view/maxc)
			sino[:, view_n] = view

		# write sino files
		fname = os.path.join(sinoDir, "{name:s}_slice{idx:04d}.2Dsinodata".format(name=setName, idx=slice_n+1))
		with open(fname, 'wb') as f:
			sino.T.tofile(f)

		# write weight files
		fname = os.path.join(weightDir, "{name:s}_slice{idx:04d}.2Dweightdata".format(name=setName, idx=slice_n+1))
		with open(fname, 'wb') as f:
			weights.T.tofile(f)

	if (writeView == True):
		# Generate view angle list
		fname = os.path.join(paramDir, 'ViewAngleList.txt')
		f = open(fname, 'w')
		theta_0 = theta_0/180*np.pi
		theta_end = theta_end/180*np.pi

		angles = np.linspace(theta_0, theta_end, num = numViews, endpoint = lAngle, dtype = np.float32)
		for angle in angles:
			f.write(str(angle) + '\n')
		f.close()

	print("Done")



def changeDataParam(numViews, numChannels, sliceStart, numSlicesRecon, setName, paramDir):

	# Change the imgparam file
	imgparam = os.path.join(paramDir, setName+'.imgparams')
	changeLine(imgparam, 13, str(numSlicesRecon))
	changeLine(imgparam, 19, str(sliceStart))

	# Change the sinoparam file
	sinoparam = os.path.join(paramDir, setName+'.sinoparams')
	changeLine(sinoparam, 1, str(numChannels))
	changeLine(sinoparam, 4, str(numViews))
	changeLine(sinoparam, 13, str(numSlicesRecon))
	changeLine(sinoparam, 19, str(sliceStart))

	# Change the reconparam file
	reconparam = os.path.join(paramDir, setName+'.reconparams')
	changeLine(reconparam, 31, str(numSlicesRecon))
	changeLine(reconparam, 34, str(sliceStart))

def changeMachineParam(numPixX, numPixY, distSlice, radiRecon, distChan, corOff, pixWid, setName, paramDir):

	# Change the imgparam file
	imgparam = os.path.join(paramDir, setName+'.imgparams')
	changeLine(imgparam, 1, str(numPixX))
	changeLine(imgparam, 4, str(numPixY))
	changeLine(imgparam, 7, str(pixWid))
	changeLine(imgparam, 10, str(radiRecon))
	changeLine(imgparam, 16, str(distSlice))

	# Change the sinoparam file
	sinoparam = os.path.join(paramDir, setName+'.sinoparams')
	changeLine(sinoparam, 7, str(distChan))
	changeLine(sinoparam, 10, str(corOff))
	changeLine(sinoparam, 16, str(distSlice))


def changeReconParam(lincoeff, q, p, t, sigX, priN, priD, priA, stopThres, maxIters, posConstraint, setName, paramDir):

	# Change the reconparam file
	reconparam = os.path.join(paramDir, setName+'.reconparams')
	changeLine(reconparam, 1, str(lincoeff))
	changeLine(reconparam, 4, str(q))
	changeLine(reconparam, 7, str(p))
	changeLine(reconparam, 10, str(t))
	changeLine(reconparam, 13, str(sigX))
	changeLine(reconparam, 16, str(priN))
	changeLine(reconparam, 19, str(priD))
	changeLine(reconparam, 22, str(priA))
	changeLine(reconparam, 25, str(stopThres))
	changeLine(reconparam, 28, str(maxIters))
	changeLine(reconparam, 37, str(1 if posConstraint is True else 0))

def dataLoad(setName, paramDir):

	dataImg = []
	dataSino = []
	dataRecon = []

	imgparam = os.path.join(paramDir, setName+'.imgparams')
	sinoparam = os.path.join(paramDir, setName+'.sinoparams')
	reconparam = os.path.join(paramDir, setName+'.reconparams')

	with open(imgparam, 'r') as file:
		dataImg = file.readlines()

	with open(sinoparam, 'r') as file:
		dataSino = file.readlines()

	with open(reconparam, 'r') as file:
		dataRecon = file.readlines()

	return dataImg, dataSino, dataRecon


def createTarGzFile(outputFile):
	os.system("tar -cvf " + outputFile + ".tar measurements parameters Reconstruction")
	os.system("gzip " + outputFile + ".tar")

def generateMD5(outputFile):
	os.system("md5sum " + outputFile + ".tar.gz > " + outputFile + ".md5sum")

def changeLine(filename, lineIndex, newLine):
	data = []
	with open(filename, 'r') as file:
		data = file.readlines()
	data[lineIndex] = newLine + "\n"
	with open(filename, 'w') as file:
		file.writelines(data)

def main():
	# Define global variables
	numViews 		= 900
	numChannels 	= 1024
	numSlices 		= 1024
	sliceStart 		= 505
	numSlicesRecon 	= 20
	setName         = "demo"
	inputFileBase   = "demo_AAAA_BBB"
	dataFolder         = "data"

	#numViews = 1
	preprocess(numViews, numChannels, numSlices, sliceStart, numSlicesRecon, setName, inputFileBase, dataFolder)
	changeParameters(numViews, numChannels, sliceStart, numSlicesRecon, setName, paramDir)
	createTarGzFile(outputFile)
	generateMD5(outputFile)

if __name__ == "__main__":
	main()
