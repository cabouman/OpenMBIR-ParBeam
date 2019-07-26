% Read 2-D Image Data

function [imgdata] = ReadImgData(fname, imgparams)

fp = fopen(fname,'r');
% Image[iy*Nx+ix], y-slowest, x-fastest
imgdata=fread(fp,[imgparams.Nx imgparams.Ny],'float32'); 
% Permute 
imgdata=imgdata';

fclose(fp);

end