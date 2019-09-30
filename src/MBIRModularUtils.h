#ifndef MBIR_MODULAR_UTILS_H
#define MBIR_MODULAR_UTILS_H

#include "MBIRModularDefs.h"

/**********************************************/
/*  Utilities for reading/writing 3D sinogram */
/**********************************************/

/* Utility for reading 3D parallel beam sinogram parameters */
/* Returns 0 if no error occurs */
int ReadSinoParams3DParallel(
  char *fname,                               /* Input: Reads sinogram parameters from <fname>.sinoparams */
  struct SinoParams3DParallel *sinoparams);  /* Output: Reads sinogram parameters into data structure */

/* Utility for writing out 3D parallel beam sinogram parameters and data */
/* Returns 0 if no error occurs */
int WriteSino3DParallel(
                        char *fname,             /* Input: Writes sinogram parameters to <fname>.sinoparams and data (if available) to ... */
                                                 /* <fname>_slice<InitialIndex>.2Dsinodata to <fname>_slice<FinalIndex>.2Dsinodata  */
                        struct Sino3DParallel *sinogram); /* Input: Writes out sinogram parameters and data */

/* Utility for writing out weights for 3D parallel beam sinogram data */
/* Returns 0 if no error occurs */
int WriteWeights3D(
                char *fname,             /* Input: Writes sinogram measurement weights <fname>_slice<InitialIndex>.2Dweightdata to <fname>_slice<FinalIndex>.2Dweightdata */
                struct Sino3DParallel *sinogram); /* Input: Sinogram data structure */

/* Utility for reading 3D parallel beam sinogram data */
/* Warning: Memory must be allocated before use */
/* Returns 0 if no error occurs */
int ReadSinoData3DParallel(
                           char *fname,               /* Input: Reads sinogram data from <fname>_slice<InitialIndex>.2Dsinodata to <fname>_slice<FinalIndex>.2Dsinodata */
                           struct Sino3DParallel *sinogram);  /* Input/Output: Uses sinogram parameters and reads sinogram data into data structure */

int ReadWeights3D(
                 char *fname,             /* Input: Read sinogram measurement weights from <fname>_slice<InitialIndex>.2Dweightdata to <fname>_slice<FinalIndex>.2Dweightdata */
                 struct Sino3DParallel *sinogram); /* Input: Stores weights into Sinogram Data Structure  */


/* Utility for allocating memory for Sino */
/* Returns 0 if no error occurs */
int AllocateSinoData3DParallel(
                               struct Sino3DParallel *sinogram);  /* Input: Sinogram parameters data structure */

/* Utility for freeing memory allocated for ViewAngles and Sino */
/* Returns 0 if no error occurs */
int FreeSinoData3DParallel(
                           struct Sino3DParallel *sinogram); /* Input: Sinogram parameters data structure */

/*******************************************/
/* Utilities for reading/writing 3D images */
/*******************************************/

/* VS : Utility for reading 3D Image parameters */
/* Returns 0 if no error occurs */
int ReadImageParams3D(
  char *fname,                         /* Input: Reads image type parameter from <fname>.imgparams */
  struct ImageParams3D *imgparams);    /* Output: Reads image parameters into data structure */

/* Utility for reading 3D image data */
/* Warning: Memory must be allocated before use */
/* Returns 0 if no error occurs */
int ReadImage3D(
                char *fname,              /* Input: Reads 2D image data from <fname>_slice<InitialIndex>.2Dimgdata to <fname>_slice<FinalIndex>.2Dimgdata */
                struct Image3D *Image);   /* Output: Reads Image parameters and data (if available) into data structure */


/* Utility for writing 3D image parameters and data */
/* Returns 0 if no error occurs */
int WriteImage3D(
                 char *fname,              /* Input: Writes to image parameters to <fname>.imgparams and data (if available) to .. */
                                           /* <fname>_slice<InitialIndex>.2Dimgdata to <fname>_slice<FinalIndex>.2Dimgdata */
                 struct Image3D *Image);   /* Input: Image data structure (both data and params) */


/* Utility for allocating memory for Image */
/* Returns 0 if no error occurs */
int AllocateImageData3D(
                        struct Image3D *Image);  /* Input: Image data structure */


/* Utility for freeing memory for Image */
/* Returns 0 if no error occurs */
int FreeImageData3D(
                    struct Image3D *Image);    /* Input: Image data structure */


/**************************************************/
/* Utilities for reading in reconstruction params */
/**************************************************/

int ReadReconParamsQGGMRF3D(
                             char *fname,
                             struct ReconParamsQGGMRF3D *reconparams);

/***********************************/
/* Miscellanous Functions         */
/* Remove or shift them out later */
/***********************************/

void printReconParamsQGGMRF3D(struct ReconParamsQGGMRF3D *reconparams);
void printImageParams3D(struct ImageParams3D *imgparams);
void printSinoParams3DParallel(struct SinoParams3DParallel *sinoparams);





/**********************************************/
/*  Utilities for reading/writing 2D sinogram */
/**********************************************/

/* Utility for writing out 2D parallel beam sinogram data */
/* Returns 0 if no error occurs */
int WriteSino2DParallel(
	char *fname,				/* Input: Writes sinogram parameters to <fname>.2dsinodata */
	struct Sino2DParallel *sinogram);	/* Input: Writes out sinogram parameters and data */

int WriteWeights2D(
	char *fname,				/* Input: Writes sinogram measurement weights to <fname>.wght */
	struct Sino2DParallel *sinogram);	/* Input: Sinogram data structure */

/* Utility for reading 2D parallel beam sinogram data */
/* Warning: Memory must be allocated before use */
/* Returns 0 if no error occurs */
int ReadSinoData2DParallel(
	char *fname,				/* Input: Reads sinogram data from <fname>.2dsinodata */
	struct Sino2DParallel *sinogram);	/* Input/Output: Uses sinogram parameters and reads into sinogram data structure */

int ReadWeights2D(
	char *fname,				/* Input: Read sinogram weights from <fname>.wght */
	struct Sino2DParallel *sinogram);	/* Input: Stores weights into Sinogram Data Structure  */

/* Utility for allocating memory for Sino */
/* Returns 0 if no error occurs */
int AllocateSinoData2DParallel(
	struct Sino2DParallel *sinogram);	/* Input: Sinogram parameters data structure */

/* Utility for freeing memory allocated for ViewAngles and Sino */
/* Returns 0 if no error occurs */
int FreeSinoData2DParallel(
	struct Sino2DParallel *sinogram);	/* Input: Sinogram parameters data structure */

/*******************************************/
/* Utilities for reading/writing 2D images */
/*******************************************/

/* Utility for reading 2D image parameters and data */
/* Warning: Memory must be allocated before use */
/* Returns 0 if no error occurs */
int ReadImage2D(
	char *fname,		/* Input: Reads 2D image data from <fname>.2dimgdata */
	struct Image2D *Image);	/* Output: Reads into data structure */

/* Utility for writing 2D image parameters and data */
/* Returns 0 if no error occurs */
int WriteImage2D(
	char *fname,		/* Input: Writes to image data to <fname>.2dimgdata */
	struct Image2D *Image);	/* Input: Image data structure (both data and params) */

/* Utility for allocating memory for Image */
/* Returns 0 if no error occurs */
int AllocateImageData2D(
	struct Image2D *Image);	/* Input: Image data structure */

/* Utility for freeing memory for Image */
/* Returns 0 if no error occurs */
int FreeImageData2D(
	struct Image2D *Image);	/* Input: Image data structure */

/******************************************************/
/* Utilities for reading/writing sparse System matrix */
/******************************************************/

/* Utility for reading/allocating the Sparse System Matrix */
/* Returns 0 if no error occurs */
/* Warning: Memory is allocated for the data structure inside subroutine */
int ReadSysMatrix2D(
	char *fname,		/* Input: Basename of Sparse System Matrix file <fname>.2dsysmatrix */
	struct SysMatrix2D *A);	/* Ouput: Sparse system matrix structure */

/* Utility for writing the Sparse System Matrix */
/* Returns 0 if no error occurs */
int WriteSysMatrix2D(
	char *fname,		/* Input: Basename of output file <fname>.2dsysmatrix */
	struct SysMatrix2D *A);	/* Input: Sparse system matrix structure */

/* Utility for freeing memory from Sparse System Matrix */
/* Returns 0 if no error occurs */
int FreeSysMatrix2D(
	struct SysMatrix2D *A);	/* Input: Free all memory from data structure */



#endif /* MBIR_MODULAR_UTILS_H */

