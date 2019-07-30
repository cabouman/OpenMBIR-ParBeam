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

#include "../../../../Utilities/2D/MBIRModularUtils_2D.h"
#include "../../../../Utilities/MemoryAllocation/allocate.h"
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
