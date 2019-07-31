#ifndef _RECON_2D_H_
#define _RECON_2D_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "MBIRModularUtils_2D.h"
#include "allocate.h"
#include "icd_2D.h"

void MBIRReconstruct2D(struct Image2D *Image, struct Sino2DParallel *sinogram, struct ReconParamsQGGMRF2D reconparams, struct SysMatrix2D *A, char *ImageReconMask);

float MAPCostFunction2D(float *e, struct Image2D *Image, struct Sino2DParallel *sinogram, struct ReconParamsQGGMRF2D *reconparams);

void forwardProject2D(float *AX, struct Image2D *X, struct SysMatrix2D *A); /* Compute A-matrix times X */

void shuffle(int *order, int len);

#endif
