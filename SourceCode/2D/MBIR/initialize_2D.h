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

#ifndef _INITIALIZE_2D_H_
#define _INITIALIZE_2D_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h> /* strcmp */
#include <getopt.h> /* getopt */

#include "../../MBIRModularUtils_2D.h"
#include "../../allocate.h"

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


void Initialize_Image(struct Image2D *Image, struct CmdLineMBIR *cmdline, float InitValue);
void GenConstImage(struct Image2D *Image, float value);
char *GenImageReconMask (struct Image2D *Image, float OutsideROIValue);
void readSystemParams ( struct CmdLineMBIR *cmdline, struct ImageParams2D *imgparams, struct SinoParams2DParallel *sinoparams, struct ReconParamsQGGMRF2D *reconparams);
void NormalizePriorWeights2D(struct ReconParamsQGGMRF2D *reconparams);
void readCmdLineMBIR(int argc, char *argv[], struct CmdLineMBIR *cmdline);
void PrintCmdLineUsage(char *ExecFileName);
int CmdLineHelp(char *string);

#endif
