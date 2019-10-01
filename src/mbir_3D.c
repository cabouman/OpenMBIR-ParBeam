
#include <stdio.h>
#include <stdlib.h>

#include "MBIRModularDefs.h"
#include "MBIRModularUtils.h"
#include "allocate.h"
#include "initialize_3D.h"
#include "recon_3D.h"


int main(int argc, char *argv[])
{
	struct Image3D Image;
    struct Sino3DParallel sinogram;
    struct ReconParamsQGGMRF3D reconparams;
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
    if(AllocateSinoData3DParallel(&sinogram))
    {   fprintf(stderr, "Error in allocating sinogram data (and weights) memory through function AllocateSinoData3DParallel \n");
        exit(-1);
    }
    if(ReadSinoData3DParallel(cmdline.SinoDataFile, &sinogram))
    {   fprintf(stderr, "Error in reading sinogram data from file %s through function ReadSinoData3DParallel \n",cmdline.SinoDataFile);
        exit(-1);
    }
    if(ReadWeights3D(cmdline.SinoWeightsFile, &sinogram))
    {   fprintf(stderr, "Error in reading sinogram weights from file %s through function ReadWeights3D \n", cmdline.SinoWeightsFile);
        exit(-1);
    }
    
    /* Read System Matrix */
    A.Ncolumns = Image.imgparams.Nx * Image.imgparams.Ny;
    if(ReadSysMatrix2D(cmdline.SysMatrixFile,&A))
    {   fprintf(stderr, "Error in reading system matrix from file %s through function ReadSysMatrix2D \n",cmdline.SysMatrixFile);
        exit(-1);
    }
    
    /* Allocate memory for image */
    if(AllocateImageData3D(&Image))
    {   fprintf(stderr, "Error in allocating memory for image through function AllocateImageData3D \n");
        exit(-1);
    }
    /* Initialize image and reconstruction mask */
    InitValue = reconparams.InitImageValue;
    OutsideROIValue = 0;
    Initialize_Image(&Image, &cmdline, InitValue);
    ImageReconMask = GenImageReconMask(&Image,OutsideROIValue);
    
    /* MBIR - Reconstruction */
    MBIRReconstruct3D(&Image,&sinogram,reconparams,&A,ImageReconMask);
    
    /* Write out reconstructed image */
    if(WriteImage3D(cmdline.ReconImageDataFile, &Image))
    {
        fprintf(stderr, "Error in writing out reconstructed image file through function WriteImage3D \n");
        exit(-1);
    }
        
	/* free image, sinogram and system matrix memory allocation */
    if(FreeImageData3D(&Image))
    {  fprintf(stderr, "Error image memory could not be freed through function FreeImageData3D \n");
        exit(-1);
    }
    if(FreeSinoData3DParallel(&sinogram))
    {  fprintf(stderr, "Error sinogram memory could not be freed through function FreeSinoData3DParallel \n");
        exit(-1);
    }
    if(FreeSysMatrix2D(&A))
    {  fprintf(stderr, "Error System Matrix memory could not be freed through function FreeSysMatrix3D \n");
        exit(-1);
    }
    
    free((void *)ImageReconMask);
    
    return 0;
}


