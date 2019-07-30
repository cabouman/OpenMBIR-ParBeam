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

#ifndef _ICD_2D_H_
#define _ICD_2D_H_

#include <math.h>

#include "../../MBIRModularUtils_2D.h"
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
