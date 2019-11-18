#ifndef _RECON_3D_H_
#define _RECON_3D_H_

#include "MBIRModularDefs.h"

void MBIRReconstruct3D(struct Image3D *Image, struct Sino3DParallel *sinogram, struct ReconParams reconparams, struct SysMatrix2D *A, char *ImageReconMask);

float MAPCostFunction3D(float **e, struct Image3D *Image, struct Sino3DParallel *sinogram, struct ReconParams *reconparams);

void forwardProject3D(float **AX, struct Image3D *X, struct SysMatrix2D *A); /* Compute A-matrix times X */

void shuffle(int *order, int len);

#endif
