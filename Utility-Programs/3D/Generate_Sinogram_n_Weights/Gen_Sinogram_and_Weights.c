#include <stdio.h>
#include <stdlib.h>

#include "../../../src/MBIRModularUtils_3D.h"
#include "../../../src/MBIRModularUtils_2D.h"
#include "../../../src/allocate.h"
#include "sino_and_weights.h"


int main(int argc, char *argv[])
{
    struct PhCount3D phcount;         /* input  : application-specific measurement (photon-counts) data */
    struct Sino3DParallel sinogram;   /* output : projections data */
    struct CmdLineSinoGen cmdline;    /* command line arguments */
    
	/*** read input arguments and parameters ***/
	readCmdLineSinoGen(argc, argv, &cmdline); /* read command line arguments */
    
    /*** read in sinogram parameters ***/
    if(ReadSinoParams3DParallel(cmdline.sinoparamsFile, &sinogram.sinoparams))
    { fprintf(stderr,"Error in reading sinogram parameters \n");
        exit(-1);
    }
    
    /*** Allocate memory **/
    SetPhCountParams3D(sinogram.sinoparams, &phcount);
    AllocatePhcountData3D(&phcount);
    
    /** read in application-specific measurements (photon-counts) data **/
    ReadPhCountData3D(cmdline.phcountFile, &phcount); /* read in photon count data */
    
    /**** Compute Sinogram (Projections) and Weights ****/
    Compute_Sinogram_n_Weights(&phcount, &sinogram);
    
    /* Write Out Sinogram and Weights */
    if(WriteSino3DParallel(cmdline.sinoFile, &sinogram))
    {   fprintf(stderr,"Error in writing sinogram data \n");
        exit(-1);
    }
    if(WriteWeights3D(cmdline.wghtFile, &sinogram))
    {   fprintf(stderr,"Error in writing sinogram weights \n");
        exit(-1);
    }
    /* free allocated memory */
    FreePhCountData3D(&phcount);
    if(FreeSinoData3DParallel(&sinogram))
    {   fprintf(stderr,"Error in freeing memory allocated to sinogram \n");
        exit(-1);
    }
    
	return 0;
}
