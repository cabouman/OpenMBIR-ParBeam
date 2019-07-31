#include "MBIRModularUtils_2D.h"
#include "allocate.h"
#include "A_comp_2D.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    struct CmdLineSysGen cmdline;
    struct ImageParams2D imgparams;
    struct SinoParams2DParallel sinoparams;
    float **PixelDetector_profile;
    struct SysMatrix2D *A ;
    
    /* read Command Line */
    readCmdLineSysGen(argc, argv, &cmdline);
	/* read input arguments and parameters */
	readParamsSysMatrix(&cmdline, &imgparams, &sinoparams);
    /* Compute Pixel-Detector Profile */
    PixelDetector_profile = ComputePixelProfile2DParallel(&sinoparams, &imgparams);  /* pixel-detector profile function */
    /* Compute System Matrix */
    A = ComputeSysMatrix2DParallel(&sinoparams, &imgparams, PixelDetector_profile);
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
