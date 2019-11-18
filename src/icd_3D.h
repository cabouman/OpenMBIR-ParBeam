#ifndef _ICD_3D_H_
#define _ICD_3D_H_

#include "MBIRModularDefs.h"

struct ICDInfo
{
    int VoxelIndex ; /* Index of Voxel being updated */
    float v; /* current pixel value */
    float neighbors[10]; /* Currently 10-point neighborhood system */
    float proxv;  /* proximal map pixel value, if P&P */
    
	float theta1; /* Quadratic surrogate function parameters -theta1 and theta2 */
	float theta2;
    
    struct ReconParams Rparams; /* Reconstruction Parameters (includes prior parameters) */
    
    int Nxy;    /* Number of pixels within a given slice */
};

float ICDStep3D(float **e, float **w, struct SysMatrix2D *A, struct ICDInfo *icd_info);

/* Prior-specific, independent of neighborhood */
float QGGMRF_SurrogateCoeff(float delta, struct ICDInfo *icd_info);
float QGGMRF_Potential(float delta, struct ReconParams *Rparams);
/* Prior and neighborhood specific */
float QGGMRF3D_Update(struct ICDInfo *icd_info);
float PandP_Update(struct ICDInfo *icd_info);
/* Only neighborhood specific */
void ExtractNeighbors3D(struct ICDInfo *icd_info, struct Image3D *X);

/* Update error term e=y-Ax after an ICD update on x */
void UpdateError3D(float **e, struct SysMatrix2D *A, float diff, struct ICDInfo *icd_info);

#endif
