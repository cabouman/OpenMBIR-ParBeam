/* ========================================================== */
/* MBIR RECONSTRUCTION SOFTWARE FOR 2D PARALLEL-BEAM X-RAY CT */
/*           by Pengchong Jin, Purdue University              */
/* ========================================================== */

#include <stdio.h>
#include <stdlib.h>

#include "../../../src/MBIRModularUtils_2D.h"
#include "../../../src/allocate.h"
#include "sino_and_weights.h"


int main(int argc, char *argv[])
{
    struct PhCount2D phcount;        /* input  : application-specific measurement (photon-counts) data */
    struct Sino2DParallel sinogram;  /* output : projections data */
    struct CmdLineSinoGen cmdline;   /* command line arguments */
    
	/*** read input arguments and parameters ***/
	readCmdLineSinoGen(argc, argv, &cmdline); /* read command line arguments */
    
    /*** read in sinogram parameters ***/
    if(ReadSinoParams2DParallel(cmdline.sinoparamsFile, &sinogram.sinoparams))
    { fprintf(stderr,"Error in reading sinogram parameters \n");
      exit(-1);
    }
    
    /*** Allocate Memory ***/
    SetPhCountParams2D(sinogram.sinoparams, &phcount);
    AllocatePhcountData2D(&phcount);
    
    /** read in application-specific measurements (photon-counts) data **/
    if(ReadPhCountData2D(cmdline.phcountFile, &phcount))
    {
        fprintf(stderr,"Error in reading photon-count data \n");
        exit(-1);
    }
    /**** Compute Sinogram (Projections) and Weights ****/
    Compute_Sinogram_n_Weights(&phcount, &sinogram);
    
    /* Write Out Sinogram and Weights */
    if(WriteSino2DParallel(cmdline.sinoFile, &sinogram))
    {   fprintf(stderr,"Error in writing sinogram data \n");
        exit(-1);
    }
    if(WriteWeights2D(cmdline.wghtFile, &sinogram))
    {   fprintf(stderr,"Error in writing sinogram weights \n");
        exit(-1);
    }
    
    /* free allocated memory */
    FreePhCountData2D(&phcount);
    if(FreeSinoData2DParallel(&sinogram))
    {   fprintf(stderr,"Error in freeing memory allocated to sinogram \n");
        exit(-1);
    }
    
	return 0;
}
