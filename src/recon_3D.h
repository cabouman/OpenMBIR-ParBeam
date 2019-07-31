#ifndef _RECON_3D_H_
#define _RECON_3D_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "MBIRModularUtils_3D.h"
#include "MBIRModularUtils_2D.h"
#include "allocate.h"
#include "icd_3D.h"

void MBIRReconstruct3D(struct Image3D *Image, struct Sino3DParallel *sinogram, struct ReconParamsQGGMRF3D reconparams, struct SysMatrix2D *A, char *ImageReconMask);

float MAPCostFunction3D(float **e, struct Image3D *Image, struct Sino3DParallel *sinogram, struct ReconParamsQGGMRF3D *reconparams);

void forwardProject3D(float **AX, struct Image3D *X, struct SysMatrix2D *A); /* Compute A-matrix times X */

void shuffle(int *order, int len);

#endif
