
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "MBIRModularDefs.h"
#include "MBIRModularUtils.h"
#include "allocate.h"
#include "initialize_3D.h"

/* Initialize image state */
void Initialize_Image(
	struct Image3D *Image,
	struct CmdLineMBIR *cmdline,
	char *ImageReconMask,
	float InitValue,
	float OutsideROIValue)
{
    int j,jz;
    int Nxy = Image->imgparams.Nx * Image->imgparams.Ny;
    int Nz = Image->imgparams.Nz;

    fprintf(stdout, "\nInitializing Image ... \n");

    if(strcmp(cmdline->InitImageDataFile,"NA") == 0) /* Image file not available */
    {
        /* Generate constant image */
        for(jz=0; jz<Nz; jz++)
        for(j=0; j<Nxy; j++)
        if(ImageReconMask[j]==0) 
            Image->image[jz][j] = OutsideROIValue;
        else
            Image->image[jz][j] = InitValue;
    }
    else 
        ReadImage3D(cmdline->InitImageDataFile, Image);

}

/* Allocate and generate Image Reconstruction mask */
char *GenImageReconMask(struct ImageParams3D *imgparams)
{
    int jx, jy, Nx, Ny;
    float x_0, y_0, Deltaxy, x, y, yy, ROIRadius, R_sq, R_sq_max;
    char *ImageReconMask;
    
    Nx = imgparams->Nx;
    Ny = imgparams->Ny;
    Deltaxy = imgparams->Deltaxy;
    ROIRadius = imgparams->ROIRadius;
    x_0 = -(Nx-1)*Deltaxy/2;
    y_0 = -(Ny-1)*Deltaxy/2;
    
    /* Reconstruction Mask same for each slice, hence 2-D */
    ImageReconMask = (char *)get_spc(Ny*Nx,sizeof(char));
    
    if (ROIRadius < 0.0)
    {
        printf("Error in GenImageReconMask : Invalid Value for Radius of Reconstruction \n");
        exit(-1);
    }
    else
    {
        R_sq_max = ROIRadius*ROIRadius;
        
        for (jy = 0; jy < Ny; jy++)
        {
            y = y_0 + jy*Deltaxy;
            yy = y*y;
            for (jx = 0; jx < Nx; jx++)
            {
                x = x_0 + jx*Deltaxy;
                R_sq = x*x + yy;
                if (R_sq > R_sq_max)
                    ImageReconMask[jy*Nx+jx] = 0;
                else
                    ImageReconMask[jy*Nx+jx] = 1;
            }
        }
    }
    
    return ImageReconMask;
}


/* Normalize weights to sum to 1 */
/* Only neighborhood specific */
void NormalizePriorWeights3D(
                         struct ReconParams *reconparams)
{
    double sum;
    
    /* All neighbor weights must sum to one, assuming 8 pt neighborhood */
    sum = 4.0*reconparams->b_nearest + 4.0*reconparams->b_diag + 2.0*reconparams->b_interslice;
    
    reconparams->b_nearest /= sum;
    reconparams->b_diag /= sum;
    reconparams->b_interslice /= sum;
}

/* Wrapper to read in Image, sinogram and reconstruction parameters */
void readSystemParams  (
                         struct CmdLineMBIR *cmdline,
                         struct ImageParams3D *imgparams,
                         struct SinoParams3DParallel *sinoparams,
                         struct ReconParams *reconparams)
{
    //printf("\nReading Image, Sinogram and Reconstruction Parameters ... \n");
    
    if(ReadImageParams3D(cmdline->ImageParamsFile, imgparams)) {
        fprintf(stderr,"Error in reading image parameters \n");
        exit(-1);
    }
    printImageParams3D(imgparams);

    if(ReadSinoParams3DParallel(cmdline->SinoParamsFile, sinoparams)) {
        fprintf(stderr,"Error in reading sinogram parameters \n");
        exit(-1);
    }
    printSinoParams3DParallel(sinoparams);

    /* Read Recon parameters */
    if(ReadReconParams(cmdline->ReconParamsFile,reconparams))
    {
        fprintf(stderr,"Error in reading reconstruction parameters\n");
        exit(-1);
    }

    if(cmdline->ReconType == MBIR_MODULAR_RECONTYPE_QGGMRF_3D)
    {
        NormalizePriorWeights3D(reconparams);
        printReconParamsQGGMRF3D(reconparams);
    }
    if(cmdline->ReconType == MBIR_MODULAR_RECONTYPE_PandP)
    {
        printReconParamsPandP(reconparams);
    }

    fprintf(stdout,"\n");

    if(reconparams->ReconType != cmdline->ReconType)
    {
        fprintf(stdout,"Warning** \"PriorModel\" field in reconparams file doesn't agree with the\n");
        fprintf(stdout,"Warning** command line says you want to do. Proceeding anyway.\n");
        reconparams->ReconType = cmdline->ReconType;
    }

    /* Determine and SET number of slice index digits in data files */
    int Ndigits = NumSinoSliceDigits(cmdline->SinoDataFile, sinoparams->FirstSliceNumber);
    if(Ndigits==0)
    {
        int i;
        fprintf(stderr,"Error: Can't read the first data file. Looking for any one of the following:\n");
        for(i=MBIR_MODULAR_MAX_NUMBER_OF_SLICE_DIGITS; i>0; i--)
            fprintf(stderr,"\t%s_slice%.*d.2Dsinodata\n",cmdline->SinoDataFile, i, sinoparams->FirstSliceNumber);
        exit(-1);
    }
    //printf("Detected %d slice index digits\n",Ndigits);
    sinoparams->NumSliceDigits = Ndigits;
    imgparams->NumSliceDigits = Ndigits;

}

/* Read Command-line */
void readCmdLineMBIR(int argc, char *argv[], struct CmdLineMBIR *cmdline)
{
    char ch;
    
    /* set defaults */
    strcpy(cmdline->InitImageDataFile, "NA"); /* default */
    cmdline->ReconType = MBIR_MODULAR_RECONTYPE_QGGMRF_3D;
    
    if(argc<15)
    {
        if(argc==2 && CmdLineHelp(argv[1]))
        {
            fprintf(stdout,"\n=========HELP==========\n");
            PrintCmdLineUsage(argv[0]);
            exit(-1);
        }
        else
        {
         fprintf(stderr, "\nError : Improper Command line for exec-program %s, Number of arguments lower than needed \n",argv[0]);
         PrintCmdLineUsage(argv[0]);
         exit(-1);
        }
    }
    
    /* get options */
    while ((ch = getopt(argc, argv, "i:j:k:m:s:w:r:t:p:v")) != EOF)
    {
        switch (ch)
        {
            case 'i':
            {
                sprintf(cmdline->ImageParamsFile, "%s", optarg);
                break;
            }
            case 'j':
            {
                sprintf(cmdline->SinoParamsFile, "%s", optarg);
                break;
            }
            case 'k':
            {
                sprintf(cmdline->ReconParamsFile, "%s", optarg);
                break;
            }
            case 'm':
            {
                sprintf(cmdline->SysMatrixFile, "%s", optarg);
                break;
            }
            case 's':
            {
                sprintf(cmdline->SinoDataFile, "%s", optarg);
                break;
            }
            case 'w':
            {
                sprintf(cmdline->SinoWeightsFile, "%s", optarg);
                break;
            }
            case 'r':
            {
                sprintf(cmdline->ReconImageDataFile, "%s", optarg);
                break;
            }
            case 't':
            {
                sprintf(cmdline->InitImageDataFile, "%s", optarg);
                break;
            }
            case 'p':
            {
                sprintf(cmdline->ProxMapImageDataFile, "%s", optarg);
                cmdline->ReconType = MBIR_MODULAR_RECONTYPE_PandP;
                break;
            }
            // Reserve this for verbose-mode flag
            case 'v':
            {
                break;
            }
            default:
            {
                fprintf(stderr,"Error : Command line symbol not recongized\n");
                PrintCmdLineUsage(argv[0]);
                exit(-1);
                break;
            }
        }
    }

}

void PrintCmdLineUsage(char *ExecFileName)
{
    fprintf(stdout, "\nBASELINE MBIR RECONSTRUCTION SOFTWARE FOR 3D PARALLEL-BEAM  CT\n");
    fprintf(stdout, "build time: %s, %s\n", __DATE__,  __TIME__);
    fprintf(stdout, "\nCommand line Format for Executable File %s :\n", ExecFileName);
    fprintf(stdout, "%s -i <InputFileName>[.imgparams] -j <InputFileName>[.sinoparams]\n",ExecFileName);
    fprintf(stdout, "   -k <InputFileName>[.reconparams] -m <InputFileName>[.2Dsysmatrix]\n");
    fprintf(stdout, "   -s <InputProjectionsBaseFileName> -w <InputWeightsBaseFileName>\n");
    fprintf(stdout, "   -r <OutputImageBaseFileName>\n\n");
    fprintf(stdout, "Additional options:\n");
    fprintf(stdout, "   -t <InitialImageBaseFileName>   # Read initial image\n");
    fprintf(stdout, "   -p <ProxMapImageBaseFileName>   # Read/run Proximal Map prior\n\n");
    fprintf(stdout, "Note : The necessary extensions for certain input files are mentioned above within\n");
    fprintf(stdout, "a \"[]\" symbol above, however the extensions should be OMITTED in the command line\n\n");
    fprintf(stdout, "The following instructions pertain to the -s, -w and -r options:\n");
    fprintf(stdout, "A) The Sinogram Projection data files should be stored slice by slice in a single\n");
    fprintf(stdout, "   directory. All files within this directory must share a common BaseFileName and\n");
    fprintf(stdout, "   adhere to the following format :\n");
    fprintf(stdout, "      <ProjectionsBaseFileName>_slice<SliceIndex>.2Dsinodata\n");
    fprintf(stdout, "   where \"SliceIndex\" is a non-negative integer indexing each slice and is printed\n");
    fprintf(stdout, "   with a fixed number of digits. Eg. 000 to 999 OR 0000 to 9999 would be valid.\n");
    fprintf(stdout, "B) Similarly, the format for the Sinogram Weights files is :\n");
    fprintf(stdout, "      <WeightsBaseFileName>_slice<SliceIndex>.2Dweightdata\n");
    fprintf(stdout, "C) Similarly, the Reconstructed (Output) Image is organized slice by slice :\n");
    fprintf(stdout, "      <ImageBaseFileName>_slice<SliceIndex>.2Dimgdata\n\n");
}

int CmdLineHelp(char *string)
{
    if( (strcmp(string,"-h")==0) || (strcmp(string,"-help")==0) || (strcmp(string,"--help")==0) || (strcmp(string,"help")==0) )
        return 1;
    else
        return 0;
}


