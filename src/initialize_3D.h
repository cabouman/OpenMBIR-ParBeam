#ifndef _INITIALIZE_3D_H_
#define _INITIALIZE_3D_H_

#include "MBIRModularDefs.h"

struct CmdLineMBIR{
    char ReconType;		/* 1:QGGMRF, 2:PandP */
    char SinoParamsFile[200];
    char ImageParamsFile[200];
    char ReconParamsFile[200];
    char SinoDataFile[200];
    char SinoWeightsFile[200];
    char ReconImageDataFile[200]; /* output */
    char SysMatrixFile[200];
    char InitImageDataFile[200]; /* optional input */
    char ProxMapImageDataFile[200]; /* optional input */
};

void Initialize_Image(
	struct Image3D *Image,
	struct CmdLineMBIR *cmdline,
	char *ImageReconMask,
	float InitValue,
	float OutsideROIValue);
char *GenImageReconMask(struct ImageParams3D *imgparams);
void readSystemParams(
	struct CmdLineMBIR *cmdline,
	struct ImageParams3D *imgparams,
	struct SinoParams3DParallel *sinoparams,
	struct ReconParams *reconparams);
void NormalizePriorWeights3D(struct ReconParams *reconparams);
void readCmdLineMBIR(int argc, char *argv[], struct CmdLineMBIR *cmdline);
void PrintCmdLineUsage(char *ExecFileName);
int CmdLineHelp(char *string);

#endif
