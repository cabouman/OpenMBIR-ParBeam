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

/* ========================================================== */
/* MBIR RECONSTRUCTION SOFTWARE FOR 2D PARALLEL-BEAM X-RAY CT */
/*           by Pengchong Jin, Purdue University              */
/* ========================================================== */

#include <stdio.h>
#include <stdlib.h>

#include "../../../Utilities/3D/MBIRModularUtils_3D.h"
#include "../../../Utilities/2D/MBIRModularUtils_2D.h"
#include "../../../Utilities/MemoryAllocation/allocate.h"
#include "initialize.h"
#include "recon3d.h"

/* #ifdef STORE_A_MATRIX - This option set by default in A_comp.h */
/* This option is to precompute and store the forward matrix rather than compute it on the fly */

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


