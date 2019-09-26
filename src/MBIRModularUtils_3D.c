#include "MBIRModularUtils_3D.h"
#include "MBIRModularUtils_2D.h"
#include <string.h>
#include <libgen.h>


/**********************************************/
/*  Utilities for reading/writing 3D sinogram */
/**********************************************/


void printSinoParams3DParallel(struct SinoParams3DParallel *sinoparams)
{
    fprintf(stdout, "SINOGRAM PARAMETERS:\n");
    fprintf(stdout, " - Number of sinogram views per slice    = %d\n", sinoparams->NViews);
    fprintf(stdout, " - Number of detector channels per slice = %d\n", sinoparams->NChannels);
    fprintf(stdout, " - Number of slices                      = %d\n", sinoparams->NSlices);
    fprintf(stdout, " - Spacing between Detector Channels     = %.7f (mm) \n", sinoparams->DeltaChannel);
    fprintf(stdout, " - Center of rotation offset             = %.7f (channels)\n", sinoparams->CenterOffset);
    fprintf(stdout, " - Spacing between slices                = %.7f (mm)\n", sinoparams->DeltaSlice);
    fprintf(stdout, " - First Slice Index                     = %d \n", sinoparams->FirstSliceNumber);
}


/* Utility for reading 3D parallel beam sinogram parameters */
/* Returns 0 if no error occurs */
int ReadSinoParams3DParallel(
	char *basename,					/* Input: Reads sinogram parameters from <fname>.sinoparams */
	struct SinoParams3DParallel *sinoparams)	/* Output: Reads sinogram parameters into data structure */
{
	FILE *fp;
	char fname[200];
	char tag[200], fieldname[200], fieldval_s[200], *ptr;
	char AngleListFname[200]=" ";
	int i, Nlines;
	char Geometry_flag=0;

	strcpy(fname,basename);
	strcat(fname,".sinoparams"); /* append file extension */
	if((fp=fopen(fname,"r")) == NULL) {
		fprintf(stderr,"ERROR in ReadSinoParams3DParallel: can't open file %s\n",fname);
		exit(-1);
	}

	Nlines=0;
	while(fgets(tag, 200, fp)!=NULL && Nlines<100)
		Nlines++;
	rewind(fp);
	//printf("Nlines=%d\n",Nlines);

	/* set defaults, also used for error checking below */
	sinoparams->NViews=0;		/* Number of view angles */
	sinoparams->NChannels=-1;	/* Number of channels in detector */
	sinoparams->DeltaChannel=0.0;	/* Detector spacing (mm) */
	sinoparams->NSlices=0;		/* Number of slices stored in Sino array */
	sinoparams->FirstSliceNumber=-1;	/* slice index coresponding to first slice in volume */
	sinoparams->DeltaSlice=0.0;	/* Spacing along slice direction (mm) */
	sinoparams->CenterOffset=0.0;	/* Offset of center-of-rotation ... */

	/* parse each line assuming a single ":" delimiter */
	for(i=0; i<Nlines; i++)
	{
		strcpy(fieldname," ");
		strcpy(fieldval_s," ");
		fgets(tag, 200, fp);
		ptr=strtok(tag,":\n\r");	// including the newline will keep it out of the last token
		if(ptr!=NULL) {
			//strcpy(fieldname,ptr);
			sscanf(ptr,"%s",fieldname);	// this won't include leading/trailing spaces, strcpy will
			ptr=strtok(NULL,":\n\r");
		}
		if(ptr!=NULL) sscanf(ptr,"%s",fieldval_s);
		//printf("|%s|%s|\n",fieldname,fieldval_s);

		if(strcmp(fieldname,"Geometry")==0)
		{
			Geometry_flag=1;
			if(strcmp(fieldval_s,"3DPARALLEL")!=0) {
				fprintf(stderr,"Error in %s: Geometry value \"%s\" unrecognized\n",fname,fieldval_s);
				exit(-1);
			}
		}
		else if(strcmp(fieldname,"NChannels")==0)
		{
			sscanf(fieldval_s,"%d",&(sinoparams->NChannels));
		}
		else if(strcmp(fieldname,"NViews")==0)
		{
			sscanf(fieldval_s,"%d",&(sinoparams->NViews));
		}
		else if(strcmp(fieldname,"NSlices")==0)
		{
			sscanf(fieldval_s,"%d",&(sinoparams->NSlices));
		}
		else if(strcmp(fieldname,"DeltaChannel")==0)
		{
			sscanf(fieldval_s,"%f",&(sinoparams->DeltaChannel));
		}
		else if(strcmp(fieldname,"CenterOffset")==0)
		{
			sscanf(fieldval_s,"%f",&(sinoparams->CenterOffset));
		}
		else if(strcmp(fieldname,"DeltaSlice")==0)
		{
			sscanf(fieldval_s,"%f",&(sinoparams->DeltaSlice));
		}
		else if(strcmp(fieldname,"FirstSliceNumber")==0)
		{
			sscanf(fieldval_s,"%d",&(sinoparams->FirstSliceNumber));
		}
		else if(strcmp(fieldname,"ViewAngleList")==0)
			sscanf(fieldval_s,"%s",AngleListFname);  // this won't include leading/trailing spaces, strcpy will
			//strcpy(AngleListFname,fieldval_s);
		else
			fprintf(stderr,"Warning: unrecognized field \"%s\" in %s, line %d\n",fieldname,fname,i+1);

	}  // done parsing

	fclose(fp);

	//printSinoParams3DParallel(sinoparams);

	/* do some error checking */
	if(Geometry_flag==0) {
		fprintf(stderr,"Error in %s: \"Geometry\" field unspecified\n",fname);
		exit(-1);
	}
	if(sinoparams->NViews<=0 || sinoparams->NChannels<=0 || sinoparams->NSlices<=0) {
		printSinoParams3DParallel(sinoparams);
		fprintf(stderr,"Error in %s: NViews, NChannels, NSlices must all be positive\n",fname);
		exit(-1);
	}
	if(sinoparams->DeltaChannel<=0 && sinoparams->NChannels>1) {
		printSinoParams3DParallel(sinoparams);
		fprintf(stderr,"Error in %s: DeltaChannel needs to be positive (mm)\n",fname);
		exit(-1);
	}
	if(sinoparams->DeltaSlice<=0 && sinoparams->NSlices>1) {
		printSinoParams3DParallel(sinoparams);
		fprintf(stderr,"Error in %s: DeltaSlice needs to be positive (mm)\n",fname);
		exit(-1);
	}
	if(sinoparams->FirstSliceNumber < 0) {
		printSinoParams3DParallel(sinoparams);
		fprintf(stderr,"Error in %s: FirstSliceNumber should be non-negative\n",fname);
		exit(-1);
	}

	/* form full pathname of ViewAngleList file; path relative to sinoparams directory */
	strcpy(fieldval_s,AngleListFname);  // tmp copy
	strcpy(tag,fname);
	ptr=dirname(tag);
	sprintf(AngleListFname,"%s/%s",ptr,fieldval_s);
	//printf("Views filename \"%s\"\n",AngleListFname);

	/* Read the view angle */
	if((fp=fopen(AngleListFname,"r")) == NULL) {
		fprintf(stderr,"ERROR in ReadSinoParams3DParallel: can't open ViewAngle file %s\n",AngleListFname);
		exit(-1);
	}

	sinoparams->ViewAngles = (float *)get_spc(sinoparams->NViews, sizeof(float));

	for(i=0;i<sinoparams->NViews;i++) {
		if(fscanf(fp,"%f\n",&(sinoparams->ViewAngles[i])) == 0) {
			fprintf(stderr, "ERROR in ReadSinoParams3DParallel: List of view angles in file %s terminated early.\n", AngleListFname);
			exit(-1);
		}
	}

	fclose(fp);

	return(0);
}


/* Utility for reading 2D Image parameters */
/* Returns 0 if no error occurs */
int ReadImageParams3D(
	char *basename,				/* Input: Reads image type parameter from <fname>.imgparams */
	struct ImageParams3D *imgparams)	/* Output: Reads image parameters into data structure */
{
	FILE *fp;
	char fname[200];
	char tag[200], fieldname[200], fieldval_s[200], *ptr;
	int i, Nlines;

	strcpy(fname,basename);
	strcat(fname,".imgparams");
	if((fp=fopen(fname,"r")) == NULL) {
		fprintf(stderr,"ERROR in ReadImageParams3D: can't open file %s\n",fname);
		exit(-1);
	}

	Nlines=0;
	while(fgets(tag, 200, fp)!=NULL && Nlines<100)
		Nlines++;
	rewind(fp);
	//printf("Nlines=%d\n",Nlines);

	/* set defaults, also used for error checking below */
	imgparams->Nx=0;
	imgparams->Ny=0;
	imgparams->Nz=0;
	imgparams->FirstSliceNumber=-1;
	imgparams->Deltaxy=0.0;
	imgparams->DeltaZ=0;
	imgparams->ROIRadius=0.0;

	/* parse each line assuming a single ":" delimiter */
	for(i=0; i<Nlines; i++)
	{
		strcpy(fieldname," ");
		strcpy(fieldval_s," ");
		fgets(tag, 200, fp);
		ptr=strtok(tag,":\n\r");	// including the newline will keep it out of the last token
		if(ptr!=NULL) {
			//strcpy(fieldname,ptr);
			sscanf(ptr,"%s",fieldname);	// this won't include leading/trailing spaces, strcpy will
			ptr=strtok(NULL,":\n\r");
		}
		if(ptr!=NULL) sscanf(ptr,"%s",fieldval_s);
		//printf("|%s|%s|\n",fieldname,fieldval_s);

		if(strcmp(fieldname,"Nx")==0)
		{
			sscanf(fieldval_s,"%d",&(imgparams->Nx));
		}
		else if(strcmp(fieldname,"Ny")==0)
		{
			sscanf(fieldval_s,"%d",&(imgparams->Ny));
		}
		else if(strcmp(fieldname,"Nz")==0)
		{
			sscanf(fieldval_s,"%d",&(imgparams->Nz));
		}
		else if(strcmp(fieldname,"FirstSliceNumber")==0)
		{
			sscanf(fieldval_s,"%d",&(imgparams->FirstSliceNumber));
		}
		else if(strcmp(fieldname,"Deltaxy")==0)
		{
			sscanf(fieldval_s,"%f",&(imgparams->Deltaxy));
		}
		else if(strcmp(fieldname,"DeltaZ")==0)
		{
			sscanf(fieldval_s,"%f",&(imgparams->DeltaZ));
		}
		else if(strcmp(fieldname,"ROIRadius")==0)
		{
			sscanf(fieldval_s,"%f",&(imgparams->ROIRadius));
		}
		else
			fprintf(stderr,"Warning: unrecognized field \"%s\" in %s, line %d\n",fieldname,fname,i+1);

	}  // done parsing

	fclose(fp);

	//printImageParams3D(imgparams);

	/* do some error checking */
	if(imgparams->Nx<=0 || imgparams->Ny<=0 || imgparams->Nz<=0) {
		printImageParams3D(imgparams);
		fprintf(stderr,"Error in %s: Nx, Ny, Nz must all be positive\n",fname);
		exit(-1);
	}
	if(imgparams->Deltaxy<=0) {
		printImageParams3D(imgparams);
		fprintf(stderr,"Error in %s: Deltaxy needs to be positive (mm)\n",fname);
		exit(-1);
	}
	if(imgparams->DeltaZ<=0 && imgparams->Nz>1) {
		printImageParams3D(imgparams);
		fprintf(stderr,"Error in %s: DeltaZ needs to be positive (mm)\n",fname);
		exit(-1);
	}
	if(imgparams->FirstSliceNumber < 0) {
		printImageParams3D(imgparams);
		fprintf(stderr,"Error in %s: FirstSliceNumber should be non-negative\n",fname);
		exit(-1);
	}
	if(imgparams->ROIRadius<=0) {
		imgparams->ROIRadius = imgparams->Nx * imgparams->Deltaxy;
		fprintf(stderr,"Warning in %s: ROIRadius needs to be positive. Defaulting to %.4f (mm)\n",fname,imgparams->ROIRadius);
		printImageParams3D(imgparams);
	}

	return(0);
}



int ReadReconParamsQGGMRF3D(
	char *basename,				/* base file name <fname>.reconparams */
	struct ReconParamsQGGMRF3D *reconparams) /* recon parameters data structure */
{
	FILE *fp;
	char fname[200];
	char tag[200], fieldname[200], fieldval_s[200], *ptr;
	double fieldval_f;
	int fieldval_d;
	int i, Nlines;
	char Prior_flag=0;

	strcpy(fname,basename);
	strcat(fname,".reconparams");
	if((fp=fopen(fname,"r")) == NULL) {
		fprintf(stderr,"ERROR in ReadReconParamsQGGMRF3D: can't open file %s\n",fname);
		exit(-1);
	}

	Nlines=0;
	while(fgets(tag, 200, fp)!=NULL && Nlines<100)
		Nlines++;
	rewind(fp);
	//printf("Nlines=%d\n",Nlines);

	/* set defaults, also used for error checking below */
	reconparams->InitImageValue=MUWATER;
	reconparams->p=1.2;
	reconparams->q=2.0;
	reconparams->T=0.1;
	reconparams->SigmaX=0.02;
	reconparams->SigmaY=1.0;
	reconparams->b_nearest=1.0;
	reconparams->b_diag=0.707;
	reconparams->b_interslice=1.0;
	reconparams->StopThreshold=1.0;
	reconparams->MaxIterations=20;
	reconparams->Positivity=1;

	/* parse each line assuming a single ":" delimiter */
	for(i=0; i<Nlines; i++)
	{
		strcpy(fieldname," ");
		strcpy(fieldval_s," ");
		fgets(tag, 200, fp);
		ptr=strtok(tag,":\n\r");	// including the newline will keep it out of the last token
		if(ptr!=NULL) {
			//strcpy(fieldname,ptr);
			sscanf(ptr,"%s",fieldname);	// this won't include leading/trailing spaces, strcpy will
			ptr=strtok(NULL,":\n\r");
		}
		if(ptr!=NULL) sscanf(ptr,"%s",fieldval_s);
		printf("|%s|%s|\n",fieldname,fieldval_s);

		if(strcmp(fieldname,"PriorModel")==0)
		{
			Prior_flag=1;
			if(strcmp(fieldval_s,"QGGMRF")!=0) {
				fprintf(stderr,"Error in %s: PriorModel value \"%s\" unrecognized\n",fname,fieldval_s);
				exit(-1);
			}
		}
		else if(strcmp(fieldname,"InitImageValue")==0)
		{
			sscanf(fieldval_s,"%lf",&(fieldval_f));
			if(fieldval_f <= 0)
				fprintf(stderr,"Warning in %s: InitImageValue should be positive. Reverting to default.\n",fname);
			else
				reconparams->InitImageValue = fieldval_f;
		}
		else if(strcmp(fieldname,"p")==0)
		{
			sscanf(fieldval_s,"%lf",&(fieldval_f));
			if(fieldval_f < 1 || fieldval_f > 2)
				fprintf(stderr,"Warning in %s: p parameter should be in range [1,2]. Reverting to default.\n",fname);
			else
				reconparams->p = fieldval_f;
		}
		else if(strcmp(fieldname,"q")==0)
		{
			sscanf(fieldval_s,"%lf",&(fieldval_f));
			if(fieldval_f < 1 || fieldval_f > 2)
				fprintf(stderr,"Warning in %s: q parameter should be in range [1,2]. Reverting to default.\n",fname);
			else
				reconparams->q = fieldval_f;
		}
		else if(strcmp(fieldname,"T")==0)
		{
			sscanf(fieldval_s,"%lf",&(fieldval_f));
			if(fieldval_f <= 0)
				fprintf(stderr,"Warning in %s: T parameter should be positive. Reverting to default.\n",fname);
			else
				reconparams->T = fieldval_f;
		}
		else if(strcmp(fieldname,"SigmaX")==0)
		{
			//sscanf(fieldval_s,"%lf",&(reconparams->SigmaX));
			sscanf(fieldval_s,"%lf",&(fieldval_f));
			if(fieldval_f <= 0)
				fprintf(stderr,"Warning in %s: SigmaX parameter should be positive. Reverting to default.\n",fname);
			else
				reconparams->SigmaX = fieldval_f;
		}
		else if(strcmp(fieldname,"SigmaY")==0)
		{
			sscanf(fieldval_s,"%lf",&(fieldval_f));
			if(fieldval_f <= 0)
				fprintf(stderr,"Warning in %s: SigmaY parameter should be positive. Reverting to default.\n",fname);
			else
				reconparams->SigmaY = fieldval_f;
		}
		else if(strcmp(fieldname,"b_nearest")==0)
		{
			sscanf(fieldval_s,"%lf",&(fieldval_f));
			if(fieldval_f <= 0)
				fprintf(stderr,"Warning in %s: b_nearest parameter should be positive. Reverting to default.\n",fname);
			else
				reconparams->b_nearest = fieldval_f;
		}
		else if(strcmp(fieldname,"b_diag")==0)
		{
			sscanf(fieldval_s,"%lf",&(fieldval_f));
			if(fieldval_f < 0)
				fprintf(stderr,"Warning in %s: b_diag parameter should be non-negative. Reverting to default.\n",fname);
			else
				reconparams->b_diag = fieldval_f;
		}
		else if(strcmp(fieldname,"b_interslice")==0)
		{
			sscanf(fieldval_s,"%lf",&(fieldval_f));
			if(fieldval_f < 0)
				fprintf(stderr,"Warning in %s: b_interslice parameter should be non-negative. Reverting to default.\n",fname);
			else
				reconparams->b_interslice = fieldval_f;
		}
		else if(strcmp(fieldname,"StopThreshold")==0)
		{
			sscanf(fieldval_s,"%lf",&(fieldval_f));
			if(fieldval_f < 0)
				fprintf(stderr,"Warning in %s: StopThreshold should be non-negative. Reverting to default.\n",fname);
			else
				reconparams->StopThreshold = fieldval_f;
		}
		else if(strcmp(fieldname,"MaxIterations")==0)
		{
			//sscanf(fieldval_s,"%d",&(reconparams->MaxIterations));
			sscanf(fieldval_s,"%d",&(fieldval_d));
			if(fieldval_d < 1)
				fprintf(stderr,"Warning in %s: MaxIterations should be at least 1. Reverting to default.\n",fname);
			else
				reconparams->MaxIterations = fieldval_d;
		}
		else if(strcmp(fieldname,"Positivity")==0)
		{
			sscanf(fieldval_s,"%d",&(fieldval_d));
			if( strcmp(fieldval_s,"0") && strcmp(fieldval_s,"1") )
				fprintf(stderr,"Warning in %s: \"Positivity\" parameter options are 0/1. Reverting to default.\n",fname);
			else
				reconparams->Positivity = fieldval_d;
		}
		else
			fprintf(stderr,"Warning: unrecognized field \"%s\" in %s, line %d\n",fieldname,fname,i+1);

	}  // done parsing

	fclose(fp);

	//printReconParamsQGGMRF3D(reconparams);

	/* do some error checking */
	if(Prior_flag==0) {
		fprintf(stderr,"Error in %s: \"PriorModel\" field unspecified\n",fname);
		exit(-1);
	}
	if(reconparams->p > reconparams->q) {
		printReconParamsQGGMRF3D(reconparams);
		fprintf(stderr,"Error in %s: Need (p <= q) for convexity. (p<q for strict convexity)\n",fname);
		exit(-1);
	}

	return(0);
}






/* Utility for reading 3D parallel beam sinogram parameters */
/* Returns 0 if no error occurs */
int ReadSinoParams3DParallel_old(
                             char *fname,                               /* Input: Reads sinogram parameters from <fname>.sinoparams */
                             struct SinoParams3DParallel *sinoparams)  /* Output: Reads sinogram parameters into data structure */
{
    FILE *fp;
    char tag[200];
    char AngleListFileName[200];
    char AngleListFileName_relative[200];
    int i;
    
    strcat(fname,".sinoparams"); /* append file extension */
    
    if ((fp = fopen(fname, "r")) == NULL)
    {
        fprintf(stderr, "ERROR in ReadSinoParams3DParallel: can't open file %s.\n", fname);
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%d\n", &(sinoparams->NChannels));
    if(sinoparams->NChannels <= 0){
        fprintf(stderr,"ERROR in ReadSinoParams3DParallel: Number of channels must be a positive integer. And it must be specified.\n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%d\n", &(sinoparams->NViews));
    if(sinoparams->NViews <= 0){
        fprintf(stderr,"ERROR in ReadSinoParams3DParallel: Number of views must be a positive integer. And it must be specified.\n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%f\n", &(sinoparams->DeltaChannel));
    if(sinoparams->DeltaChannel <= 0){
        fprintf(stderr,"ERROR in ReadSinoParams3DParallel: Detector-channel spacing must be a positive floating point. And it must be specified.\n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%f\n", &(sinoparams->CenterOffset)); /* NOTE : THIS IS IN UNITS OF NUMBER OF CHANNELS RATHER THAN ACTUAL DISPLACEMENT in mm */
    if(fabs(sinoparams->CenterOffset) >= sinoparams->NChannels){
        fprintf(stderr,"ERROR in ReadSinoParams3DParallel: Detector-center offset cannot be greater than number of channels \n");
        exit(-1);
    }

    fgets(tag, 200, fp);
    fscanf(fp, "%d\n", &(sinoparams->NSlices));
    if(sinoparams->NSlices <= 0){
        fprintf(stderr,"ERROR in ReadSinoParams3DParallel: Number of slices must be a positive integer. And it must be specified.\n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%f\n", &(sinoparams->DeltaSlice));
    if(sinoparams->DeltaSlice <= 0){
        fprintf(stderr,"ERROR in ReadSinoParams3DParallel: Spacing between slices must be a positive floating point. And it must be specified.\n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%d\n", &(sinoparams->FirstSliceNumber));
    if(sinoparams->FirstSliceNumber <= 0){
        fprintf(stderr,"ERROR in ReadSinoParams3DParallel: First slice must be a non-negative integer. And it must be specified.\n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%s\n", AngleListFileName_relative); /* List of View angles */

    char* dir = dirname(fname);

    sprintf(AngleListFileName, "%s/%s", dir, AngleListFileName_relative); 
    printf("%s\n", AngleListFileName);

    fclose(fp);
    
    /* Read in list of View Angles */
    sinoparams->ViewAngles = (float *)get_spc(sinoparams->NViews, sizeof(float));
    
    if ((fp = fopen(AngleListFileName, "r")) == NULL)
    {
        fprintf(stderr, "ERROR in ReadSinoParams3DParallel: can't open file containing list of view angles %s.\n", AngleListFileName);
        exit(-1);
    }
    
    for(i=0;i<sinoparams->NViews;i++)
    {
        if(fscanf(fp,"%f\n",&(sinoparams->ViewAngles[i])) == 0)
       {
         fprintf(stderr, "ERROR in ReadSinoParams3DParallel: List of view angles in file %s terminated early.\n", AngleListFileName);
         exit(-1);
       }
    }
    
    fclose(fp);
    
    return 0;
}


/* Utility for reading 3D parallel beam sinogram data */
/* Warning: Memory must be allocated before use */
/* Returns 0 if no error occurs */
int ReadSinoData3DParallel(
                           char *fname,   /* Input: Reads sinogram data from <fname>_slice<InitialIndex>.2Dsinodata to <fname>_slice<FinalIndex>.2Dsinodata */
                           struct Sino3DParallel *sinogram)  /* Input/Output: Uses sinogram parameters and reads sinogram data into data structure */
{
    char slicefname[200];
    char *sliceindex;
    int i,NSlices,NChannels,NViews,FirstSliceNumber;
    struct Sino2DParallel SingleSliceSinogram;
    
    strcat(fname,"_slice"); /* <fname>_slice */
    sliceindex= (char *)malloc(MBIR_MODULAR_MAX_NUMBER_OF_SLICE_DIGITS);
    
    NSlices = sinogram->sinoparams.NSlices ;
    NChannels = sinogram->sinoparams.NChannels;
    NViews = sinogram->sinoparams.NViews;
    FirstSliceNumber=sinogram->sinoparams.FirstSliceNumber;
    
    /* Copy necessary slice information */
    SingleSliceSinogram.sinoparams.NChannels = NChannels;
    SingleSliceSinogram.sinoparams.NViews = NViews;
 
    printf("\nReading 3-D Projection Data ... \n");
    
    for(i=0;i<NSlices;i++)
    {
        SingleSliceSinogram.sino = sinogram->sino[i];  /* pointer to beginning of data for i-th slice */
        
        /* slice index : integer to string conversion with fixed precision */
        sprintf(sliceindex,"%.*d",MBIR_MODULAR_MAX_NUMBER_OF_SLICE_DIGITS,i+FirstSliceNumber);
        
        /* Obtain file name for the given slice */
        strcpy(slicefname,fname);
        strcat(slicefname,sliceindex); /* append slice index */
        
        if(ReadSinoData2DParallel(slicefname, &SingleSliceSinogram))
        {   fprintf(stderr, "Error in ReadSinoData3DParallel : Unable to read sinogram data for slice %d from file %s \n",i,slicefname);
            exit(-1);
        }
    }
    
    free((void *)sliceindex); 

    return 0;
}


/* Utility for reading weights for 3D sinogram projections data */
/* Warning: Memory must be allocated before use */
/* Returns 0 if no error occurs */
int ReadWeights3D(
                    char *fname,       /* Input: Reads sinogram data from <fname>_slice<InitialIndex>.2Dweightdata to <fname>_slice<FinalIndex>.2Dweightdata */
                    struct Sino3DParallel *sinogram) /* Input/Output: Uses sinogram parameters and reads sinogram data into data structure */
{
    char slicefname[200];
    char *sliceindex;
    int i,NSlices,NChannels,NViews,FirstSliceNumber;
    struct Sino2DParallel SingleSliceSinogram;
    
    strcat(fname,"_slice"); /* <fname>_slice */
    sliceindex= (char *)malloc(MBIR_MODULAR_MAX_NUMBER_OF_SLICE_DIGITS);
    
    NSlices = sinogram->sinoparams.NSlices ;
    NChannels = sinogram->sinoparams.NChannels;
    NViews = sinogram->sinoparams.NViews;
    FirstSliceNumber = sinogram->sinoparams.FirstSliceNumber;
    
    /* Copy necessary slice information */
    SingleSliceSinogram.sinoparams.NChannels = NChannels;
    SingleSliceSinogram.sinoparams.NViews = NViews;
    
    printf("\nReading 3-D Sinogram Weights Data ... \n");
    
    for(i=0;i<NSlices;i++)
    {
        SingleSliceSinogram.weight = sinogram->weight[i]; /* pointer to beginning of data for i-th slice */
        
        /* slice index : integer to string conversion with fixed precision */
        sprintf(sliceindex,"%.*d",MBIR_MODULAR_MAX_NUMBER_OF_SLICE_DIGITS,i+FirstSliceNumber);
        
        /* Obtain file name for the given slice */
        strcpy(slicefname,fname);
        strcat(slicefname,sliceindex); /* append slice index */
        
        if(ReadWeights2D(slicefname, &SingleSliceSinogram))
        {   fprintf(stderr, "Error in ReadWeights3D : Unable to read sinogram weight data for slice %d from file %s \n",i,slicefname);
            exit(-1);
        }
    }
    free((void *)sliceindex); 
    return 0;
}

/* Utility for writing out 3D parallel beam sinogram parameters and data */
/* Returns 0 if no error occurs */
int WriteSino3DParallel(
                        char *fname,             /* Input: Writes sinogram parameters to <fname>.sinoparams and data (if available) to ... */
                                                 /* <fname>_slice<InitialIndex>.2Dsinodata to <fname>_slice<FinalIndex>.2Dsinodata  */
                        struct Sino3DParallel *sinogram)  /* Input: Writes out sinogram parameters and data */
{
    char slicefname[200];
    char *sliceindex;
    int i,NSlices,NChannels,NViews,FirstSliceNumber;
    struct Sino2DParallel SingleSliceSinogram;
    
    strcat(fname,"_slice"); /* <fname>_slice */
    sliceindex= (char *)malloc(MBIR_MODULAR_MAX_NUMBER_OF_SLICE_DIGITS);
    
    NSlices = sinogram->sinoparams.NSlices ;
    NChannels = sinogram->sinoparams.NChannels;
    NViews = sinogram->sinoparams.NViews;
    FirstSliceNumber = sinogram->sinoparams.FirstSliceNumber;
    
    /* Copy necessary slice information */
    SingleSliceSinogram.sinoparams.NChannels = NChannels;
    SingleSliceSinogram.sinoparams.NViews = NViews;
    
    printf("\nWriting 3-D Projection Data ... \n");
    
    for(i=0;i<NSlices;i++)
    {
        SingleSliceSinogram.sino = sinogram->sino[i];  /* pointer to beginning of data for i-th slice */
        
        /* slice index : integer to string conversion with fixed precision */
        sprintf(sliceindex,"%.*d",MBIR_MODULAR_MAX_NUMBER_OF_SLICE_DIGITS,i+FirstSliceNumber);
        
        /* Obtain file name for the given slice */
        strcpy(slicefname,fname);
        strcat(slicefname,sliceindex); /* append slice index */
        
        if(WriteSino2DParallel(slicefname, &SingleSliceSinogram))
        {   fprintf(stderr, "Error in WriteSinoData3DParallel : Unable to write sinogram data for slice %d from file %s \n",i,slicefname);
            exit(-1);
        }
    }
    free((void *)sliceindex); 
    return 0;
}


/* Utility for writing out weights for 3D parallel beam sinogram data */
/* Returns 0 if no error occurs */
int WriteWeights3D(
                   char *fname,        /* Input: Writes sinogram measurement weights <fname>_slice<InitialIndex>.2Dweightdata to <fname>_slice<FinalIndex>.2Dweightdata */
                   struct Sino3DParallel *sinogram) /* Input: Sinogram data structure */
{
    char slicefname[200];
    char *sliceindex;
    int i,NSlices,NChannels,NViews,FirstSliceNumber;
    struct Sino2DParallel SingleSliceSinogram;
    
    strcat(fname,"_slice"); /* <fname>_slice */
    sliceindex= (char *)malloc(MBIR_MODULAR_MAX_NUMBER_OF_SLICE_DIGITS);
    
    NSlices = sinogram->sinoparams.NSlices ;
    NChannels = sinogram->sinoparams.NChannels;
    NViews = sinogram->sinoparams.NViews;
    FirstSliceNumber = sinogram->sinoparams.FirstSliceNumber;
    
    /* Copy necessary slice information */
    SingleSliceSinogram.sinoparams.NChannels = NChannels;
    SingleSliceSinogram.sinoparams.NViews = NViews;
    
    printf("\nWriting 3-D Sinogram Weights Data ... \n");
    
    for(i=0;i<NSlices;i++)
    {
        SingleSliceSinogram.weight = sinogram->weight[i];  /* pointer to beginning of data for i-th slice */
        
        /* slice index : integer to string conversion with fixed precision */
        sprintf(sliceindex,"%.*d",MBIR_MODULAR_MAX_NUMBER_OF_SLICE_DIGITS,i+FirstSliceNumber);
        
        /* Obtain file name for the given slice */
        strcpy(slicefname,fname);
        strcat(slicefname,sliceindex); /* append slice index */
        
        if(WriteWeights2D(slicefname, &SingleSliceSinogram))
        {   fprintf(stderr, "Error in WriteWeights3D: Unable to write sinogram weight data for slice %d from file %s \n",i,slicefname);
            exit(-1);
        }
    }
    free((void *)sliceindex); 
    return 0;
}

/* Utility for allocating memory for Sino */
/* Returns 0 if no error occurs */
int AllocateSinoData3DParallel(
                               struct Sino3DParallel *sinogram)  /* Input: Sinogram parameters data structure */
{
    printf("\nAllocating Sinogram Memory ... \n");
    
    sinogram->sino   = (float **)multialloc(sizeof(float), 2, sinogram->sinoparams.NSlices,sinogram->sinoparams.NViews * sinogram->sinoparams.NChannels);
    sinogram->weight = (float **)multialloc(sizeof(float), 2, sinogram->sinoparams.NSlices,sinogram->sinoparams.NViews * sinogram->sinoparams.NChannels);
 
    return 0;
}


/* Utility for freeing memory allocated for ViewAngles and Sino */
/* Returns 0 if no error occurs */
int FreeSinoData3DParallel(
                           struct Sino3DParallel *sinogram)  /* Input: Sinogram parameters data structure */
{
    multifree(sinogram->sino,2);
    multifree(sinogram->weight,2);
    return 0;
}

/*******************************************/
/* Utilities for reading/writing 3D images */
/*******************************************/

/* VS : Utility for reading 2D Image parameters */
/* Returns 0 if no error occurs */
int ReadImageParams3D_old(
                      char *fname,                         /* Input: Reads image type parameter from <fname>.imgparams */
                      struct ImageParams3D *imgparams)     /* Output: Reads image parameters into data structure */
{
    FILE *fp;
    char tag[200];
    
    strcat(fname,".imgparams"); /* append file extension */
    
    if ((fp = fopen(fname, "r")) == NULL)
    {
        fprintf(stderr, "ERROR in ReadImageParams3D: can't open file %s.\n", fname);
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%d\n", &(imgparams->Nx));
    if(imgparams->Nx <= 0){
        fprintf(stderr,"ERROR in ReadImageParams3D: No. of pixels along horizontal direction, Nx, must be a positive integer. And it must be specified.\n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%d\n", &(imgparams->Ny));
    if(imgparams->Ny <= 0){
        fprintf(stderr,"ERROR in ReadImageParams3D: No. of pixels along vertical direction, Ny, must be a positive integer. And it must be specified. \n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%f\n", &(imgparams->Deltaxy));
    if(imgparams->Deltaxy <= 0){
        fprintf(stderr,"ERROR in ReadImageParams3D: Pixel Dimension (mm) must be a positive floating point. And it must be specified.\n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%f\n", &(imgparams->ROIRadius));
    if(imgparams->ROIRadius <= 0){
        fprintf(stderr,"ERROR in ReadImageParams3D: Region-of-Interest Radius (mm) must be a positive floating point. And it must be specified.\n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%d\n", &(imgparams->Nz));
    if(imgparams->Nz <= 0){
        fprintf(stderr,"ERROR in ReadImageParams3D: Number of slices must be a positive integer. And it must be specified.\n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%f\n", &(imgparams->DeltaZ));
    if(imgparams->DeltaZ <= 0){
        fprintf(stderr,"ERROR in ReadImageParams3D: spacing between slices (mm) must be a positive floating point. And it must be specified.\n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%d\n", &(imgparams->FirstSliceNumber));
    if(imgparams->FirstSliceNumber <= 0){
        fprintf(stderr,"ERROR in ReadImageParams3D: First Slice Index must be a positive integer. And it must be specified.\n");
        exit(-1);
    }
    
    fclose(fp);
    return 0 ;
}


void printImageParams3D(struct ImageParams3D *imgparams)
{
    fprintf(stdout, "IMAGE PARAMETERS:\n");
    fprintf(stdout, " - Number of Pixels within a single slice in X direction = %d\n", imgparams->Nx);
    fprintf(stdout, " - Number of Pixels within a single slice in Y direction = %d\n", imgparams->Ny);
    fprintf(stdout, " - Number of Slices to reconstruct                       = %d \n", imgparams->Nz);
    fprintf(stdout, " - Pixel width  in XY plane                              = %.7f (mm)\n", imgparams->Deltaxy);
    fprintf(stdout, " - Spacing between slices                                = %.7f (mm)\n", imgparams->DeltaZ);
    fprintf(stdout, " - First Slice Index                                     = %d \n", imgparams->FirstSliceNumber);
    fprintf(stdout, " - ROIRadius                                             = %.7f (mm)\n", imgparams->ROIRadius);
}
           
/* Utility for reading 3D image data */
/* Warning: Memory must be allocated before use */
/* Returns 0 if no error occurs */
int ReadImage3D(
                char *fname,              /* Input: Reads 2D image data from <fname>_slice<InitialIndex>.2Dimgdata to <fname>_slice<FinalIndex>.2Dimgdata */
                struct Image3D *Image)    /* Output: Reads Image parameters and data (if available) into data structure */
{
    char slicefname[200];
    char *sliceindex;
    int i,Nx,Ny,Nz,FirstSliceNumber;
    struct Image2D SingleSliceImage;
    
    strcat(fname,"_slice"); /* <fname>_slice */
    sliceindex= (char *)malloc(MBIR_MODULAR_MAX_NUMBER_OF_SLICE_DIGITS);
    
    Nx = Image->imgparams.Nx ;
    Ny = Image->imgparams.Ny;
    Nz = Image->imgparams.Nz;
    FirstSliceNumber = Image->imgparams.FirstSliceNumber;
    
    /* Copy necessary slice information */
    SingleSliceImage.imgparams.Nx = Nx;
    SingleSliceImage.imgparams.Ny = Ny;
    
    for(i=0;i<Nz;i++)
    {
        SingleSliceImage.image = Image->image[i];  /* pointer to beginning of data for i-th slice */
        
        /* slice index : integer to string conversion with fixed precision */
        sprintf(sliceindex,"%.*d",MBIR_MODULAR_MAX_NUMBER_OF_SLICE_DIGITS,i+FirstSliceNumber);
        
        /* Obtain file name for the given slice */
        strcpy(slicefname,fname);
        strcat(slicefname,sliceindex); /* append slice index */
        
        if(ReadImage2D(slicefname, &SingleSliceImage))
        {   fprintf(stderr, "Error in ReadImage3D : Unable to read image data for slice %d from file %s \n",i,slicefname);
            exit(-1);
        }
    }
    free((void *)sliceindex); 
    return 0;
}
           

/* Utility for writing 3D image parameters and data */
/* Returns 0 if no error occurs */
int WriteImage3D(
                char *fname,            /* Input: Writes to image parameters to <fname>.imgparams and data (if available) to .. */
                                        /* <fname>_slice<InitialIndex>.2Dimgdata to <fname>_slice<FinalIndex>.2Dimgdata */
                struct Image3D *Image)  /* Input: Image data structure (both data and params) */
{
    char slicefname[200];
    char *sliceindex;
    int i,Nx,Ny,Nz,FirstSliceNumber;
    struct Image2D SingleSliceImage;
    
    strcat(fname,"_slice"); /* <fname>_slice */
    sliceindex= (char *)malloc(MBIR_MODULAR_MAX_NUMBER_OF_SLICE_DIGITS);
    
    Nx = Image->imgparams.Nx ;
    Ny = Image->imgparams.Ny;
    Nz = Image->imgparams.Nz;
    FirstSliceNumber = Image->imgparams.FirstSliceNumber;
    
    /* Copy necessary slice information */
    SingleSliceImage.imgparams.Nx = Nx;
    SingleSliceImage.imgparams.Ny = Ny;
    
    printf("\nWriting 3-D Image ... \n");
    for(i=0;i<Nz;i++)
    {
        SingleSliceImage.image = Image->image[i];  /* pointer to beginning of data for i-th slice */
        
        /* slice index : integer to string conversion with fixed precision */
        sprintf(sliceindex,"%.*d",MBIR_MODULAR_MAX_NUMBER_OF_SLICE_DIGITS,i+FirstSliceNumber);
        
        /* Obtain file name for the given slice */
        strcpy(slicefname,fname);
        strcat(slicefname,sliceindex); /* append slice index */
        
        if(WriteImage2D(slicefname, &SingleSliceImage))
        {   fprintf(stderr, "Error in WriteImage3D : Unable to write image data for slice %d from file %s \n",i,slicefname);
            exit(-1);
        }
    }
    free((void *)sliceindex); 
    return 0;
}
           

/* Utility for allocating memory for Image */
/* Returns 0 if no error occurs */
int AllocateImageData3D(
                        struct Image3D *Image)    /* Input: Image data structure */
{
    Image->image = (float **)multialloc(sizeof(float), 2, Image->imgparams.Nz, Image->imgparams.Nx * Image->imgparams.Ny);
    return 0;
}

/* Utility for freeing memory for Image */
/* Returns 0 if no error occurs */
int FreeImageData3D(
                    struct Image3D *Image)    /* Input: Image data structure */
{
    multifree(Image->image,2);
    return 0;
}


/**************************************************/
/* Utilities for reading in reconstruction params */
/**************************************************/

/* Read prior model information */

int ReadReconParamsQGGMRF3D_old(char *fname, struct ReconParamsQGGMRF3D *reconparams)
{
    FILE *fp;
    char tag[200];
    
    strcat(fname,".reconparams"); /* append file extension */
    
    if ((fp = fopen(fname, "r")) == NULL)
    {
        fprintf(stderr, "ERROR in ReadReconParamsQGGMRF3D: can't open file %s.\n", fname);
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%lf\n", &(reconparams->InitImageValue));
    if(reconparams->InitImageValue<= 0){
        fprintf(stderr,"ERROR in ReadReconParamsQGGMRF3D: Initial Image Value (mm^-1) should not be a negative number for our implmentation \n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%lf\n", &(reconparams->q));
    if(reconparams->q<= 1) {
        fprintf(stderr,"ERROR in ReadReconParamsQGGMRF3D: Parameter q must be greater than 1, typical choice is q=2 \n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%lf\n", &(reconparams->p));
    if(reconparams->p< 1 || reconparams->p >= reconparams->q){
        fprintf(stderr,"ERROR in ReadReconParamsQGGMRF3D: Parameter p must be in the range [1,q) \n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%lf\n", &(reconparams->T));
    if(reconparams->T <= 0){
        fprintf(stderr,"ERROR in ReadReconParamsQGGMRF3D: Parameter T is must be greater than zero \n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%lf\n", &(reconparams->SigmaX));
    if(reconparams->SigmaX <= 0){
        fprintf(stderr,"ERROR in ReadReconParamsQGGMRF3D: Parameter SigmaX must be greater than zero \n");
        exit(-1);
    }
    
    /* b_nearest and b_diag are neighborhood weights (2D) */
    /* default values : b_nearest=1 and b_diag=1 */
    
    fgets(tag, 200, fp);
    fscanf(fp, "%lf\n", &(reconparams->b_nearest));
    if(reconparams->b_nearest <= 0){
        fprintf(stderr,"ERROR in ReadReconParamsQGGMRF3D: Parameter b_nearest must be greater than zero \n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%lf\n", &(reconparams->b_diag));
    if(reconparams->b_diag <= 0){
        fprintf(stderr,"ERROR in ReadReconParamsQGGMRF3D: Parameter b_diag must be greater than zero \n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%lf\n", &(reconparams->b_interslice));
    if(reconparams->b_interslice < 0){
        fprintf(stderr,"ERROR in ReadReconParamsQGGMRF3D: Parameter b_diag must be greater than zero \n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%lf\n", &(reconparams->StopThreshold));
    //SJK: allow 0 (or negative) StopThreshold to disable and "run maximum iterations"
    //if(reconparams->StopThreshold <= 0){
    //    fprintf(stderr,"ERROR in ReadReconParamsQGGMRF3D: Stop Threshold in %% must be greater than zero \n");
    //    exit(-1);
    //}
    
    fgets(tag, 200, fp);
    fscanf(fp, "%d\n",  &(reconparams->MaxIterations));
    if(reconparams->MaxIterations <= 0){
        fprintf(stderr,"ERROR in ReadReconParamsQGGMRF3D: Maximum no. of iterations must be a positive integer \n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%d\n", &(reconparams->NSlices));
    if(reconparams->NSlices <= 0){
        fprintf(stderr,"ERROR in ReadReconParamsQGGMRF3D: Number of slices must be a positive integer. And it must be specified.\n");
        exit(-1);
    }
    
    fgets(tag, 200, fp);
    fscanf(fp, "%d\n", &(reconparams->FirstSliceNumber));
    if(reconparams->FirstSliceNumber <= 0){
        fprintf(stderr,"ERROR in ReadReconParamsQGGMRF3D: First Slice Index must be a positive integer. And it must be specified.\n");
        exit(-1);
    }
    
    fclose(fp);
    
    return 0;
}

/* Print prior model information */

void printReconParamsQGGMRF3D(struct ReconParamsQGGMRF3D *reconparams)
{
    fprintf(stdout, "PRIOR PARAMETERS:\n");
    fprintf(stdout, " - Q-GGMRF Prior Parameter, q                            = %f\n", reconparams->p);
    fprintf(stdout, " - Q-GGMRF Prior Parameter, p                            = %f\n", reconparams->q);
    fprintf(stdout, " - Q-GGMRF Prior Parameter, T                            = %f\n", reconparams->T);
    fprintf(stdout, " - Prior Regularization parameter, SigmaX                = %.7f (mm^-1)\n", reconparams->SigmaX);
    fprintf(stdout, " - Scaling for weight matrix, SigmaY (W <- W/SigmaY^2)   = %.7f (mm^-1)\n", reconparams->SigmaY);
    fprintf(stdout, " - Prior weight for nearest neighbors within slice       = %.7f\n", reconparams->b_nearest);
    fprintf(stdout, " - Prior weight for diagonal neighbors within slice      = %.7f\n", reconparams->b_diag);
    fprintf(stdout, " - Prior weight for nearest neighbors in adjacent slices = %.7f\n", reconparams->b_interslice);
    fprintf(stdout, " - Inital image value                                    = %-10f (mm-1)\n", reconparams->InitImageValue);
    fprintf(stdout, " - Stop threshold for convergence                        = %.7f %%\n", reconparams->StopThreshold);
    fprintf(stdout, " - Maximum number of ICD iterations                      = %d\n", reconparams->MaxIterations);
    fprintf(stdout, " - Positivity constraint flag                            = %d\n", reconparams->Positivity);
    //fprintf(stdout, " - Number of slices to reconstruct                               = %d\n", reconparams->NSlices);
    //fprintf(stdout, " - First slice Index                                             = %d\n", reconparams->FirstSliceNumber);
}





