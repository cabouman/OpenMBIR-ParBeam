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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <getopt.h>	/* getopt */

#include "../../../../Utilities/MemoryAllocation/allocate.h"
#include "../../../../Utilities/2D/MBIRModularUtils_2D.h"
#include "sino_and_weights.h"

void Compute_Sinogram_n_Weights(struct PhCount2D *phcount, struct Sino2DParallel *sinogram)
{
    printf("\nComputing Projections and Weights ... \n");
    
    /* Compute Sinogram and weights from photon count data */
    Compute_Sinogram(phcount, sinogram);
    Compute_Weights_from_Sinogram(sinogram);
}

void Compute_Sinogram(struct PhCount2D *phcount, struct Sino2DParallel *sinogram)
{
    struct SinoParams2DParallel *sinoparams;
    int i,M;
    float lambda_cal; /* lambda_cal : photon counts calibrated wrt blank scan and dark scan */
    
    sinoparams = &sinogram->sinoparams;
    M = sinoparams->NViews * sinoparams->NChannels;
    sinogram->sino = (float *)get_spc(M, sizeof(float));
    
    for (i = 0; i < M; i++)
    {
        lambda_cal = (phcount->lambda_object[i]-phcount->lambda_dark[i])/(phcount->lambda_blank[i]-phcount->lambda_dark[i]);
        sinogram->sino[i] = -log(lambda_cal) ;
    }
}

void Compute_Weights_from_Sinogram(struct Sino2DParallel *sinogram) /* Update vector sinogram->weight */
{
    struct SinoParams2DParallel *sinoparams;
    int i,M;
    float lambda_cal; /* photon counts calibrated wrt blank scan and dark scan */
    float sinoMean=0, sinoVar=0 ;/* Mean and variance of Projection measurements */
    
    sinoparams = &sinogram->sinoparams;
    M = sinoparams->NViews * sinoparams->NChannels;
    sinogram->weight = (float *)get_spc(M, sizeof(float));
    
    for (i = 0; i < M; i++)
    {
        lambda_cal = exp(-sinogram->sino[i]) ;
        sinogram->weight[i] = lambda_cal ; /* scale weights by sinogram variance in a next step */
        sinoMean += sinogram->sino[i] ;    /* calculate sinogram variance simultaneously */
    }
    sinoMean=sinoMean/M;
    
    /* Calculate variance of projections */
    for (i = 0; i < M; i++)
    {
     sinoVar += (sinogram->sino[i]-sinoMean)*(sinogram->sino[i]-sinoMean);
    }
    sinoVar = sinoVar/M;
    
    /* Scale weight by sinogram variance */
    if(sinoVar>0)
    for (i = 0; i < M; i++)
    sinogram->weight[i] = sinogram->weight[i]/sinoVar ;
}

void SetPhCountParams2D(struct SinoParams2DParallel sinoparams, struct PhCount2D *phcount)
{
    phcount->NViews = sinoparams.NViews;
    phcount->NChannels = sinoparams.NChannels;
}

/* The function to read Application-specific (photon-count) data for a given slice. The projections will later be computed from this data */
/* Currently, this data includes ... */
/* 1) Object dependent (bright) scan measurements */
/* 2) Blank scan (bright) measurements */
/* 3) Dark Scan (detector-offset calibration) measurements */
/* FUTURE WORK - this file must be in HDF5 format*/
int ReadPhCountData2D(char *fname, struct PhCount2D *phcount)
{
    FILE *fp;
    int len;
    
    strcat(fname, ".2Dphcountdata"); /* append file extension */
    
    if ((fp = fopen(fname, "r")) == NULL)
    {
        fprintf(stderr, "ReadPhCountData2D : can't open file %s.\n", fname);
        exit(-1);
    }
    
    len =  phcount->NViews * phcount->NChannels;
    
    if(fread(phcount->lambda_object,sizeof(float),len,fp)!=len)
    {
        fprintf(stderr, "ERROR in ReadPhCountData2D : file terminated early \n");
        fclose(fp);
        exit(1);
    }
    
    fseek(fp,2,SEEK_CUR); /* carriage return twice */
    if(fread(phcount->lambda_blank,sizeof(float),len,fp)!=len)
    {
        fprintf(stderr, "ERROR in ReadPhCountData2D : file terminated early \n");
        fclose(fp);
        exit(1);
    }
    
    fseek(fp,2,SEEK_CUR); /* carriage return twice */
    if(fread(phcount->lambda_dark,sizeof(float),len,fp)!=len)
    {
        fprintf(stderr, "ERROR in ReadPhCountData2D : file terminated early \n");
        fclose(fp);
        exit(1);
    }
    
    fclose(fp);
    
    return 0;
}

void AllocatePhcountData2D(struct PhCount2D *phcount)
{
    phcount->lambda_object = (float *)get_spc(phcount->NViews * phcount->NChannels, sizeof(float));
    phcount->lambda_blank  = (float *)get_spc(phcount->NViews * phcount->NChannels, sizeof(float));
    phcount->lambda_dark   = (float *)get_spc(phcount->NViews * phcount->NChannels, sizeof(float));
}

/* Free photon count data  memory allocation */
void FreePhCountData2D(struct PhCount2D *phcount)
{
    free((void *)phcount->lambda_object);
    free((void *)phcount->lambda_blank);
    free((void *)phcount->lambda_dark);
}

void readCmdLineSinoGen(int argc, char *argv[], struct CmdLineSinoGen *cmdline)
{
    char ch;
    
    if(argc<7)
    {
        if(argc==2 && CmdLineHelp(argv[1]))
        {
            fprintf(stdout,"\nHELP \n");
            PrintCmdLineUsage(argv[0]);
            exit(-1);
        }
        else
        {
            fprintf(stderr, "\nError : Improper Command line for exec-program %s, Number of arguments lower than needed \n",argv[0]);
            PrintCmdLineUsage(argv[0]);
            exit(-1);
        }
    }
    
    /* get options */
    while ((ch = getopt(argc, argv, "j:p:s:w:")) != EOF)
    {
        switch (ch)
        {
            case 'j':
            {
                sprintf(cmdline->sinoparamsFile, "%s", optarg);
                break;
            }
            case 'p':
            {
                sprintf(cmdline->phcountFile, "%s", optarg);
                break;
            }
            case 's':
            {
                sprintf(cmdline->sinoFile, "%s", optarg);
                break;
            }
            case 'w':
            {
                sprintf(cmdline->wghtFile, "%s", optarg);
                break;
            }	
            default:
            {
                fprintf(stderr, "\nError : Unrecognized Command-line Symbol for exec-program %s \n",argv[0]);
                PrintCmdLineUsage(argv[0]);
                exit(-1);
                break;
            }
        }
    }
}


void PrintCmdLineUsage(char *ExecFileName)
{
    fprintf(stdout, "\nBASELINE MBIR RECONSTRUCTION SOFTWARE FOR 2D PARALLEL-BEAM  CT \n");
    fprintf(stdout, "build time: %s, %s\n", __DATE__,  __TIME__);
    fprintf(stdout, "\nCommand line Format for Executable File %s : \n", ExecFileName);
    fprintf(stdout, "%s -j <InputFileName>[.sinoparams] -p <InputFileName>[.2Dphcountdata] -s <OutputFileName>[.2Dsinodata] -w <OutputFileName>[.2Dweightdata] \n",ExecFileName);
}


int CmdLineHelp(char *string)
{
    if( (strcmp(string,"-h")==0) || (strcmp(string,"-help")==0) || (strcmp(string,"--help")==0) || (strcmp(string,"help")==0) )
        return 1;
    else
        return 0;
}

