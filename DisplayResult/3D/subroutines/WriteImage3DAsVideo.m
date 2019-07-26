% Write 3-D image as a Video

function WriteImage3DAsVideo(imgdata, imgparams, DisplayScale, VideoOutputFname)

Nz = imgparams.Nz;
Ny = imgparams.Ny;
Nx = imgparams.Nx;
FirstSliceIndex = imgparams.FirstSliceNumber;

% Open video to write out Video
v = VideoWriter(VideoOutputFname,'MPEG-4');
v.FrameRate=1;
open(v);

for i=1:Nz

sliceindex = FirstSliceIndex+i-1;
SliceImgData = squeeze(imgdata(i,:,:));

DisplayParams.XAxisLabel = 'X-Axis';
DisplayParams.YAxisLabel = 'Y-Axis';
DisplayParams.Title = sprintf('$ Image \\  Slice \\  %d \\  \\left( Units: \\  mm^{-1} \\right) $',sliceindex);
DisplayParams.DisplayScale = DisplayScale;

fig = DisplayImage2D(SliceImgData,DisplayParams);
print(fig,'TempFigure','-dtiffn');
close ; % close figure

VideoImg = imread('TempFigure.tif');
writeVideo(v,VideoImg);
delete('TempFigure.tif');

end

close(v); % close video

end