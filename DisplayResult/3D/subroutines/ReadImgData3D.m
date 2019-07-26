% Read 3-D Image Data

% fname - input, base file name of image data (includes Directory path)
% imgaprams - input, image parameters
% SliceNumDigits - input, no. of digits used to specificy slice index
% imgdata - 3-D image (volume), order image(SliceIndex, Yindex, Xindex)

function [imgdata] = ReadImgData3D(fname, imgparams, SliceNumDigits)

Nx = imgparams.Nx;
Ny = imgparams.Ny;
Nz = imgparams.Nz;
FirstSliceIndex = imgparams.FirstSliceNumber;

LastSliceIndex = FirstSliceIndex+Nz-1;
imgdata = zeros(Nz,Ny,Nx);

% Read in Image data for each slice
for sliceindex = FirstSliceIndex:1:LastSliceIndex
slicefname = strcat(fname,'_slice');
slicefname = strcat(slicefname,sprintf('%.*d',SliceNumDigits,sliceindex));
imgdata(sliceindex-FirstSliceIndex+1,:,:)=ReadImgData2D(slicefname, imgparams);
end 

end