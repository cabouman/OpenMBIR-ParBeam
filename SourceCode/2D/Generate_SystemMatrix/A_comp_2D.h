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

#ifndef _ACOMP_2D_H_
#define _ACOMP_2D_H_

/* Computation options */
#define WIDE_BEAM   /* Finite element analysis of detector channel, accounts for sensitivity variation across its aperture */
#define LEN_PIX 511 /* determines the spatial resolution for Detector-Pixel computation. Higher LEN_PIX, higher resolution */
                    /* In this implementation, spatial resolution is : [2*PixelDimension/LEN_PIX]^(-1) */
#define LEN_DET 101 /* No. of Detector Elements */
                    /* Each detector channel is "split" into LEN_DET smaller elements ... */
                    /* to account for detector sensitivity variation across its aperture */

/* Command Line structure for Generating System matrix */
struct CmdLineSysGen
{
    char imgparamsFileName[200];  /* input file */
    char sinoparamsFileName[200]; /* input file */
    char SysMatrixFileName[200]; /* output file */
};

/* Compute Pixel-Detector Profile for 2D Parallel Beam Geometry */
float **ComputePixelProfile2DParallel(struct SinoParams2DParallel *sinoparams, struct ImageParams2D *imgparams);
/* Compute System Matrix for 2D Parallel Beam Geometry*/
struct SysMatrix2D *ComputeSysMatrix2DParallel(struct SinoParams2DParallel *sinoparams, struct ImageParams2D *imgparams, float **pix_prof);
/* Read Command line */
void readCmdLineSysGen(int argc, char *argv[], struct CmdLineSysGen *cmdline);
/* Wrapper to read in system matrix parameters from Command line */
void readParamsSysMatrix(struct CmdLineSysGen *cmdline, struct ImageParams2D *imgparams, struct SinoParams2DParallel *sinoparams);
/* Print correct usage of Command Line*/
void PrintCmdLineUsage(char *ExecFileName);
int CmdLineHelp(char *string);

#endif
