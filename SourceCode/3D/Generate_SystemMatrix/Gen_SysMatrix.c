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

#include "../../../Utilities/3D/MBIRModularUtils_3D.h"
#include "../../../Utilities/2D/MBIRModularUtils_2D.h"
#include "../../../Utilities/MemoryAllocation/allocate.h"
#include "A_comp.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/* #ifdef STORE_A_MATRIX - This option set by default in A_comp.h */
/* This option is to precompute and store the forward matrix rather than compute it on the fly */

int main(int argc, char *argv[])
{
    struct CmdLineSysGen cmdline;
    struct ImageParams3D imgparams;
    struct SinoParams3DParallel sinoparams;
    float **PixelDetector_profile;
    struct SysMatrix2D *A ;
    
    /* read Command Line */
    readCmdLineSysGen(argc, argv, &cmdline);
	/* read input arguments and parameters */
	readParamsSysMatrix(&cmdline, &imgparams, &sinoparams);
    
    fprintf(stdout, "\nGeneration of System Matrix takes about 8 minutes for this Demo. Please Wait patiently ... \n");
    
    /* Compute Pixel-Detector Profile */
    PixelDetector_profile = ComputePixelProfile3DParallel(&sinoparams, &imgparams);  /* pixel-detector profile function */
    /* Compute Forward Matrix */
    A = ComputeSysMatrix3DParallel(&sinoparams, &imgparams, PixelDetector_profile);
    
    
    /* Write out System Matrix */
    if(WriteSysMatrix2D(cmdline.SysMatrixFileName, A))
    {  fprintf(stderr, "Error in writing out System Matrix to file %s through function WriteSysMatrix2D \n", cmdline.SysMatrixFileName);
       exit(-1);
    }
    /* Free System Matrix */
    if(FreeSysMatrix2D(A))
    {  fprintf(stderr, "Error System Matrix memory could not be freed through function FreeSysMatrix2D \n");
       exit(-1);
    }
    
	return 0;
}
