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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "../../MBIRModularUtils_2D.h"
#include "../../allocate.h"
#include "icd_2D.h"
#include "recon_2D.h"

#define EPSILON 0.0000001

/* The MBIR algorithm  */
/* Note : */
/* 1) Image must be intialized before this function is called */
/* 2) Image reconstruction Mask must be generated before this call */

void MBIRReconstruct2D(
                       struct Image2D *Image,
                       struct Sino2DParallel *sinogram,
                       struct ReconParamsQGGMRF2D reconparams,
                       struct SysMatrix2D *A,
                       char *ImageReconMask )
{
    int it, MaxIterations, j, k, l, Nx, Ny, N, i, M;
    time_t start;
    float *x;  /* image data */
    float *y;  /* sinogram projections data */
    float *e;  /* e=y-Ax, error */
    float *w;  /* projections weights data */
  
    float pixel, diff;
    float cost, TotalValueChange, avg_update, TotalPixelValue, AvgPixelValue, StopThreshold, ratio;
    char zero_skip_FLAG;
    char stop_FLAG;
    int *order;
    int NumUpdatedPixels;
    
    struct ICDInfo icd_info; /* Local Cost Function Information */
    
    x = Image->image;   /* x is the image vector */
    y = sinogram->sino;  /* y is the sinogram vector */
    w = sinogram->weight; /* vector of weights for each sinogram measurement */
    Nx = Image->imgparams.Nx;
    Ny = Image->imgparams.Ny;
    N = Nx*Ny;
    M = sinogram->sinoparams.NViews * sinogram->sinoparams.NChannels;
    
    /********************************************/
    /* Forward Projection and Error Calculation */
    /********************************************/
    e = (float *)get_spc(M, sizeof(float));	 /* error term memory allocation */
    /* Initialize error to zero, since it is first computed as forward-projection Ax */
    for (i = 0; i < M; i++)
    e[i]=0;
    /* compute Ax (store it in e as of now) */
    forwardProject2D(e, Image, A);
    /* Compute the initial error e=y-Ax */
    for (i = 0; i < M; i++)
    e[i] = y[i]-e[i];
    
    /****************************************/
    /* Iteration and convergence Parameters */
    /****************************************/
    icd_info.Rparams = reconparams;
    MaxIterations = reconparams.MaxIterations;
    StopThreshold = reconparams.StopThreshold;
    
    /* Order of pixel updates need NOT be raster order, just initialize */
    order = (int *)get_spc(N, sizeof(int));
    for (j = 0; j < N; j++)
    order[j] = j;
    
    stop_FLAG = 0;
    start = time(NULL);  /* XW: starting time */
    
    /****************************************/
    /* START iterative RECONSTRUCTION       */
    /****************************************/

    printf("\nStarting Iterative Reconstruction ... \n\n");
    for (it = 0; ((it < MaxIterations) && (stop_FLAG == 0)); it++)
    {
        fprintf(stdout, "** Iteration %-3d **\n", it+1);  /* it is the current iteration number */
        
        shuffle(order, N);  /* shuffle the coordinate and update pixels randomly */
                            /* Random update provides the fastest algorithmic convergence. */
        
        TotalValueChange = 0.0; /* sum of absolute change in value of all pixels */
        NumUpdatedPixels=0; /* number of updated pixels */
        TotalPixelValue=0;
        
        for (l = 0; l < N; l++)
        {
            j = order[l];
            if (ImageReconMask[j]) /* Pixel is within ROI */
            {
                /*****ICD - Local Cost Function Parameters *******/
                icd_info.v = x[j];  /* store the voxel value before update */
                icd_info.PixelIndex = j; /* Index of Pixel to be updated */
                ExtractNeighbors2D(&icd_info, Image);  /* extract pixel neighorborhood */
                
                /* Skip update only if Pixel=0, PixelNeighborhood=0 and System-matrix column for that pixel is a 0 vector */
                zero_skip_FLAG = 0;

                /* Here use if(fabs(a) <= EPSILON)  instead of if(a==0.0) when a is float, where EPSILON is a very small float close to 0 */
                if (icd_info.v <= EPSILON && A->column[j].Nnonzero==0)
                {
                    zero_skip_FLAG = 1;	/* If all 9 pixels in the neighborhood system is zero. Then skip this pixel update */
                    for (k = 0; k < 8; k++)
                    {
                        if (icd_info.neighbors[k] > EPSILON) /* is neighbor non-zero */
                        {
                            zero_skip_FLAG = 0;
                            break;
                        }
                    }
                }
                
                if (zero_skip_FLAG == 0)
                {
                        pixel = ICDStep2D(e, w, A, &icd_info);  /* pixel is the updated pixel value */
                        x[j] = ((pixel < 0.0) ? 0.0 : pixel);  /* clip to non-negative */
                        diff = x[j] - icd_info.v;
                        TotalValueChange += fabs(diff);
                        UpdateError2D(e, A, diff, &icd_info);   /* update the error term e= e - A * delta(x) */
                    
                        TotalPixelValue += icd_info.v ; /* using previous pixel value here */
                        NumUpdatedPixels++ ;
                }
              }
          }
        
         cost = MAPCostFunction2D(e, Image, sinogram, &reconparams);
         if(NumUpdatedPixels>0)
         {
           avg_update = TotalValueChange/NumUpdatedPixels;
           AvgPixelValue = TotalPixelValue/NumUpdatedPixels;
           if(AvgPixelValue>0)
           ratio = (avg_update/AvgPixelValue)*100;
         }
         else
         avg_update=0;
        
        fprintf(stdout, "cost = %-15f, Average Update = %f mm^{-1} \n", cost, avg_update);
        
        if (ratio < StopThreshold || NumUpdatedPixels==0)
        {
            stop_FLAG = 1;
        }
    }
    
    fprintf(stdout, "\nTime elapsed in Iterative reconstruction is %d seconds\n", (int) difftime(time(NULL), start));
    
    if (stop_FLAG == 1)
    {
        fprintf(stdout, "Reached stopping condition.\n");
    }
    else if (StopThreshold> 0)
    {
        fprintf(stdout, "WARNING: Didn't reach stopping condition.\n");
    }
    
    if(AvgPixelValue>0)
    fprintf(stdout, "Average Update to Average Pixel-Value Ratio = %f %% \n", ratio);
    
    
    free((void *)order);
}


/* The function to compute cost function */
float MAPCostFunction2D(
	float *e,
    struct Image2D *Image,
	struct Sino2DParallel *sinogram,
    struct ReconParamsQGGMRF2D *reconparams)
{
	int i, M, j, jx, jy, Nx, Ny, plusx, minusx, plusy;
    float *x ;
    float *w ;
	float nloglike, nlogprior_nearest, nlogprior_diag;
    
    x = Image->image;
    w = sinogram->weight;
    
	M = sinogram->sinoparams.NViews * sinogram->sinoparams.NChannels;
	Nx = Image->imgparams.Nx;
	Ny = Image->imgparams.Ny;

	nloglike = 0.0;
	for (i = 0; i < M; i++)
    nloglike += e[i]*w[i]*e[i];

	nloglike /= 2.0;
	nlogprior_nearest = 0.0;
	nlogprior_diag = 0.0;
    
	for (jy = 0; jy < Ny; jy++)
	{
		for (jx = 0; jx < Nx; jx++)
		{
			plusx = jx + 1;
			plusx = ((plusx < Nx) ? plusx : 0);
			minusx = jx - 1;
			minusx = ((minusx < 0) ? Nx-1 : minusx);
			plusy = jy + 1;
			plusy = ((plusy < Ny) ? plusy : 0);

			j = jy*Nx + jx;

            /* Trick to avoid computing the contribution of pair-wise cliques twice */
            nlogprior_nearest += QGGMRF_Potential((x[j] - x[jy*Nx+plusx]), reconparams);
            nlogprior_nearest += QGGMRF_Potential((x[j] - x[plusy*Nx+jx]),reconparams);

            nlogprior_diag += QGGMRF_Potential((x[j] - x[plusy*Nx+minusx]),reconparams);
            nlogprior_diag += QGGMRF_Potential((x[j] - x[plusy*Nx+plusx]),reconparams);
		}
	}

	return (nloglike + reconparams->b_nearest * nlogprior_nearest + reconparams->b_diag * nlogprior_diag);
}


/* compute A times X, A-matrix is pre-computed */
void forwardProject2D(
                      float *AX,            /* Note : Vector AX must be initiliazed to zero */
                      struct Image2D *X,
                      struct SysMatrix2D *A)
{
    int j,n ;
    struct SparseColumn A_column;
    
    printf("\nComputing Forward Projection ... \n");
    
    if(A->Ncolumns != X->imgparams.Nx * X->imgparams.Ny)
        printf("Error in forwardProject2D : dimensions of System Matrix and Image are not compatible \n");
    
    for (j = 0; j < A->Ncolumns; j++)
    {
        A_column = A->column[j]; /* jth column */
        if (A_column.Nnonzero > 0)
        {
            for (n = 0; n < A_column.Nnonzero; n++)
            {
                AX[A_column.RowIndex[n]] += A_column.Value[n]*X->image[j];
            }
        }
    }
}


/* shuffle the coordinate to enable random update */
void shuffle(int *order, int len)
{
    int i, j, tmp;
    
    srand(time(NULL));
    
    for (i = 0; i < len-1; i++)
    {
        j = i + (rand() % (len-i));
        tmp = order[j];
        order[j] = order[i];
        order[i] = tmp;
    }
}
