
% Display 3D-Image through a video, where slices are displayed sequentially
% After executing this code, manually play the resulting video named 'SlicesDisplay.mp4' in current directory

clear 
clc
% Add path for subroutines
addpath('../subroutines')

% Arguments 
ImageBaseName='afrl'; 
ImageDirName= '../../../Data/3D/Demo_Fast/Reconstruction/Image/'; % Directory in which image files for each slice are located
ImageParamsFname = '../../../Data/3D/Demo_Fast/Parameters/afrl.imgparams'; % Image Parameters File Name
SliceNumDigits=4; % Number of digits to represent slice-index in image file-name format
VideoOutputFname = 'SlicesDisplay.mp4'; % Video which displays each slice as a frame
DisplayScale = [0 50]; % Image Display scale 

% Read image parameters
imgparams=ReadImgParams3D(ImageParamsFname);

% Read image data
fname = strcat(ImageDirName,ImageBaseName);
Image = ReadImgData3D(fname, imgparams, SliceNumDigits);

% Write Out 3-D image as a video
WriteImage3DAsVideo(Image, imgparams, DisplayScale, VideoOutputFname);

% Manually play the resulting video named 'SlicesDisplay.mp4' in current directory