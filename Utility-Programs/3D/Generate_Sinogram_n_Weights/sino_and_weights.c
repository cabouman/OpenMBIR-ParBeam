#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <getopt.h>	/* getopt */

#include "../../../src/MBIRModularUtils_3D.h"
#include "../../../src/MBIRModularUtils_2D.h"
#include "../../../src/allocate.h"
#include "sino_and_weights.h"

void Compute_Sinogram_n_Weights(struct PhCount3D *phcount, struct Sino3DParallel *sinogram)
{
    printf("\nComputing Projections and Weights ... \n");
    
    /* Compute Sinogram and weights from photon count data */
    Compute_Sinogram(phcount, sinogram);
    Compute_Weights_from_Sinogram(sinogram);
}

void Compute_Sinogram(struct PhCount3D *phcount, struct Sino3DParallel *sinogram)
{
    struct SinoParams3DParallel *sinoparams;
    int i, jz, M, NSlices;
    float lambda_cal; /* lambda_cal : photon counts calibrated wrt blank scan and dark scan */
    
    sinoparams = &sinogram->sinoparams;
    M = sinoparams->NViews * sinoparams->NChannels;
    NSlices =  sinoparams->NSlices;

    sinogram->sino = (float **)multialloc(sizeof(float),2,NSlices,M);
    
    for (jz = 0 ; jz < NSlices ; jz++)
    for (i = 0; i < M; i++)
    {
        lambda_cal = (phcount->lambda_object[jz][i]-phcount->lambda_dark[jz][i])/(phcount->lambda_blank[jz][i]-phcount->lambda_dark[jz][i]);
        sinogram->sino[jz][i] = -log(lambda_cal) ;
    }
}

void Compute_Weights_from_Sinogram(struct Sino3DParallel *sinogram) /* Update vector sinogram->weight */
{
    struct SinoParams3DParallel *sinoparams;
    int i, jz, M, NSlices;
    float lambda_cal; /* photon counts calibrated wrt blank scan and dark scan */
    float sinoMean=0, sinoVar=0 ;/* Mean and variance of Projection measurements */
    
    sinoparams = &sinogram->sinoparams;
    M = sinoparams->NViews * sinoparams->NChannels ;
    NSlices =  sinoparams->NSlices;

    sinogram->weight = (float **)multialloc(sizeof(float),2,NSlices,M); 
    
    for (jz = 0 ; jz < NSlices ; jz++)
    for (i = 0; i < M; i++)
    {
        lambda_cal = exp(-sinogram->sino[jz][i]) ;
        sinogram->weight[jz][i] = lambda_cal ; /* scale weights by sinogram variance in a next step */
        sinoMean += sinogram->sino[jz][i] ;    /* calculate sinogram variance simultaneously */
    }
    sinoMean=sinoMean/(M*NSlices);
    
    /* Calculate variance of projections */
    for (jz = 0 ; jz < NSlices ; jz++)
    for (i = 0; i < M; i++)
    {
     sinoVar += (sinogram->sino[jz][i]-sinoMean)*(sinogram->sino[jz][i]-sinoMean);
    }
    sinoVar = sinoVar/(M*NSlices);
    
    /* Scale weight by sinogram variance */
    if(sinoVar>0){
    for (jz = 0 ; jz < NSlices ; jz++)    
    for (i = 0; i < M; i++)
    sinogram->weight[jz][i] = sinogram->weight[jz][i]/sinoVar ;
    }
}


void SetPhCountParams3D(struct SinoParams3DParallel sinoparams, struct PhCount3D *phcount)
{
    phcount->NSlices = sinoparams.NSlices;
    phcount->NViews = sinoparams.NViews;
    phcount->NChannels = sinoparams.NChannels;
    phcount->FirstSliceNumber = sinoparams.FirstSliceNumber;
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


void ReadPhCountData3D(char *fname, struct PhCount3D *phcount)
{
    
    char slicefname[200];
    char *sliceindex;
    int i,NSlices,NChannels,NViews,FirstSliceNumber;
    struct PhCount2D SingleSlicePhCount ;
    
    strcat(fname,"_slice"); /* <fname>_slice */
    sliceindex= (char *)malloc(MBIR_MODULAR_MAX_NUMBER_OF_SLICE_DIGITS);
    
    NSlices = phcount->NSlices;
    NChannels = phcount->NChannels;
    NViews = phcount->NViews;
    FirstSliceNumber=phcount->FirstSliceNumber;
    
    /* copy single slice information */
    SingleSlicePhCount.NChannels = NChannels;
    SingleSlicePhCount.NViews = NViews;
    
    printf("\nReading 3-D Photon Count Data - Bright (Object-dependent), Blank and Dark Scans ... \n");
    
    for(i=0;i<NSlices;i++)
    {
        SingleSlicePhCount.lambda_object = phcount->lambda_object[i];  /* pointer to beginning of data for i-th slice */
        SingleSlicePhCount.lambda_blank  = phcount->lambda_blank[i];   /* pointer to beginning of data for i-th slice */
        SingleSlicePhCount.lambda_dark   = phcount->lambda_dark[i];    /* pointer to beginning of data for i-th slice */
        
        /* slice index : integer to string conversion with fixed precision */
        sprintf(sliceindex,"%.*d",MBIR_MODULAR_MAX_NUMBER_OF_SLICE_DIGITS,i+FirstSliceNumber);
        
        /* Obtain file name for the given slice */
        strcpy(slicefname,fname);
        strcat(slicefname,sliceindex); /* append slice index */
        
        if(ReadPhCountData2D(slicefname, &SingleSlicePhCount))
        {   fprintf(stderr, "Error in ReadPhCountData3D : Unable to read sinogram data for slice %d from file %s \n",i,slicefname);
            exit(-1);
        }
    }
    free((void *)sliceindex);   
}

void AllocatePhcountData3D(struct PhCount3D *phcount)
{
    phcount->lambda_object = (float **)multialloc(sizeof(float),2,phcount->NSlices,phcount->NViews * phcount->NChannels);
    phcount->lambda_blank  = (float **)multialloc(sizeof(float),2,phcount->NSlices,phcount->NViews * phcount->NChannels);
    phcount->lambda_dark   = (float **)multialloc(sizeof(float),2,phcount->NSlices,phcount->NViews * phcount->NChannels);
}

/* Free photon count data  memory allocation */
void FreePhCountData3D(struct PhCount3D *phcount)
{
    multifree(phcount->lambda_object,2);
    multifree(phcount->lambda_blank,2);
    multifree(phcount->lambda_dark,2);
}

void readCmdLineSinoGen(int argc, char *argv[], struct CmdLineSinoGen *cmdline)
{
    char ch;
    
    if(argc<9)
    {
        if(argc==2 && CmdLineHelp(argv[1]))
        {
            fprintf(stdout,"\n=========HELP==========\n");
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
    fprintf(stdout, "\nBASELINE MBIR RECONSTRUCTION SOFTWARE FOR 3D PARALLEL-BEAM  CT \n");
    fprintf(stdout, "build time: %s, %s\n", __DATE__,  __TIME__);
    fprintf(stdout, "\nCommand line Format for Executable File %s : \n", ExecFileName);
    fprintf(stdout, "%s -j <InputFileName>[.sinoparams] -p <InputPhotonCountsBaseFileName> -s <OutputProjectionsBaseFileName> -w <OutputWeightsBaseFileName> \n",ExecFileName);
}


int CmdLineHelp(char *string)
{
    if( (strcmp(string,"-h")==0) || (strcmp(string,"-help")==0) || (strcmp(string,"--help")==0) || (strcmp(string,"help")==0) )
        return 1;
    else
        return 0;
}

