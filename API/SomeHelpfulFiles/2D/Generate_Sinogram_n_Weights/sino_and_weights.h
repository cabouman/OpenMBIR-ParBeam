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

#ifndef _SINOANDWEIGHTS_H_
#define _SINOANDWEIGHTS_H_

struct CmdLineSinoGen{
    char sinoparamsFile[200];
    char phcountFile[200];
    char sinoFile[200] ;
    char wghtFile[200] ;
};

struct PhCount2D
{
    int NViews;
    int NChannels;
    float *lambda_object; /* Object dependent bright-scan */
    float *lambda_blank;  /* Blank Scan */
    float *lambda_dark;   /* Dark Scan - detector offset */
};

void Compute_Sinogram_n_Weights(struct PhCount2D *phcount, struct Sino2DParallel *sinogram);
void Compute_Sinogram(struct PhCount2D *phcount, struct Sino2DParallel *sinogram);
void Compute_Weights_from_Sinogram(struct Sino2DParallel *sinogram);

void SetPhCountParams2D(struct SinoParams2DParallel sinoparams, struct PhCount2D *phcount);
int  ReadPhCountData2D(char *fname, struct PhCount2D *phcount);
void AllocatePhcountData2D(struct PhCount2D *phcount);
void FreePhCountData2D(struct PhCount2D *phcount);

void readCmdLineSinoGen(int argc, char *argv[], struct CmdLineSinoGen *cmdline);
void PrintCmdLineUsage(char *ExecFileName);
int CmdLineHelp(char *string);

#endif
