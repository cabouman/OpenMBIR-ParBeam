function [imgparams] = ReadImgParams3D(fname)

fp = fopen(fname,'r');

tag=fgets(fp,200);
imgparams.Nx=fscanf(fp,'%d\n');

tag=fgets(fp,200);
imgparams.Ny=fscanf(fp,'%d\n');

tag=fgets(fp,200);
imgparams.Deltaxy=fscanf(fp,'%f\n');

tag=fgets(fp,200);
imgparams.ROIRadius=fscanf(fp,'%f\n');

tag=fgets(fp,200);
imgparams.Nz=fscanf(fp,'%d\n');

tag=fgets(fp,200);
imgparams.DeltaZ=fscanf(fp,'%f\n');

tag=fgets(fp,200);
imgparams.FirstSliceNumber=fscanf(fp,'%d\n');

fclose(fp);

end