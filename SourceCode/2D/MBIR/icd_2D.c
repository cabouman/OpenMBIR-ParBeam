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


#include "icd_2D.h"


float ICDStep2D(
	float *e,  /* e=y-AX */
	float *w,
    struct SysMatrix2D *A,
	struct ICDInfo *icd_info)
{
	int i, n;
    struct SparseColumn A_column;
	float UpdatedPixelValue;

    A_column = A->column[icd_info->PixelIndex];
    
    /* Formulate the quadratic surrogate function (with coefficients theta1, theta2) for the local cost function */
	icd_info->theta1 = 0.0;
	icd_info->theta2 = 0.0;
    
	for (n = 0; n < A_column.Nnonzero; n++)
	{
		i = A_column.RowIndex[n];
        
        icd_info->theta1 -= A_column.Value[n]*w[i]*e[i];
        icd_info->theta2 += A_column.Value[n]*w[i]*A_column.Value[n];
	}
   
    /* theta1 and theta2 must be further adjusted according to Prior Model */
    /* Step can be skipped if merely ML estimation (no prior model) is followed rather than MAP estimation */
    QGGMRF2D_UpdateICDParams(icd_info);
	
    /* Calculate Updated Pixel Value */
    UpdatedPixelValue = icd_info->v - (icd_info->theta1/icd_info->theta2) ;
    
	return UpdatedPixelValue;
}

/* ICD update with the QGGMRF prior model */
/* Prior and neighborhood specific */
void QGGMRF2D_UpdateICDParams(struct ICDInfo *icd_info)
{
    int j; /* Neighbor relative position to Pixel being updated */
    float delta, SurrogateCoeff;
    float sum1_Nearest=0, sum1_Diag=0; /* for theta1 calculation */
    float sum2_Nearest=0, sum2_Diag=0; /* for theta2 calculation */
    float b_nearest, b_diag;
    
    b_nearest=icd_info->Rparams.b_nearest;
    b_diag=icd_info->Rparams.b_diag;
    
    for (j = 0; j < 8; j++)
    {
        delta = icd_info->v - icd_info->neighbors[j];
        SurrogateCoeff = QGGMRF_SurrogateCoeff(delta,icd_info);
        
        if (j < 4)
        {
            sum1_Nearest += (SurrogateCoeff * delta);
            sum2_Nearest += SurrogateCoeff;
        }
        if (j >= 4)
        {
            sum1_Diag += (SurrogateCoeff * delta);
            sum2_Diag += SurrogateCoeff;
        }
    }
    
    icd_info->theta1 +=  (b_nearest * sum1_Nearest + b_diag * sum1_Diag) ;
    icd_info->theta2 +=  (b_nearest * sum2_Nearest + b_diag * sum2_Diag) ;
}



/* the potential function of the QGGMRF prior model.  p << q <= 2 */
float QGGMRF_Potential(float delta, struct ReconParamsQGGMRF2D *Rparams)
{
    float p, q, T, SigmaX;
    float temp, GGMRF_Pot;
    
    p = Rparams->p;
    q = Rparams->q;
    T = Rparams->T;
    SigmaX = Rparams->SigmaX;
    
    GGMRF_Pot = pow(fabs(delta),p)/(p*pow(SigmaX,p));
    temp = pow(fabs(delta/(T*SigmaX)), q-p);
    
    return ( GGMRF_Pot * temp/(1.0+temp) );
}

/* Quadratic Surrogate Function for the log(prior model) */
/* For a given convex potential function rho(delta) ... */
/* The surrogate function defined about a point "delta_p", Q(delta ; delta_p), is given by ... */
/* Q(delta ; delta_p) = a(delta_p) * (delta^2/2), where the coefficient a(delta_p) is ... */
/* a(delta_p) = [ rho'(delta_p)/delta_p ]   ... */
/* for the case delta_current is Non-Zero and rho' is the 1st derivative of the potential function */
/* Return this coefficient a(delta_p) */
/* Prior-model specific, independent of neighborhood */

float QGGMRF_SurrogateCoeff(float delta, struct ICDInfo *icd_info)
{
    float p, q, T, SigmaX, qmp;
    float num, denom, temp;
    
    p = icd_info->Rparams.p;
    q = icd_info->Rparams.q;
    T = icd_info->Rparams.T;
    SigmaX = icd_info->Rparams.SigmaX;
    qmp = q - p;
    
    /* Refer to Chapter 7, MBIR Textbook by Prof Bouman, Page 151 */
    /* Table on Quadratic surrogate functions for different prior models */
    
    if (delta == 0.0)
    return 2.0/( p*pow(SigmaX,q)*pow(T,qmp) ) ; /* rho"(0) */
    
    temp = pow(fabs(delta/(T*SigmaX)), qmp);
    num = q/p + temp;
    denom = pow(SigmaX,p) * (1.0+temp) * (1.0+temp);
    num = num * pow(fabs(delta),p-2) * temp;
    
    return num/denom;
}


/* extract the neighborhood system */
void ExtractNeighbors2D(
                        struct ICDInfo *icd_info,
                        struct Image2D *Image)
{
    int jx, jy, plusx, minusx, plusy, minusy;
    int Nx, Ny;
    
    Nx = Image->imgparams.Nx;
    Ny = Image->imgparams.Ny;
    
    /* PixelIndex = jy*Nx+jx */
    jy = icd_info->PixelIndex/Nx; /* Y-index of pixel */
    jx = icd_info->PixelIndex%Nx; /* X-index of pixel */
    
    plusx = jx + 1;
    plusx = ((plusx < Nx) ? plusx : 0);
    minusx = jx - 1;
    minusx = ((minusx < 0) ? (Nx-1) : minusx);
    plusy = jy + 1;
    plusy = ((plusy < Ny) ? plusy : 0);
    minusy = jy - 1;
    minusy = ((minusy < 0) ? (Ny-1) : minusy);
    
    icd_info->neighbors[0] = Image->image[jy*Nx+plusx];
    icd_info->neighbors[1] = Image->image[jy*Nx+minusx];
    icd_info->neighbors[2] = Image->image[plusy*Nx+jx];
    icd_info->neighbors[3] = Image->image[minusy*Nx+jx];
    
    icd_info->neighbors[4] = Image->image[plusy*Nx+plusx];
    icd_info->neighbors[5] = Image->image[plusy*Nx+minusx];
    icd_info->neighbors[6] = Image->image[minusy*Nx+plusx];
    icd_info->neighbors[7] = Image->image[minusy*Nx+minusx];
}

/* Update error term e=y-Ax after an ICD update on x */
void UpdateError2D(
                   float *e,
                   struct SysMatrix2D *A,
                   float diff,
                   struct ICDInfo *icd_info)
{
    int n, j, i;
    
    j = icd_info->PixelIndex; /* index of updated pixel */
    
    for (n = 0; n < A->column[j].Nnonzero; n++)
    {
        i = A->column[j].RowIndex[n];
        e[i] -= A->column[j].Value[n]*diff;
    }
}



