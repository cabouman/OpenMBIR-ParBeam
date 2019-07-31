
% Display 2D-Image
clear 
clc
% Add path for subroutines
addpath('../subroutines')

% File Path for Image Parameters, Data  and Display Scale
ImgParamsFname = '../../../Data/2D/Demo_Fast/parameters/shepp.imgparams';
ImgDataFname   = '../../../Data/2D/Demo_Fast/Reconstruction/shepp.2Dimgdata';
ImgTitleName   = 'Shepp Logan Phantom Reconstruction';
DisplayScale   = [0 0.03] ;   %units is -1^mm

% Read Parameters, Data and Display Image
imgparams = ReadImgParams(ImgParamsFname);
imgdata   = ReadImgData(ImgDataFname, imgparams);
DisplayImage2D(imgdata,DisplayScale,ImgTitleName);

