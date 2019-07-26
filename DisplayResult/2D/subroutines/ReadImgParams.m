function [imgparams] = ReadImgParams(fname)

fp = fopen(fname,'r');

tag=fgets(fp,200);
imgparams.Nx=fscanf(fp,'%d\n');

tag=fgets(fp,200);
imgparams.Ny=fscanf(fp,'%d\n');

tag=fgets(fp,200);
imgparams.Deltaxy=fscanf(fp,'%f\n');

tag=fgets(fp,200);
imgparams.ROIRadius=fscanf(fp,'%f\n');

fclose(fp);

end