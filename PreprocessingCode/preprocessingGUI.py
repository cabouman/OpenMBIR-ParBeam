#!/usr/bin/env python3

import sys, os, shutil
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from preprocessing import *

class PreprocessingGUI(QWidget):

	def __init__(self):

		self.initParam()
		super().__init__()
		self.initUI()


	def initParam(self):
		# System Parameters
		self.numViews 		= 900
		self.numChannels 	= 1024
		self.numRows 		= 1024
		self.rowStart 		= 500
		self.numRowsRecon 	= 22
		self.setName        = "demo"
		self.inputFileBase  = "demo_AAAA_BBB"
		self.dataOutDir     = "upload"
		self.dataFolder     = "data/raw"
		self.numPixX		= 1024
		self.numPixY		= 1024
		self.numPixZ		= self.numRowsRecon
		self.distRow		= 0.0000625
		self.distSliceV		= self.distRow
		self.radiRecon		= 100
		self.distChan		= 0.0000625
		self.corOff			= 0.0
		self.pixWid			= 0.0000625
		self.initPV 		= 0.0202527
		self.q				= 2
		self.p				= 1.2
		self.t				= 1
		self.sigX			= 600
		self.sigY			= 1
		self.priN			= 1
		self.priD			= 0.707
		self.priA			= 1
		self.stopThres		= 0.1
		self.maxIters		= 150
		self.writeView 		= True
		self.fileType		= 'tif'
		self.theta_0		= 0
		self.theta_end		= 180
		self.lAngle			= False
		self.posConstraint  = True
		self.paramDir = os.path.join(self.dataOutDir, 'parameters')
		self.preprocess_type = 1

		# initiate error flag and error messages
		self.errFlag = 0
		self.errMsg = ''

	def initUI(self):

		self.resize(620,500)

		self.font = QFont("Arial")
		self.font.setPointSize(12)
		self.setFont(self.font)

		# Create tabs for the window
		self.tabs = QTabWidget()
		self.tabData = QWidget()
		self.tabSino = QWidget()
		self.tabImg = QWidget()
		self.tabRecon = QWidget()

		# Add subtabs to the tap windwow
		self.tabs.addTab(self.tabData, "Data Input Param")
		self.tabs.addTab(self.tabSino, "Sinogram Param")
		self.tabs.addTab(self.tabImg, "Image Param")
		self.tabs.addTab(self.tabRecon, "Reconstruction Param")

		# Add tool tips for tabs
		self.tabData.setToolTip('Location of raw data files and type of input data to preprocess')
		self.tabSino.setToolTip('Parameters associated with generation of sinograms and detector resolution')
		self.tabImg.setToolTip('Parameters specfying dimensions of input img files and slices to reconstuct')
		self.tabRecon.setToolTip('Parameters specifying prior model assumptions and other misc reconstruction parameters')

		# Configurate Data data

		self.tabData.layout = QFormLayout()
		self.tabData.setLayout(self.tabData.layout)
		self.addEntryFieldstabData()

		# Configurate Tab Sinogram
		self.tabSino.layout = QFormLayout()
		self.tabSino.setLayout(self.tabSino.layout)
		self.addEntryFieldstabSino()

		# Configurate Tab Image
		self.tabImg.layout = QFormLayout()
		self.tabImg.setLayout(self.tabImg.layout)
		self.addEntryFieldstabImg()

		# Configurate Tab Reconstruction
		self.tabRecon.layout = QFormLayout()
		self.tabRecon.setLayout(self.tabRecon.layout)
		self.addEntryFieldstabRecon()

		# Add tabs to the window as well as the buttons
		self.layout = QVBoxLayout()
		self.setLayout(self.layout)
		self.addEntryFieldsButtons()

		self.setWindowTitle("Preprocessing GUI")
		self.show()

	def addEntryFieldstabData(self):

		self.ll_daN = QLabel("Name of the Project")
		self.ll_daN.setToolTip('')
		self.le_daN = QLineEdit()
		self.le_daN.setText(self.setName)
		self.tabData.layout.addRow(self.ll_daN, self.le_daN)

		self.hbox_daF = QHBoxLayout()
		self.fl = QLabel(self.dataFolder)
		self.btn_cF = QPushButton("Select")
		self.hbox_daF.addWidget(self.fl)
		self.hbox_daF.addWidget(self.btn_cF)
		self.btn_cF.clicked.connect(self.handleFolderChoose)
		self.ll_daF = QLabel("Choose the raw data folder")

		self.tabData.layout.addRow(self.ll_daF, self.hbox_daF)

		self.ll_daFB = QLabel("Base of raw data file name")
		self.le_daFB = QLineEdit()
		self.le_daFB.setText(self.inputFileBase)
		self.tabData.layout.addRow(self.ll_daFB, self.le_daFB)

		self.ll_daFT = QLabel("File extension of the raw data")
		self.le_daFT = QLineEdit()
		self.le_daFT.setText(self.fileType)
		self.tabData.layout.addRow(self.ll_daFT, self.le_daFT)


		self.type_chkbx = QComboBox(self)
		self.type_chkbx.addItem("Normalized photon counts in uint8")
		self.type_chkbx.addItem("Unnormalized photon counts in uint8")
		self.hbox_daType = QHBoxLayout()
		self.hbox_daType.addWidget(self.type_chkbx)
		self.ll_type = QLabel("Input Data Type")
		self.tabData.layout.addRow(self.ll_type, self.hbox_daType)
		self.type_chkbx.currentIndexChanged.connect(self.handleTypeChange)


		self.preprocess_data_desc_label = QLabel('')
		self.tabData.layout.addRow(self.preprocess_data_desc_label)
		self.updateDataTypeDescription()


	def addEntryFieldstabSino(self):

		self.ll_numC = QLabel("Number of Channels")
		self.ll_numC.setToolTip('Number of Channels in detector')
		self.le_numC = QLineEdit()
		self.le_numC.setText(str(self.numChannels))
		self.le_numC.setToolTip('Number of Channels in detector')
		self.tabSino.layout.addRow(self.ll_numC, self.le_numC)

		self.ll_numV = QLabel("Number of Views")
		self.ll_numV.setToolTip('Number of view angles')
		self.le_numV = QLineEdit()
		self.le_numV.setText(str(self.numViews))
		self.le_numV.setToolTip('Number of view angles')
		self.tabSino.layout.addRow(self.ll_numV, self.le_numV)

		self.ll_numR = QLabel("Number of Rows")
		self.le_numR = QLineEdit()
		self.le_numR.setToolTip('Number of rows in detector')
		self.le_numR.setText(str(self.numRows))
		self.tabSino.layout.addRow(self.ll_numR, self.le_numR)

		self.ll_deltaC = QLabel("Spacing between channels (mm)")
		self.le_deltaC = QLineEdit()
		self.le_deltaC.setToolTip('Detector spacing between channels in mm')
		self.le_deltaC.setText(str(self.distChan))
		self.tabSino.layout.addRow(self.ll_deltaC, self.le_deltaC)

		self.ll_deltaR = QLabel("Spacing between rows (mm)")
		self.le_deltaR = QLineEdit()
		self.le_deltaR.setToolTip('Detector spacing between rows in mm')
		self.le_deltaR.setText(str(self.distRow))
		self.tabSino.layout.addRow(self.ll_deltaR, self.le_deltaR)

		self.ll_CoR = QLabel("Center of offset (units of channel)")
		self.le_CoR = QLineEdit()
		self.le_CoR.setToolTip('Output: Offset of center-of-rotation (Computed from center of detector). Units is the number of channels, and it can be fractional')
		self.le_CoR.setText(str(self.corOff))
		self.tabSino.layout.addRow(self.ll_CoR, self.le_CoR)

		self.ll_startS = QLabel("Index of the first row")
		self.le_startS = QLineEdit()
		self.le_startS.setToolTip('Index fo first row to begin reconstruction')
		self.le_startS.setText(str(self.rowStart))
		self.tabSino.layout.addRow(self.ll_startS, self.le_startS)

		self.ll_numRS = QLabel("Number of rows used to reconstruct")
		self.le_numRS = QLineEdit()
		self.le_numRS.setToolTip('Number of rows to reconstruct starting at index of first row')

		self.le_numRS.setText(str(self.numRowsRecon))
		self.tabSino.layout.addRow(self.ll_numRS, self.le_numRS)

		self.emptyLine_sino = QLabel("")
		self.tabSino.layout.addRow(self.emptyLine_sino)

		self.hbox_VA = QHBoxLayout()
		self.cb1 = QCheckBox("Choose")
		self.cb1.setToolTip('Choose of text file with view angles in radians')
		self.cb2 = QCheckBox("Generate")
		self.cb2.setToolTip('Generate a view angles list file with linearly spaced angle values between first and last angle. Number of angles is same as number of views above.')
		self.cb2.setChecked(True)
		self.bg = QButtonGroup()
		self.bg.addButton(self.cb1)
		self.bg.addButton(self.cb2)
		self.hbox_VA.addWidget(self.cb1)
		self.hbox_VA.addWidget(self.cb2)
		self.bg.buttonClicked.connect(self.btngroupVA)
		self.bgl = QLabel("List of View Angles")
		self.tabSino.layout.addRow(self.bgl, self.hbox_VA)

		# self.ll_VA = QLabel("Selected angle list file:")
		# self.le_VA = QLabel("Something")
		# # self.tabSino.layout.addRow(self.ll_VA, self.le_VA)

		self.ll_fV = QLabel("First view angle (in degrees)")
		self.le_fV = QLineEdit()
		self.le_fV.setToolTip('First View angle in auto-generated view angle list file')
		self.le_fV.setText(str(self.theta_0))
		self.tabSino.layout.addRow(self.ll_fV, self.le_fV)

		self.ll_lV = QLabel("Last view angle (in degrees)")
		self.le_lV = QLineEdit()
		self.le_lV.setToolTip('Last View angle in auto-generated view angle list file')
		self.le_lV.setText(str(self.theta_end))
		self.tabSino.layout.addRow(self.ll_lV, self.le_lV)

		self.hbox_LA = QHBoxLayout()
		self.cb3 = QCheckBox("Yes")
		self.cb3.setToolTip('Include last view angle in list of view angles')
		self.cb4 = QCheckBox("No")
		self.cb4.setToolTip('Do Not Include last view angle in list of view angles')
		self.cb4.setChecked(True)
		self.bg1 = QButtonGroup()
		self.bg1.addButton(self.cb3)
		self.bg1.addButton(self.cb4)
		self.hbox_LA.addWidget(self.cb3)
		self.hbox_LA.addWidget(self.cb4)
		self.bg1.buttonClicked.connect(self.btngroupLA)
		self.bgl1 = QLabel("Include last angle?")
		self.tabSino.layout.addRow(self.bgl1, self.hbox_LA)

	def addEntryFieldstabImg(self):

		self.hbox_pB = QHBoxLayout()


		self.bin_chkbx = QComboBox(self)
		self.bin_chkbx.addItem("No Binning")
		self.bin_chkbx.addItem("2 Pixel Binning")
		self.hbox_pB.addWidget(self.bin_chkbx)

		self.ll_pB = QLabel("Pixel binning")
		self.tabImg.layout.addRow(self.ll_pB, self.hbox_pB)
		self.bin_chkbx.currentIndexChanged.connect(self.handleBinning)

		self.ll_nX = QLabel("Number of pixels in X direction")
		self.ll_nXV = QLabel(str(self.numPixX))
		self.ll_nXV.setToolTip('Number of pixels along x axis (fast variable in storage)')
		self.tabImg.layout.addRow(self.ll_nX, self.ll_nXV)

		self.ll_nY = QLabel("Number of pixels in Y direction")
		self.ll_nYV = QLabel(str(self.numPixY))
		self.ll_nYV.setToolTip('Number of pixels along y axis (slow variable in storage)')

		self.tabImg.layout.addRow(self.ll_nY, self.ll_nYV)

		self.ll_pW = QLabel("Pixel Width (mm)")
		self.ll_pWV = QLabel(str(self.pixWid))
		self.ll_pWV.setToolTip('Spacing between pixels within a single slice in x and y direction (mm)')
		self.tabImg.layout.addRow(self.ll_pW, self.ll_pWV)

		self.emptyLine_img1 = QLabel("")
		self.tabImg.layout.addRow(self.emptyLine_img1)

		self.ll_nZ = QLabel("Number of slices to reconstruct")
		self.ll_nZV = QLabel(str(self.numPixZ))
		self.ll_nZV.setToolTip('Number of slices to preprocess and then reconstruct')
		self.tabImg.layout.addRow(self.ll_nZ, self.ll_nZV)

		self.ll_distSlice = QLabel("Spacing between slices (mm)")
		self.ll_distSliceV = QLabel(str(self.distSliceV))
		self.ll_distSliceV.setToolTip('Spacing between individual slices')
		self.tabImg.layout.addRow(self.ll_distSlice, self.ll_distSliceV)

		self.emptyLine_img2 = QLabel("")
		self.tabImg.layout.addRow(self.emptyLine_img2)

		self.ll_ROI = QLabel("radius of reconstruction in percentage (%)")
		self.le_ROI = QLineEdit()
		self.le_ROI.setToolTip('Radius of the reconstruction as a percentage')
		self.le_ROI.setText(str(self.radiRecon))
		self.tabImg.layout.addRow(self.ll_ROI, self.le_ROI)

	def addEntryFieldstabRecon(self):
		self.hbox_pC = QHBoxLayout()


		self.ll_PV = QLabel("Initial value of the pixel (mm-1)")
		self.le_PV = QLineEdit()
		self.le_PV.setToolTip('Initial image Pixel Value. Unit is mm^-1')
		self.le_PV.setText(str(self.initPV))
		self.tabRecon.layout.addRow(self.ll_PV, self.le_PV)

		self.ll_p = QLabel("q-GGMRF param: p")
		self.le_p = QLineEdit()
		self.le_p.setToolTip('q-GGMRF p parameter [suggested = 1.2]')
		self.le_p.setText(str(self.p))
		self.tabRecon.layout.addRow(self.ll_p, self.le_p)

		self.ll_q = QLabel("q-GGMRF param: q")
		self.le_q = QLineEdit()
		self.le_q.setToolTip('q-GGMRF q parameter [suggested = 2]')
		self.le_q.setText(str(self.q))
		self.tabRecon.layout.addRow(self.ll_q, self.le_q)

		self.ll_t = QLabel("q-GGMRF param: T")
		self.le_t = QLineEdit()
		self.le_t.setToolTip('q-GGMRF t parameter [suggested = 0.1]')
		self.le_t.setText(str(self.t))
		self.tabRecon.layout.addRow(self.ll_t, self.le_t)

		self.ll_sX = QLabel("sigmaX (mm-1)")
		self.le_sX = QLineEdit()
		self.le_sX.setToolTip('q-GGMRF sigma_x parameter (cmmm^-1) [suggested = 0.02 cmmm-1')
		self.le_sX.setText(str(self.sigX))
		self.tabRecon.layout.addRow(self.ll_sX, self.le_sX)

		self.ll_sY = QLabel("sigmaY (mm-1)")
		self.ll_sYV = QLabel(str(self.sigY))
		self.ll_sYV.setToolTip('Scaling constant for weight matrix (W<-W/SigmaY^2). If SigmaY=0, then it is estimated [suggested = 1]')
		self.tabRecon.layout.addRow(self.ll_sY, self.ll_sYV)

		self.ll_wN = QLabel("prior weight (nearest)")
		self.le_wN = QLineEdit()
		self.le_wN.setToolTip('Relative nearest neighbor weight [default = 1]')
		self.le_wN.setText(str(self.priN))
		self.tabRecon.layout.addRow(self.ll_wN, self.le_wN)

		self.ll_wD = QLabel("prior weight (diagonal)")
		self.le_wD = QLineEdit()
		self.le_wD.setToolTip('Relative diagonal neighbor weight in (x,y) plane [default = 1/sqrt(2)]')
		self.le_wD.setText(str(self.priD))
		self.tabRecon.layout.addRow(self.ll_wD, self.le_wD)

		self.ll_wA = QLabel("prior weight (adjacent slice)")
		self.le_wA = QLineEdit()
		self.le_wA.setToolTip('Relative inter-slice neighbor weight along z axis [default = 1]')
		self.le_wA.setText(str(self.priA))
		self.tabRecon.layout.addRow(self.ll_wA, self.le_wA)

		self.ll_sT = QLabel("stop threshold (percentage)")
		self.le_sT = QLineEdit()
		self.le_sT.setToolTip('Stopping threshold in percent [suggested = 1%]')
		self.le_sT.setText(str(self.stopThres))
		self.tabRecon.layout.addRow(self.ll_sT, self.le_sT)

		self.ll_mI = QLabel("maximum number of iters")
		self.le_mI = QLineEdit()
		self.le_mI.setToolTip('Maximum number of iterations, [suggested = 20]')
		self.le_mI.setText(str(self.maxIters))
		self.tabRecon.layout.addRow(self.ll_mI, self.le_mI)

		self.pos_bin_chkbx = QComboBox(self)
		self.pos_bin_chkbx.addItem("On")
		self.pos_bin_chkbx.addItem("Off")
		self.hbox_pC.addWidget(self.pos_bin_chkbx)
		self.ll_pConstraint = QLabel("Positivity Constraint")
		self.tabRecon.layout.addRow(self.ll_pConstraint, self.hbox_pC)
		self.pos_bin_chkbx.currentIndexChanged.connect(self.handlePositivty)


	def handlePositivty(self, i):
		if self.pos_bin_chkbx.currentText() == 'On':
			self.posConstraint  = True
		else:
			self.posConstraint  = False

	def handleBinning(self, i):
		if self.bin_chkbx.currentText() == 'No Binning':
			self.bin1()
		else:
			self.bin2()

	def handleTypeChange(self, i):
		if self.type_chkbx.currentText() == 'Normalized photon counts in uint8':
			self.preprocess_type  = 1
			self.updateDataTypeDescription()
		elif self.type_chkbx.currentText() == 'Unnormalized photon counts in uint8':
			self.preprocess_type  = 2
			self.updateDataTypeDescription()

	def updateDataTypeDescription(self):
		data_desc = 'Data Input File Description:\n'

		if self.preprocess_type == 1:
			data_desc += 'In this format, each input file is a tiff image representing\n'
			data_desc += 'one view of the object being scanned. The individual files\n'
			data_desc += 'contain the normalized photon counts in unsigned 8-bit int format\n'
			data_desc += 'where 0 means dark scan and 255 means ..\n'
		elif self.preprocess_type == 2:
			data_desc += 'In this format, each input file is a tiff image representing\n'
			data_desc += 'one view of the object being scanned. The individual files\n'
			data_desc += 'contain the unnormalized photon counts in unsigned 8-bit int format\n'
			data_desc += 'The preprocessing script will first normalize the data,\n'
			data_desc += 'and then do the preprocessing\n'

		self.preprocess_data_desc_label.setText(data_desc)

	def bin1(self):
		self.pixWid = self.distRow
		self.numPixX = self.numChannels
		self.numPixY = self.numChannels
		self.ll_nXV.setText(str(self.numPixX))
		self.ll_nYV.setText(str(self.numPixY))
		self.ll_pWV.setText(str(self.pixWid))

	def bin2(self):
		self.pixWid = float(self.distRow)*2
		self.numPixX = int(int(self.numChannels)/2)
		self.numPixY = int(int(self.numChannels)/2)
		self.ll_nXV.setText(str(self.numPixX))
		self.ll_nYV.setText(str(self.numPixY))
		self.ll_pWV.setText(str(self.pixWid))

	def btngroupVA(self, btn):
		if (btn.text() == 'Choose'):
			if (self.writeView == True):
				self.writeView = False

				self.VaFile = QFileDialog.getOpenFileName(self, "Select file")[0].split('/')[-1]

				self.tabSino.layout.removeRow(self.ll_fV)
				self.tabSino.layout.removeRow(self.ll_lV)
				self.tabSino.layout.removeRow(self.bgl1)

				self.ll_VA = QLabel("Selected angle list file:")
				self.le_VA = QLabel("Something")
				self.tabSino.layout.addRow(self.ll_VA, self.le_VA)
				self.le_VA.setText(self.VaFile)

		else:
			if (self.writeView == False):
				self.writeView = True
				self.tabSino.layout.removeRow(self.ll_VA)

				self.ll_fV = QLabel("First view angle (in degrees)")
				self.le_fV = QLineEdit()
				self.le_fV.setText(str(self.theta_0))
				self.tabSino.layout.addRow(self.ll_fV, self.le_fV)

				self.ll_lV = QLabel("Last view angle (in degrees)")
				self.le_lV = QLineEdit()
				self.le_lV.setText(str(self.theta_end))
				self.tabSino.layout.addRow(self.ll_lV, self.le_lV)

				self.hbox_LA = QHBoxLayout()
				self.cb3 = QCheckBox("Yes")
				self.cb4 = QCheckBox("No")
				self.cb4.setChecked(True)
				self.bg1 = QButtonGroup()
				self.bg1.addButton(self.cb3)
				self.bg1.addButton(self.cb4)
				self.hbox_LA.addWidget(self.cb3)
				self.hbox_LA.addWidget(self.cb4)
				self.bg1.buttonClicked.connect(self.btngroupLA)
				self.bgl1 = QLabel("Include last angle?")
				self.tabSino.layout.addRow(self.bgl1, self.hbox_LA)

	def btngroupLA(self, btn):
		if (btn.text() == 'Yes'):
			self.lAngle = True
		else:
			self.lAngle = False

	def addEntryFieldsButtons(self):

		# Create three buttons
		self.rst = QPushButton("Reset")
		self.rst.setToolTip('Reset all parameters to default values')
		self.upd = QPushButton("Save")
		self.upd.setToolTip('Save current parameters settings')
		self.prc = QPushButton("Preprocess")
		self.prc.setToolTip('Preprocess raw input files to generate sinograms and weight values')
		self.lod = QPushButton("Load")
		self.lod.setToolTip('Load exisitng parameter values from file')
		self.rst.clicked.connect(lambda:self.askAgain("reset"))
		self.upd.clicked.connect(self.handleUpdate)
		self.prc.clicked.connect(self.handlePreprocess)
		self.lod.clicked.connect(lambda:self.askAgain("load"))

		# Group them into a Widget
		self.buttons = QWidget()
		self.buttons.layout = QHBoxLayout()
		self.buttons.setLayout(self.buttons.layout)
		self.buttons.layout.addWidget(self.rst)
		self.buttons.layout.addWidget(self.lod)
		self.buttons.layout.addWidget(self.upd)
		self.buttons.layout.addWidget(self.prc)

		# Add both widget to the main window
		self.layout.addWidget(self.tabs)
		self.layout.addWidget(self.buttons)

	def askAgain(self, option):
		# pop final check message box
		if (option == "reset"):
			self.msg1 = QMessageBox()
			self.msg1.setIcon(QMessageBox.Warning)
			self.msg1.setText("Are you sure you want to reset the parameters?")
			self.msg1.setInformativeText("Note: All changes will be lost")
			self.msg1.setWindowTitle("Warning")
			self.msg1.setStandardButtons(QMessageBox.Yes | QMessageBox.Cancel)
			self.msg1.buttonClicked.connect(self.handleReset)
			self.msg1.exec_()
		elif (option == "load"):
			self.msg3 = QMessageBox()
			self.msg3.setIcon(QMessageBox.Warning)
			self.msg3.setText("Are you sure you want to load the parameters?")
			self.msg3.setInformativeText("Note: All unsaved changes will be lost")
			self.msg3.setWindowTitle("Warning")
			self.msg3.setStandardButtons(QMessageBox.Yes | QMessageBox.Cancel)
			self.msg3.buttonClicked.connect(self.handleLoad)
			self.msg3.exec_()

	def handleReset(self, btn):
		# print(btn.text())
		if(btn.text() == "&Yes"):
			# initiate inherent param first
			self.initParam()

			# initiate all text messages
			self.le_numV.setText(str(self.numViews))
			self.le_numC.setText(str(self.numChannels))
			self.le_numR.setText(str(self.numRows))
			self.le_startS.setText(str(self.rowStart))
			self.le_numRS.setText(str(self.numRowsRecon))
			self.le_daN.setText(self.setName)
			self.le_daFB.setText(self.inputFileBase)
			self.fl.setText(self.dataFolder)
			self.ll_nXV.setText(str(self.numPixX))
			self.ll_nYV.setText(str(self.numPixY))
			self.ll_nZV.setText(str(self.numPixZ))
			self.le_deltaR.setText(str(self.distRow))
			self.ll_distSliceV.setText(str(self.distSliceV))
			self.le_ROI.setText(str(self.radiReconP))
			self.le_deltaC.setText(str(self.distChan))
			self.le_CoR.setText(str(self.corOff))
			self.le_PV.setText(str(self.initPV))
			self.le_q.setText(str(self.q))
			self.le_p.setText(str(self.p))
			self.le_t.setText(str(self.t))
			self.le_sX.setText(str(self.sigX))
			self.le_wN.setText(str(self.priN))
			self.le_wD.setText(str(self.priD))
			self.le_wA.setText(str(self.priA))
			self.le_sT.setText(str(self.stopThres))
			self.le_mI.setText(str(self.maxIters))
			self.le_daFT.setText(self.fileType)
			self.le_fV.setText(str(self.theta_0))
			self.le_lV.setText(str(self.theta_end))
			self.cb2.setChecked(True)
			self.cb4.setChecked(True)
			self.tabData_w2.setHidden(False)

	def handleUpdate(self):
		# initiate error flag and error messages
		self.errFlag = 0
		self.errMsg = ''

		# get all current text messages
		self.numViews = self.getNum(self.le_numV.text(), "# of Views", 'posiInt')
		self.numChannels = self.getNum(self.le_numC.text(), "# of Channels", 'posiInt')
		self.numRows = self.getNum(self.le_numR.text(), "# of Rows", 'posiInt')
		self.rowStart = self.getNum(self.le_startS.text(), "First index of rows", 'posiInt')
		if (self.errFlag == 0) and (self.rowStart > self.numRows) :
			self.addErr("First index of rows should be smaller than # of rows")
		self.numRowsRecon = self.getNum(self.le_numRS.text(), "# of rows used to reconstruct", 'posiInt')
		if (self.errFlag == 0) and ((self.numRowsRecon + self.rowStart - 1) > self.numRows):
			self.addErr("The chosen range for rows to reconstruct is out of the total number of rows")
		self.setName = str(self.le_daN.text())
		self.inputFileBase = str(self.le_daFB.text())
		self.distRow = self.getNum(self.le_deltaR.text(), "Spacing between rows (mm)", 'posiFloat')
		self.radiReconP = self.getNum(self.le_ROI.text(), "Radius of reconstruction in percentage(%)", 'posiFloat')
		if (self.errFlag == 0):
			self.radiRecon = self.radiReconP/100*self.distRow*self.numRows/2
		self.distChan = self.getNum(self.le_deltaC.text(), "Spacing between channels (mm)", 'posiFloat')
		self.corOff	= self.getNum(self.le_CoR.text(), "CoR offset (units of channel)", 'Float')
		self.theta_0 = self.getNum(self.le_fV.text(), "First view angle", 'Float')
		self.theta_end = self.getNum(self.le_lV.text(), "Last view angle", 'Float')
		self.initPV = self.getNum(self.le_PV.text(), "linear attenuation ceoff (mm-1)", 'posiFloat')
		self.q = self.getNum(self.le_q.text(), "q-GGMRF param: q", 'posiFloat')
		self.p = self.getNum(self.le_p.text(), "q-GGMRF param: p", 'posiFloat')
		if (self.errFlag == 0) and ((self.p < 1) or (self.p > self.q)):
			self.addErr("The value for parameter p should be between 1 and parameter q")
		self.t = self.getNum(self.le_t.text(), "q-GGMRF param: T", 'posiFloat')
		self.sigX = self.getNum(self.le_sX.text(), "sigmaX (mm-1)", 'posiFloat')
		self.priN = self.getNum(self.le_wN.text(), "prior weight nearest", 'posiFloat')
		self.priD = self.getNum(self.le_wD.text(), "prior weight diag", 'posiFloat')
		self.priA = self.getNum(self.le_wA.text(), "prior weight adj slice", 'posiFloat')
		self.stopThres = self.getNum(self.le_sT.text(), "stop threshold", 'posiFloat')
		if (self.errFlag == 0) and (self.stopThres > 1):
			self.addErr("Stop threshold should be a number between 0 and 1")
		self.maxIters = self.getNum(self.le_mI.text(), "maximum num of iters", 'posiInt')
		self.paramDir = os.path.join(self.dataOutDir, 'parameters')
		self.fileType = str(self.le_daFT.text())
		self.numPixZ = self.numRowsRecon
		self.distSliceV	= self.distRow
		self.numPixX = self.numChannels
		self.numPixY = self.numChannels


		if (self.errFlag == 1):
			# Display all the errors on the message box
			self.showErrorMessages()

		else:
			# Rename all the template paramter files
			print("Renaming the parameter files if necessary...")
			if(os.path.isfile(os.path.join(self.paramDir, self.setName+'.imgparams')) == False):
				os.system("mv {dirOld:s} {dirNew:s}".format(dirOld = os.path.join(self.paramDir, '*.imgparams'), dirNew = os.path.join(self.paramDir, self.setName+'.imgparams')))
				os.system("mv {dirOld:s} {dirNew:s}".format(dirOld = os.path.join(self.paramDir, '*.reconparams'), dirNew = os.path.join(self.paramDir, self.setName+'.reconparams')))
				os.system("mv {dirOld:s} {dirNew:s}".format(dirOld = os.path.join(self.paramDir, '*.sinoparams'), dirNew = os.path.join(self.paramDir, self.setName+'.sinoparams')))
			print("Done")

			# change the actual parameter files in the folder
			print("Start changing the parameter files...")
			changeDataParam(self.numViews, self.numChannels, self.rowStart, self.numRowsRecon, self.setName, self.paramDir)
			changeMachineParam(self.numPixX, self.numPixY, self.distRow, self.radiRecon, self.distChan, self.corOff, self.pixWid, self.setName, self.paramDir)
			changeReconParam(self.initPV, self.q, self.p, self.t, self.sigX, self.priN, self.priD, self.priA, self.stopThres, self.maxIters, self.posConstraint, self.setName, self.paramDir)
			print("Done")

	def handleFolderChoose(self):
		self.currentFolder = os.getcwd().split("/")[-1]
		self.dataFolder = QFileDialog.getExistingDirectory(self, "Select Directory").split(self.currentFolder)[-1][1:] + "/"
		self.fl.setText(self.dataFolder)
		datafile = os.listdir(self.dataFolder)

		if (datafile == []):
			self.addErr("There are no input data in the specified 'Data folder'")
		else:
			temp = datafile[1]
			self.inputFileBase = temp.split('.')[0][:-4]
			self.fileType = temp.split('.')[1]
			self.le_daFB.setText(self.inputFileBase)
			self.le_daFT.setText(self.fileType)

	def handleLoad(self, btn):

		if(btn.text() == "&Yes"):

			self.setName = str(self.le_daN.text())
			self.errFlag = 0
			self.errMsg = ''

			fnImg = os.path.join(self.dataOutDir, 'parameters', self.setName+'.imgparams')
			fnSino = os.path.join(self.dataOutDir, 'parameters', self.setName+'.sinoparams')
			fnRecon = os.path.join(self.dataOutDir, 'parameters', self.setName+'.reconparams')

			if (os.path.isfile(fnImg) == False) or (os.path.isfile(fnSino) == False) or (os.path.isfile(fnRecon) == False):
				self.errFlag = 1
				self.addErr("There no existing parameter files by the giving data name and data folder")
				self.showErrorMessages()

			else:
				self.dataImg, self.dataSino, self.dataRecon = dataLoad(self.setName, self.paramDir)
				self.numViews 		= int(self.dataSino[4])
				self.numChannels 	= int(self.dataSino[1])
				# self.numRows 		= int(self.dataSino[4])
				self.rowStart 	= int(self.dataSino[19])
				self.numRowsRecon = int(self.dataSino[13])
				self.numPixX		= int(self.dataImg[1])
				self.numPixY		= int(self.dataImg[4])
				self.distRow		= float(self.dataImg[16])
				self.radiRecon		= float(self.dataImg[10])
				self.distChan		= float(self.dataSino[7])
				self.corOff			= float(self.dataSino[10])
				self.pixWid			= float(self.dataImg[7])
				self.initPV 		= float(self.dataRecon[1])
				self.q				= float(self.dataRecon[4])
				self.p				= float(self.dataRecon[7])
				self.t				= float(self.dataRecon[10])
				self.sigX			= float(self.dataRecon[13])
				self.priN			= float(self.dataRecon[16])
				self.priD			= float(self.dataRecon[19])
				self.priA			= float(self.dataRecon[22])
				self.stopThres		= float(self.dataRecon[25])
				self.maxIters		= int(self.dataRecon[28])

				self.le_numV.setText(str(self.numViews))
				self.le_numC.setText(str(self.numChannels))
				self.le_numR.setText("Cannot be loaded from param files")
				self.le_startS.setText(str(self.rowStart))
				self.le_numRS.setText(str(self.numRowsRecon))
				self.ll_nXV.setText(str(self.numPixX))
				self.ll_nYV.setText(str(self.numPixY))
				self.le_deltaR.setText(str(self.distRow))
				self.le_ROI.setText(str(self.radiRecon))
				self.le_deltaC.setText(str(self.distChan))
				self.le_CoR.setText(str(self.corOff))
				self.le_PV.setText(str(self.initPV))
				self.le_q.setText(str(self.q))
				self.le_p.setText(str(self.p))
				self.le_t.setText(str(self.t))
				self.le_sX.setText(str(self.sigX))
				self.le_wN.setText(str(self.priN))
				self.le_wD.setText(str(self.priD))
				self.le_wA.setText(str(self.priA))
				self.le_sT.setText(str(self.stopThres))
				self.le_mI.setText(str(self.maxIters))
				self.le_daFT.setText(self.fileType)
				self.cb1.setChecked(True)

	def handlePreprocess(self):
		# update all parameters first
		self.handleUpdate()

		if (self.errFlag == 0):
			if (os.path.isdir(os.path.join('.', self.dataOutDir, 'measurements'))):
				shutil.rmtree(os.path.join('.', self.dataOutDir, 'measurements'))

			# start preprocessing
			print("Generating sinogram and weights...")
			if self.preprocess_type == 1:
				preprocess(self.numViews, self.numChannels, self.numRows, self.rowStart, self.numRowsRecon, self.setName, self.inputFileBase, self.dataOutDir, self.dataFolder, self.writeView, self.lAngle, self.theta_0, self.theta_end, self.fileType)
			elif self.preprocess_type == 2:
				preprocess_xiao(self.numViews, self.numChannels, self.numRows, self.rowStart, self.numRowsRecon, self.setName, self.inputFileBase, self.dataOutDir, self.dataFolder, self.writeView, self.lAngle, self.theta_0, self.theta_end, self.fileType)
			print("Done")

			print("Generating tar.gz file...")
			os.chdir(os.path.join('.', self.dataOutDir))

			if (os.path.isfile(os.path.join('.', self.setName+'.tar.gz'))):
				os.remove(self.setName +'.tar.gz')
				# os.remove(self.setName +'.md5sum')

			createTarGzFile(self.setName)
			# generateMD5(self.setName)
			os.chdir('..')
			print("Done")

			# pop a message box
			self.msg2 = QMessageBox()
			self.msg2.setIcon(QMessageBox.Information)
			self.msg2.setStandardButtons(QMessageBox.Ok)
			self.msg2.setText("Preprocessing Done !")
			self.msg2.setInformativeText("Please upload the tar.gz file to the websites")
			self.msg2.exec_()

	def showErrorMessages(self):

		self.msg = QMessageBox()
		self.msg.setIcon(QMessageBox.Critical)
		self.msg.setText("There is at least one illegal choice of parameters")
		self.msg.setInformativeText("Click 'Show Details..' to see more")
		self.msg.setWindowTitle("Error")
		self.msg.setDetailedText(self.errMsg)
		self.msg.setStandardButtons(QMessageBox.Ok)
		self.msg.exec_()

	def getNum(self, rawin, errParam, option):
		if (option == 'posiInt'):
			try:
				num = int(rawin)
			except ValueError:
				self.addErr(errParam +" should be a positive integer")
			else:
				if (num <= 0):
					self.addErr(errParam +" should be a positive integer")
				else:
					return num

		elif (option == "posiFloat"):
			try:
				num = float(rawin)
			except ValueError:
				self.addErr(errParam +" should be a positive float number")
			else:
				if (num <= 0):
					self.addErr(errParam +" should be a positive float number")
				else:
					return num

		elif (option == "Float"):
			try:
				num = float(rawin)
			except ValueError:
				self.addErr(errParam +" should be a float number")
			else:
				return num

	def addErr(self, errMsg):
		self.errFlag = 1
		self.errMsg = self.errMsg + errMsg + "\n"

if __name__ == '__main__':

	app = QApplication(sys.argv)
	sc = PreprocessingGUI()
	sys.exit(app.exec_())
