#ifndef _ICD_2D_H_
#define _ICD_2D_H_

#include <math.h>

#include "MBIRModularUtils_2D.h"
#include <stdio.h>
#include <stdlib.h>

struct ICDInfo
{
    int PixelIndex ; /* Index of Pixel being updated */
    float v; /* current pixel value */
    float neighbors[8]; /* Currently 8-point neighborhood system */
    
	float theta1; /* Quadratic surrogate function parameters -theta1 and theta2 */
	float theta2;
    
    struct ReconParamsQGGMRF2D Rparams; /* Reconstruction Parameters (includes prior parameters) */
};

float ICDStep2D(float *e, float *w, struct SysMatrix2D *A, struct ICDInfo *icd_info);

/* Prior-specific, independent of neighborhood */
float QGGMRF_SurrogateCoeff(float delta, struct ICDInfo *icd_info);
float QGGMRF_Potential(float delta, struct ReconParamsQGGMRF2D *Rparams);
/* Prior and neighborhood specific */
void  QGGMRF2D_UpdateICDParams(struct ICDInfo *icd_info);
/* Only neighborhood specific */
void ExtractNeighbors2D(struct ICDInfo *icd_info, struct Image2D *X);

/* Update error term e=y-Ax after an ICD update on x */
void UpdateError2D(float *e, struct SysMatrix2D *A, float diff, struct ICDInfo *icd_info);

#endif
