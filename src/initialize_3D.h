#ifndef _INITIALIZE_3D_H_
#define _INITIALIZE_3D_H_


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h> /* strcmp */
#include <getopt.h> /* getopt */

#include "MBIRModularUtils_3D.h"
#include "MBIRModularUtils_2D.h"
#include "allocate.h"

struct CmdLineMBIR{
    
    char ImageParamsFile[200];
    char InitImageDataFile[200]; /* optional input */
    char ReconImageDataFile[200]; /* output */
    char SinoParamsFile[200];
    char SinoDataFile[200];
    char SinoWeightsFile[200];
    char ReconParamsFile[200];
    char SysMatrixFile[200];
};


void Initialize_Image(struct Image3D *Image, struct CmdLineMBIR *cmdline, float InitValue);
void GenConstImage(struct Image3D *Image, float value);
char *GenImageReconMask (struct Image3D *Image, float OutsideROIValue);
void readSystemParams ( struct CmdLineMBIR *cmdline, struct ImageParams3D *imgparams, struct SinoParams3DParallel *sinoparams, struct ReconParamsQGGMRF3D *reconparams);
void NormalizePriorWeights3D(struct ReconParamsQGGMRF3D *reconparams);
void readCmdLineMBIR(int argc, char *argv[], struct CmdLineMBIR *cmdline);
void PrintCmdLineUsage(char *ExecFileName);
int CmdLineHelp(char *string);

#endif
