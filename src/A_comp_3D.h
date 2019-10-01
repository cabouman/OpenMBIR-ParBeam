#ifndef _ACOMP_H_
#define _ACOMP_H_

#include "MBIRModularDefs.h"

/* Computation options */
#define WIDE_BEAM   /* Finite element analysis of detector channel, accounts for sensitivity variation across its aperture */
#define LEN_PIX 511 /* determines the spatial resolution for Detector-Pixel computation. Higher LEN_PIX, higher resolution */
                    /* In this implementation, spatial resolution is : [2*PixelDimension/LEN_PIX]^(-1) */
#define LEN_DET 101 /* No. of Detector Elements */
                    /* Each detector channel is "split" into LEN_DET smaller elements ... */
                    /* to account for detector sensitivity variation across its aperture */

/* The System matrix does not vary with slice for 3-D Parallel Geometry */
/* So, the method of compuatation is same as that of 2-D Parallel Geometry */

/* Compute Pixel-Detector Profile for 3D Parallel Beam Geometry */
float **ComputePixelProfile3DParallel(struct SinoParams3DParallel *sinoparams, struct ImageParams3D *imgparams);
/* Compute System Matrix for 3D Parallel Beam Geometry*/
struct SysMatrix2D *ComputeSysMatrix3DParallel(struct SinoParams3DParallel *sinoparams, struct ImageParams3D *imgparams, float **pix_prof);

#endif
