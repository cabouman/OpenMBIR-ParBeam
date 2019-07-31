#ifndef _ICD_3D_H_
#define _ICD_3D_H_

#include <math.h>

#include "MBIRModularUtils_3D.h"
#include "MBIRModularUtils_2D.h"
#include <stdio.h>
#include <stdlib.h>

struct ICDInfo
{
    int VoxelIndex ; /* Index of Voxel being updated */
    float v; /* current pixel value */
    float neighbors[10]; /* Currently 10-point neighborhood system */
    
	float theta1; /* Quadratic surrogate function parameters -theta1 and theta2 */
	float theta2;
    
    struct ReconParamsQGGMRF3D Rparams; /* Reconstruction Parameters (includes prior parameters) */
    
    int Nxy;    /* Number of pixels within a given slice */
    int NViewsTimesNChannels;   /* Number of projection measurements per slice */
};

float ICDStep3D(float **e, float **w, struct SysMatrix2D *A, struct ICDInfo *icd_info);

/* Prior-specific, independent of neighborhood */
float QGGMRF_SurrogateCoeff(float delta, struct ICDInfo *icd_info);
float QGGMRF_Potential(float delta, struct ReconParamsQGGMRF3D *Rparams);
/* Prior and neighborhood specific */
void  QGGMRF3D_UpdateICDParams(struct ICDInfo *icd_info);
/* Only neighborhood specific */
void ExtractNeighbors3D(struct ICDInfo *icd_info, struct Image3D *X);

/* Update error term e=y-Ax after an ICD update on x */
void UpdateError3D(float **e, struct SysMatrix2D *A, float diff, struct ICDInfo *icd_info);

#endif
