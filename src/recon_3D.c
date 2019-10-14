
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "MBIRModularDefs.h"
#include "MBIRModularUtils.h"
#include "allocate.h"
#include "icd_3D.h"
#include "recon_3D.h"

#define EPSILON 0.0000001

/* The MBIR algorithm  */
/* Note : */
/* 1) Image must be intialized before this function is called */
/* 2) Image reconstruction Mask must be generated before this call */

void MBIRReconstruct3D(
                       struct Image3D *Image,
                       struct Sino3DParallel *sinogram,
                       struct ReconParamsQGGMRF3D reconparams,
                       struct SysMatrix2D *A,
                       char *ImageReconMask )
{
    int it, MaxIterations, j, jz, k, l, Nx, Ny, Nz, Nxy, N, i, XYPixelIndex, SliceIndex, M;
    time_t start;
    float **x;  /* image data (SliceIndex, XYPixelIndex) */
    float **y;  /* sinogram projections data  */
    float **e;  /* e=y-Ax, error */
    float **w;  /* projections weights data */
  
    float voxel, diff;
    float cost, TotalValueChange, avg_update, TotalVoxelValue, AvgVoxelValue, StopThreshold, ratio;
    char zero_skip_FLAG;
    char stop_FLAG;
    int *order;
    int NumUpdatedVoxels;
    
    struct ICDInfo icd_info; /* Local Cost Function Information */
    
    x = Image->image;   /* x is the image vector */
    y = sinogram->sino;   /* y is the sinogram projections vector  */
    w = sinogram->weight; /* w is the vector of sinogram weights */
    Nx = Image->imgparams.Nx;
    Ny = Image->imgparams.Ny;
    Nz = Image->imgparams.Nz;
    N  = Nx*Ny*Nz;
    Nxy= Nx*Ny;
    M = sinogram->sinoparams.NViews * sinogram->sinoparams.NChannels ;
    
    /********************************************/
    /* Forward Projection and Error Calculation */
    /********************************************/
    e = (float **)multialloc(sizeof(float),2,Nz,M);	 /* error term memory allocation */
    /* Initialize error to zero, since it is first computed as forward-projection Ax */
    for (jz = 0; jz < Nz; jz++)
    for (i = 0; i < M; i++)
    e[jz][i]=0;
    /* compute Ax (store it in e as of now) */
    forwardProject3D(e, Image, A);
    /* Compute the initial error e=y-Ax */
    for (jz = 0; jz < Nz; jz++)
    for (i = 0; i < M; i++)
    e[jz][i] = y[jz][i]-e[jz][i];
  
    /****************************************/
    /* Iteration and convergence Parameters */
    /****************************************/
    icd_info.Rparams = reconparams;
    icd_info.Nxy = Nxy;
    icd_info.NViewsTimesNChannels = sinogram->sinoparams.NViews * sinogram->sinoparams.NChannels ;
    
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
        fprintf(stdout, "**Iteration %-3d **\n", it+1);  /* it is the current iteration number */
        
        shuffle(order, N);        /* shuffle the coordinate and update pixels randomly */
                                  /* Random update provides the fastest algorithmic convergence. */
        
        TotalValueChange = 0.0; /* sum of absolute change in value of all pixels */
        NumUpdatedVoxels=0; /* number of updated pixels */
        TotalVoxelValue=0;
        
        for (l = 0; l < N; l++)
        {
            if(i%100==0)
            {
                printf("\r\tProgress = %2.f %%", (float)l/N*100.0); fflush(stdout);
            }
            j = order[l];          /* Voxel index from randomized list */
            XYPixelIndex = j%Nxy ; /* Pixel Index within a given slice */
            SliceIndex = j/Nxy ;   /* Slice Index*/

            if (ImageReconMask[XYPixelIndex]) /* Pixel is within ROI */
            {
                /*****ICD - Local Cost Function Parameters *******/
                icd_info.v = x[SliceIndex][XYPixelIndex];  /* store the voxel value before update */
                icd_info.VoxelIndex = j;                    /* Index of voxel to be updated */
                ExtractNeighbors3D(&icd_info, Image);  /* extract voxel neighorborhood */
                
                /* Skip update only if Pixel=0, PixelNeighborhood=0 and System-matrix column for that pixel is a 0 vector */
                zero_skip_FLAG = 0;
                      
                /* Here use if(fabs(a) <= EPSILON)  instead of if(a==0.0) when a is float, where EPSILON is a very small float close to 0 */
                if (fabs(icd_info.v) <= EPSILON && A->column[XYPixelIndex].Nnonzero==0)
                {
                    zero_skip_FLAG = 1;	/* If all 11 pixels in the neighborhood system is zero. Then skip this pixel update */
                    for (k = 0; k < 10; k++)
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
                        voxel = ICDStep3D(e, w, A, &icd_info);  /* pixel is the updated pixel value */
                        x[SliceIndex][XYPixelIndex] = ((voxel < 0.0) ? 0.0 : voxel);  /* clip to non-negative */
                        diff = x[SliceIndex][XYPixelIndex] - icd_info.v;
                        TotalValueChange += fabs(diff);
                        UpdateError3D(e, A, diff, &icd_info);   /* update the error term e= e - A * delta(x) */
                    
                        TotalVoxelValue += icd_info.v ; /* using previous pixel value here */
                        NumUpdatedVoxels++ ;
                }
                
            }
        }
        
        
         cost = MAPCostFunction3D(e, Image, sinogram, &reconparams);
         if(NumUpdatedVoxels>0)
         {
           avg_update = TotalValueChange/NumUpdatedVoxels;
           AvgVoxelValue = TotalVoxelValue/NumUpdatedVoxels;
           if(AvgVoxelValue>0)
           ratio = (avg_update/AvgVoxelValue)*100;
         }
         else
         avg_update=0;
        
        fprintf(stdout, "cost = %-15f, Average Update = %f mm^{-1} \n", cost, avg_update);
        
        if (ratio < StopThreshold || NumUpdatedVoxels==0)
        {
            stop_FLAG = 1;
        }
    }
    
    fprintf(stdout, "\nTime elapsed in Iterative reconstruction is %f seconds\n", difftime(time(NULL), start));
    
    if (stop_FLAG == 1)
    {
        fprintf(stdout, "Reached stopping condition.\n");
    }
    else if (StopThreshold> 0)
    {
        fprintf(stdout, "WARNING: Didn't reach stopping condition.\n");
    }
    
    if(AvgVoxelValue>0)
    fprintf(stdout, "Average Update to Average Voxel-Value Ratio = %f %% \n", ratio);
    
    
    free((void *)order);
}


/* The function to compute cost function */
float MAPCostFunction3D(
	float **e,
    struct Image3D *Image,
	struct Sino3DParallel *sinogram,
    struct ReconParamsQGGMRF3D *reconparams)
{
	int i, M, jx, jy, jz, jxy, Nx, Ny, Nz, Nxy, plusx, minusx, plusy, plusz ;
    float **x ;
    float **w ;
	float nloglike, nlogprior_nearest, nlogprior_diag, nlogprior_interslice ;
    
    x = Image->image;
    w = sinogram->weight;
    
	M = sinogram->sinoparams.NViews * sinogram->sinoparams.NChannels ;
	Nx = Image->imgparams.Nx;
	Ny = Image->imgparams.Ny;
    Nz = Image->imgparams.Nz; 

    Nxy = Nx*Ny;
    
	nloglike = 0.0;

    for (jz = 0; jz < Nz; jz++)
	for (i = 0; i < M; i++)
    nloglike += e[jz][i]*w[jz][i]*e[jz][i];

	nloglike /= 2.0;
	nlogprior_nearest = 0.0;
	nlogprior_diag = 0.0;
    nlogprior_interslice = 0.0;
    
   for (jz = 0; jz < Nz; jz++)
  {
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
            plusz = jz + 1;
            plusz = ((plusz < Nz) ? plusz : 0);

			jxy = jy*Nx + jx; /* XY pixel Index */

            /* Trick to avoid computing the contribution of pair-wise cliques twice */
            nlogprior_nearest += QGGMRF_Potential((x[jz][jxy] - x[jz][jy*Nx+plusx]), reconparams);
            nlogprior_nearest += QGGMRF_Potential((x[jz][jxy] - x[jz][plusy*Nx+jx]),reconparams);

            nlogprior_diag += QGGMRF_Potential((x[jz][jxy] - x[jz][plusy*Nx+minusx]),reconparams);
            nlogprior_diag += QGGMRF_Potential((x[jz][jxy] - x[jz][plusy*Nx+plusx]),reconparams);
            
            nlogprior_interslice += QGGMRF_Potential((x[jz][jxy] - x[plusz][jxy]),reconparams);
		}
	}
  }

	return (nloglike + reconparams->b_nearest * nlogprior_nearest + reconparams->b_diag * nlogprior_diag + reconparams->b_interslice * nlogprior_interslice) ;
}


/* compute A times X, A-matrix is pre-computed */
void forwardProject3D(
                      float **AX,           /* Note : Vector AX must be initiliazed to zero */
                      struct Image3D *X,
                      struct SysMatrix2D *A)
{
    int j,k,n, jz, Nxy, NSlices ;
    struct SparseColumn A_column;
    float AValue;
    
    printf("\nComputing Forward Projection ... \n");
    
    Nxy = X->imgparams.Nx * X->imgparams.Ny; /* No. of pixels within a single slice */
    NSlices = X->imgparams.Nz;
    
    if(A->Ncolumns != Nxy)
    {
      fprintf(stderr,"Error in forwardProject3D : dimensions of System Matrix and Image are not compatible \n");
      exit(-1);
    }
    

    for (j = 0; j < A->Ncolumns; j++) /* j is the PixelIndex within a single XY-slice, independent of slice index */
    {
        A_column = A->column[j]; /* As system matrix does not vary with slice for 3-D parallel beam geometry */
        if (A_column.Nnonzero > 0)
        {
            for (n = 0; n < A_column.Nnonzero; n++)
            {
                k = A_column.RowIndex[n]  ; /* (View,Detector-Channel) pair  */
                AValue = A_column.Value[n];

                for(jz=0;jz<NSlices;jz++)   /* vary slice index */
                {
                  AX[jz][k] += AValue*X->image[jz][j] ; /* Voxel index = j+jz*Nxy */
                }
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
