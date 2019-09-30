
#include "A_comp_3D.h"

/******************************************************************/
/* Compute line segment lengths through a pixel for the given set */
/* of sinogram angles and for multiple displacements (LEN_PIX)    */
/******************************************************************/
/* Fill profiles of pixels from all angles
   ONLY FOR SQUARE PIXELS NOW   
   Each profile assumed 2 pixels wide
   This is used to speed calculation (too many to store) of the
   entries of the projection matrix.
   Values are scaled by "scale"
 */


/* Compute Pixel-detector profile */
/* Refer to slides "Parallel_beam_CT_FwdModel_v2.pptx" in documentation folder */
/* The System matrix does not vary with slice for 3-D Parallel Geometry */
/* So, the method of compuatation is same as that of 2-D Parallel Geometry */

float **ComputePixelProfile3DParallel(
        struct SinoParams3DParallel *sinoparams,
        struct ImageParams3D *imgparams)
{
	int i, j;
	float pi, ang, d1, d2, t, t_1, t_2, t_3, t_4, maxval, rc, DeltaPix;
    float **pix_prof ; /* Detector-pixel profile, indexed by view angle and detector-pixel displacement */

	DeltaPix = imgparams->Deltaxy;

	pix_prof = (float **)get_img(LEN_PIX, sinoparams->NViews, sizeof(float));

	pi = PI; /* defined in MBIRModularUtils_2D.h */
	rc = sin(pi/4.0); /* Constant sin(pi/4) */
    
    /* For pixel-detector profile parameters .. */
    /* Refer to slides "Parallel_beam_CT_FwdModel_v2.pptx" in documentation folder */
    /* Compute 3 parameters of the profile function */
    /* Here the corresponding parameters are : maxval, d1 and d2 */
    
    
	for (i = 0; i < sinoparams->NViews; i++)
	{
		ang = sinoparams->ViewAngles[i];

		while (ang >= pi/2.0)
		{
			ang -= pi/2.0;
		}
		while(ang < 0.0)
		{
			ang += pi/2.0;
		}

		if (ang <= pi/4.0)
		{
			maxval = DeltaPix/cos(ang);
		}
		else
		{
			maxval = DeltaPix/cos(pi/2.0-ang);
		}

		d1 = rc*cos(pi/4.0-ang);
		d2 = rc*fabs(sin(pi/4.0-ang));

		t_1 = 1.0 - d1;
		t_2 = 1.0 - d2;
		t_3 = 1.0 + d2;
		t_4 = 1.0 + d1;
        
        /* Profile is a trapezoidal function of detector-pixel displacement*/
		for (j = 0; j < LEN_PIX; j++)
		{
			t = 2.0*j/(float)LEN_PIX;
			if (t <= t_1 || t > t_4)
			{
				pix_prof[i][j] = 0.0;
			}
			else if (t <= t_2)
			{
				pix_prof[i][j] = maxval*(t-t_1)/(t_2-t_1);
			}
			else if (t <= t_3)
			{
				pix_prof[i][j] = maxval;
			}
			else
			{
				pix_prof[i][j] = maxval*(t_4-t)/(t_4-t_3);
			}
		}
	}
    
    return pix_prof;
}

/* Compute the System Matrix column for a given pixel */
/* Refer to slides "Parallel_beam_CT_FwdModel_v2.pptx" in documentation folder */
/* The System matrix does not vary with slice for 3-D Parallel Geometry */
/* So, the method of compuatation is same as that of 2-D Parallel Geometry */

void ComputeSysMatrixColumn3DParallel(
	int ColumnIndex,
	struct SinoParams3DParallel *sinoparams,
	struct ImageParams3D *imgparams,
	float **pix_prof,
	struct SparseColumn *A_Column)
{
    int im_row, im_col ;
	int ind, ind_min, ind_max, pr;
	int pix_prof_ind, pind, i, k, proj_count;
	float Aval, t_min, t_max, ang, x, y;
	float t, const1, const2, const3, const4;
	static int Ntheta, NChannels, Nx, Ny;
	static float DeltaPix, DeltaChannel, t_0, x_0, y_0;

#ifdef WIDE_BEAM
    int prof_ind;
#endif
    
	static float dprof[LEN_DET];
	static int first = 1;
    
	t = 0;
	prof_ind = 0;
	pix_prof_ind = 0;

	if (first == 1)
	{
		first = 0;

		Ntheta = sinoparams->NViews;
		NChannels = sinoparams->NChannels;
		DeltaChannel = sinoparams->DeltaChannel; /* detector channel spacing */
        /* t_0 (mm) - Position of "Detector Channel 0" */
        /* ith channel position (mm) = t_0 + i*DeltaChannel */
		t_0 = -(sinoparams->NChannels-1)*sinoparams->DeltaChannel/2.0 - sinoparams->CenterOffset * sinoparams->DeltaChannel;

		Nx = imgparams->Nx;
		Ny = imgparams->Ny;
		DeltaPix = imgparams->Deltaxy; /* spacing between pixels (equal in x and y dimensions) */
		x_0 = -(Nx-1)*imgparams->Deltaxy/2.0;
		y_0 = -(Ny-1)*imgparams->Deltaxy/2.0;

		/* Uniform Detector Sensitivity. Weights must sum to one */
		for (k = 0; k < LEN_DET; k++)
		{
			dprof[k] = 1.0/(LEN_DET);
		}
	} 

	/* WATCH THIS : ONLY FOR SQUARE PIXELS NOW   */
    im_row = ColumnIndex/Nx;
    im_col = ColumnIndex%Nx;
	y = y_0 + im_row*DeltaPix;
	x = x_0 + im_col*DeltaPix;

	proj_count = 0;
	for (pr = 0; pr < Ntheta; pr++)
	{
		pind = pr*NChannels;
		ang = sinoparams->ViewAngles[pr];

		/* Range of interest for pixel profile.  Here, this is 2 pixel widths from the (projected) center of the pixel */
		t_min = y*cos(ang) - x*sin(ang) - DeltaPix;
		t_max = t_min + 2.0*DeltaPix;
		/* This also prevents over-reach (with rounding of negative numbers)  */
		if (t_max < t_0) 
		{
			continue;
		}

		/* Relevant detector indices */
		ind_min = ceil((t_min-t_0)/DeltaChannel - 0.5);
		ind_max = (t_max-t_0)/DeltaChannel + 0.5;

		/* To prevent over-reach at ends  */
		ind_min = (ind_min<0) ? 0 : ind_min;
		ind_max = (ind_max>=NChannels) ? NChannels-1 : ind_max;

		const1 = t_0 - DeltaChannel/2.0; /* extreme border of 1st detector */
		const2 = DeltaChannel/(float)(LEN_DET-1); /* detector-element width */
		const3 = DeltaPix - (y*cos(ang) - x*sin(ang)); /* (y*cos(ang) - x*sin(ang)) = pixel coordinate projected onto detector axis */
		const4 = (float)(LEN_PIX-1)/(2.0*DeltaPix); /* spatial resolution of detector-pixel profile function */
        const1 = const1+const2; /* position of 1st detector element, 1st detector */

		for (i = ind_min; i <= ind_max; i++)
		{
			ind = pind + i;

#ifdef WIDE_BEAM
            /* Split the aperture of a given detector into smaller elements, because sensitivity may vary across detector aperture */
            /* Final forward projection is a weighted sum of the projections measured by smaller elements */
			Aval = 0;
			for (k = 0; k < LEN_DET; k++)
			{
				t = const1 + (float)i*DeltaChannel + (float)k*const2; /* Detector element position */
				pix_prof_ind = (t+const3)*const4 +0.5;   /* +0.5 for rounding */
				if (pix_prof_ind >= 0 && pix_prof_ind < LEN_PIX)
				{
					Aval+= dprof[k]*pix_prof[pr][pix_prof_ind];
				}
			}
#else
			/*** Only use center of detector aperture while computing detector-pixel profile ****/
			prof_ind = LEN_PIX*(t_0+i*DeltaChannel+const3)/(2.0*DeltaPix);

			if (prof_ind >= LEN_PIX || prof_ind < 0)
			{
				if (prof_ind == LEN_PIX)
				{
					prof_ind = LEN_PIX-1;
				}
				else if (prof_ind == -1)
				{
					prof_ind = 0;
				}
				else
				{
					fprintf(stderr,"\nExiting Program: input parameters inconsistant\n");
					exit(-1);
				}
			}
			Aval = pix_prof[pr][prof_ind];
#endif

			if (Aval > 0.0)
			{
				A_Column->Value[proj_count] = Aval;
				A_Column->RowIndex[proj_count] = ind;   /* Index for ray */
				proj_count++;
			}
		}
	} 

	A_Column->Nnonzero = proj_count;
}


/* Compute Entire System Matrix */
/* The System matrix does not vary with slice for 3-D Parallel Geometry */
/* So, the method of compuatation is same as that of 2-D Parallel Geometry */

struct SysMatrix2D *ComputeSysMatrix3DParallel(
       struct SinoParams3DParallel *sinoparams,
       struct ImageParams3D *imgparams,
       float **pix_prof)
{
    struct SysMatrix2D *A ; /* Forward Matrix in sparse format */
    struct SparseColumn TempColumn;
	int i,r;
	int MaxNnonzero;
    
    A = (struct SysMatrix2D *)malloc(sizeof(struct SysMatrix2D));
    A->Ncolumns = imgparams->Nx * imgparams->Ny ;
    A->column = (struct SparseColumn *)get_spc(A->Ncolumns, sizeof(struct SparseColumn));

		fprintf(stdout, "\nComputing System Matrix ...\n");
		fflush(stdout);

		MaxNnonzero = sinoparams->NChannels*sinoparams->NViews; /* Maximum no. of non-zero entries in the A matrix column */

		TempColumn.RowIndex = (int *)get_spc(MaxNnonzero, sizeof(int));
        TempColumn.Value = (float *)get_spc(MaxNnonzero, sizeof(float));
    
        printf("\n");
		for (i = 0; i < A->Ncolumns; i++)
		{
            if(i%100==0)
            {
                printf("\r\tProgress = %2.1f %%", (float)i/A->Ncolumns*100.0); fflush(stdout);
            }
            ComputeSysMatrixColumn3DParallel(i, sinoparams, imgparams, pix_prof, &TempColumn);

                A->column[i].Nnonzero = TempColumn.Nnonzero;
				A->column[i].Value = (float *)get_spc(TempColumn.Nnonzero,sizeof(float));
				A->column[i].RowIndex = (int *)get_spc(TempColumn.Nnonzero,sizeof(int));
                /* Copy non-zero entries from TempColumn to A->column[i] */
				for (r = 0; r < TempColumn.Nnonzero; r++)
				{
					A->column[i].Value[r] = (float)TempColumn.Value[r];
					A->column[i].RowIndex[r] = TempColumn.RowIndex[r];
				}
		}
        printf("\n");
		free((void *)TempColumn.Value);
		free((void *)TempColumn.RowIndex);

		fprintf(stdout, "System Matrix Computation done \n");
		fflush(stdout);
    
    return A;
}


