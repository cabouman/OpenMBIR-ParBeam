/* ============================================================================== 
 * Copyright (c) 2016 Xiao Wang (Purdue University)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Xiao Wang, Purdue University,
 * nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ============================================================================== */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>	/* strcmp */
#include <getopt.h>	/* getopt */

#include "../../../Utilities/2D/MBIRModularUtils_2D.h"
#include "../../../Utilities/MemoryAllocation/allocate.h"
#include "initialize.h"

/* Initialize constant image */
void Initialize_Image(struct Image2D *Image, struct CmdLineMBIR *cmdline, float InitValue)
{
    if(strcmp(cmdline->InitImageDataFile,"NA") == 0) /* Image file not available */
        GenConstImage(Image, InitValue);               /* generate image with uniform pixel value */
    else
        ReadImage2D(cmdline->InitImageDataFile, Image); /* read image which has values in HU units */
}

/* create constant image. Each pixel value is the intial condition. */
void GenConstImage(struct Image2D *Image, float value)
{
    int j, N;
    N = Image->imgparams.Nx * Image->imgparams.Ny ;
    
    for (j = 0; j < N; j++)
    {
        Image->image[j] = value;
    }
}

/* Generate Image Reconstruction mask */
char *GenImageReconMask (struct Image2D *Image, float OutsideROIValue)
{
    int jx, jy, Nx, Ny;
    float x_0, y_0, Deltaxy, x, y, yy, ROIRadius, R_sq, R_sq_max;
    char *ImageReconMask;
    
    Nx = Image->imgparams.Nx;
    Ny = Image->imgparams.Ny;
    Deltaxy = Image->imgparams.Deltaxy;
    ROIRadius = Image->imgparams.ROIRadius;
    
    x_0 = -(Nx-1)*Deltaxy/2;
    y_0 = -(Ny-1)*Deltaxy/2;
    
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
            {
                ImageReconMask[jy*Nx+jx] = 0;
                Image->image[jy*Nx+jx] = OutsideROIValue;
            }
            else
            {
                ImageReconMask[jy*Nx+jx] = 1;
            }
        }
     }
    }
    
    return ImageReconMask;
}


/* Normalize weights to sum to 1 */
/* Only neighborhood specific */
void NormalizePriorWeights2D(
                         struct ReconParamsQGGMRF2D *reconparams)
{
    double sum;
    
    /* All neighbor weights must sum to one, assuming 8 pt neighborhood */
    sum = 4.0*reconparams->b_nearest + 4.0*reconparams->b_diag ;
    
    reconparams->b_nearest /= sum;
    reconparams->b_diag /= sum;
}

/* Wrapper to read in Image, sinogram and reconstruction parameters */
void readSystemParams  (
                         struct CmdLineMBIR *cmdline,
                         struct ImageParams2D *imgparams,
                         struct SinoParams2DParallel *sinoparams,
                         struct ReconParamsQGGMRF2D *reconparams)
{
    printf("\nReading Image, Sinogram and Reconstruction Parameters ... \n");
    
    if(ReadImageParams2D(cmdline->ImageParamsFile, imgparams))
        printf("Error in reading image parameters \n");
    
    if(ReadSinoParams2DParallel(cmdline->SinoParamsFile, sinoparams))
        printf("Error in reading sinogram parameters \n");
    
    if(ReadReconParamsQGGMRF2D(cmdline->ReconParamsFile ,reconparams))
        printf("Error in reading reconstruction parameters \n");
    
    /* Tentatively initialize weights. Remove once this is read in directly from params file */
    NormalizePriorWeights2D(reconparams);
    
    /* Print paramters */
    printImageParams2D(imgparams);
    printSinoParams2DParallel(sinoparams);
    printReconParamsQGGMRF2D(reconparams);
}

/* Read Command-line */
void readCmdLineMBIR(int argc, char *argv[], struct CmdLineMBIR *cmdline)
{
    char ch;
    
    strcpy(cmdline->InitImageDataFile, "NA"); /* default */
    
    if(argc<15)
    {
        if(argc==2 && CmdLineHelp(argv[1]))
        {
            fprintf(stdout,"\nHELP \n");
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
    while ((ch = getopt(argc, argv, "i:j:k:m:s:w:r:v")) != EOF)
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
            case 'v':
            {
                sprintf(cmdline->InitImageDataFile, "%s", optarg);
                break;
            }
            default:
            {
                printf("\nError : Command line Symbol not recongized by readParamsSysMatrix \n");
                PrintCmdLineUsage(argv[0]);
                exit(-1);
                break;
            }
        }
    }

}

void PrintCmdLineUsage(char *ExecFileName)
{
    fprintf(stdout, "\nBASELINE MBIR RECONSTRUCTION SOFTWARE FOR 2D PARALLEL-BEAM  CT \n");
    fprintf(stdout, "build time: %s, %s\n", __DATE__,  __TIME__);
    fprintf(stdout, "\nCommand line Format for Executable File %s : \n", ExecFileName);
    fprintf(stdout, "%s -i <InputFileName>[.imgparams] -j <InputFileName>[.sinoparams]  -k <InputFileName>[.reconparams] \
-m <InputFileName>[.2Dsysmatrix] -s <InputFileName>[.2Dsinodata] -w <InputFileName>[.2Dweightdata] -r <OutputFileName>[.2Dimgdata] \n",ExecFileName);
    fprintf(stdout, "Additional option (for initial image): -v <InitialImageFileName>[.2Dimgdata] \n");
    fprintf(stdout, "Note : The necessary extensions for certain input files are mentioned above within a \"[]\" symbol \n");
    fprintf(stdout, "However, they are NOT to be included as part of the file name in the command line arguments. \n");
    fprintf(stdout, "\nAlso see sample files run.sh under Data/Demo_Fast/ and Data/Demo_Slow/ for the correct format \n \n");
}

int CmdLineHelp(char *string)
{
    if( (strcmp(string,"-h")==0) || (strcmp(string,"-help")==0) || (strcmp(string,"--help")==0) || (strcmp(string,"help")==0) )
        return 1;
    else
        return 0;
}


