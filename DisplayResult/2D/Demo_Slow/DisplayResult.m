
% Display 2D-Image
clear 
clc
% Add path for subroutines
addpath('../subroutines')

% File Path for Image Parameters, Data  and Display Scale
ImgParamsFname = '../../../Data/2D/Demo_Slow/parameters/afrl.imgparams';
ImgDataFname   = '../../../Data/2D/Demo_Slow/Reconstruction/afrl.2Dimgdata';
ImgTitleName   = 'afrl.2Dimg';
DisplayScale   = [0 50] ;  %units is -1^mm

% Read Parameters, Data and Display Image
imgparams = ReadImgParams(ImgParamsFname);
imgdata   = ReadImgData(ImgDataFname, imgparams);
DisplayImage2D(imgdata,DisplayScale,ImgTitleName);