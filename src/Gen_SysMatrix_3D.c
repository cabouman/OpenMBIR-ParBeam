#include "MBIRModularUtils_3D.h"
#include "MBIRModularUtils_2D.h"
#include "allocate.h"
#include "A_comp_3D.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


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
