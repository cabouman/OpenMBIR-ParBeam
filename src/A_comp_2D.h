#ifndef _ACOMP_2D_H_
#define _ACOMP_2D_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* strcmp */
#include <math.h>
#include <getopt.h> /* getopt */

#include "MBIRModularUtils_2D.h"
#include "allocate.h"

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
