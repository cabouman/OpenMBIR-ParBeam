% Read 2-D Image Data
% fname - input, base file name of 2-D image data (includes Directory path)
% imgaprams - input, image parameters
% imgdata - 2-D image slice, order image(Yindex, Xindex)

function [imgdata] = ReadImgData2D(fname, imgparams)

fname = strcat(fname,'.2Dimgdata');
fp = fopen(fname,'r');
% Image[iy*Nx+ix], y-slowest, x-fastest
imgdata=fread(fp,[imgparams.Nx imgparams.Ny],'float32'); 
% Permute 
imgdata=imgdata';

fclose(fp);

end