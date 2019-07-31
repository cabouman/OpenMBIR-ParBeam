#include <stdio.h>
#include <stdlib.h>

#include "MBIRModularUtils_2D.h"
#include "allocate.h"
#include "initialize_2D.h"
#include "recon_2D.h"


int main(int argc, char *argv[])
{
	struct Image2D Image;
    struct Sino2DParallel sinogram;
    struct ReconParamsQGGMRF2D reconparams;
    struct SysMatrix2D A;
    struct CmdLineMBIR cmdline;
    
    char *ImageReconMask; /* Image reconstruction mask (determined by ROI) */
    float InitValue ;     /* Image data initial condition is read in from a file if available ... */
                          /* else intialize it to a uniform image with value InitValue */
    float OutsideROIValue;/* Image pixel value outside ROI Radius */
    
	/* read command line */
	readCmdLineMBIR(argc, argv, &cmdline);
    
    /* read parameters */
    readSystemParams (&cmdline, &Image.imgparams, &sinogram.sinoparams, &reconparams);
    
    /* Read Sinogram and Weights */
    if(AllocateSinoData2DParallel(&sinogram))
    {   fprintf(stderr, "Error in allocating sinogram data (and weights) memory through function AllocateSinoData2DParallel \n");
        exit(-1);
    }
    if(ReadSinoData2DParallel(cmdline.SinoDataFile, &sinogram))
    {   fprintf(stderr, "Error in reading sinogram data from file %s through function ReadSinoData2DParallel \n",cmdline.SinoDataFile);
        exit(-1);
    }
    if(ReadWeights2D(cmdline.SinoWeightsFile, &sinogram))
    {   fprintf(stderr, "Error in reading sinogram weights from file %s through function ReadWeights2D \n", cmdline.SinoWeightsFile);
        exit(-1);
    }
    
    /* Read System Matrix */
    A.Ncolumns = Image.imgparams.Nx * Image.imgparams.Ny;
    if(ReadSysMatrix2D(cmdline.SysMatrixFile,&A))
    {   fprintf(stderr, "Error in reading system matrix from file %s through function ReadSysMatrix2D \n",cmdline.SysMatrixFile);
        exit(-1);
    }
    
    /* Allocate memory for image */
    if(AllocateImageData2D(&Image))
    {   fprintf(stderr, "Error in allocating memory for image through function AllocateImageData2D \n");
        exit(-1);
    }
    /* Initialize image and reconstruction mask */
    InitValue = reconparams.InitImageValue;
    OutsideROIValue = 0;
    Initialize_Image(&Image, &cmdline, InitValue);
    ImageReconMask = GenImageReconMask(&Image,OutsideROIValue);
    
    /* MBIR - Reconstruction */
    MBIRReconstruct2D(&Image,&sinogram,reconparams,&A,ImageReconMask);
    
    /* Write out reconstructed image */
    if(WriteImage2D(cmdline.ReconImageDataFile, &Image))
    {
        fprintf(stderr, "Error in writing out reconstructed image file through function WriteImage2D \n");
        exit(-1);
    }
        
	/* free image, sinogram and system matrix memory allcoation */
    if(FreeImageData2D(&Image))
    {  fprintf(stderr, "Error image memory could not be freed through function FreeImageData2D \n");
        exit(-1);
    }
    if(FreeSinoData2DParallel(&sinogram))
    {  fprintf(stderr, "Error sinogram memory could not be freed through function FreeSinoData2DParallel \n");
        exit(-1);
    }
    if(FreeSysMatrix2D(&A))
    {  fprintf(stderr, "Error System Matrix memory could not be freed through function FreeSysMatrix2D \n");
        exit(-1);
    }
    free((void *)ImageReconMask);
}


