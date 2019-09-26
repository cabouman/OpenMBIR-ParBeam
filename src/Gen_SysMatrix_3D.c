#include "MBIRModularUtils_3D.h"
#include "MBIRModularUtils_2D.h"
#include "allocate.h"
#include "A_comp_3D.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Command Line structure for Generating System matrix */
struct CmdLineSysGen
{
    char imgparamsFileName[200];  /* input file */
    char sinoparamsFileName[200]; /* input file */
    char SysMatrixFileName[200]; /* output file */
};

void readCmdLineSysGen(int argc, char *argv[], struct CmdLineSysGen *cmdline);
void PrintCmdLineUsage(char *ExecFileName);
int CmdLineHelpCheck(char *string);

int main(int argc, char *argv[])
{
    struct CmdLineSysGen cmdline;
    struct ImageParams3D imgparams;
    struct SinoParams3DParallel sinoparams;
    float **PixelDetector_profile;
    struct SysMatrix2D *A ;
    
    /* read Command Line */
    readCmdLineSysGen(argc, argv, &cmdline);

    /* read input arguments and parameters */
    ReadSinoParams3DParallel(cmdline.sinoparamsFileName, &sinoparams);
    ReadImageParams3D(cmdline.imgparamsFileName, &imgparams);
    printSinoParams3DParallel(&sinoparams);
    printImageParams3D(&imgparams);

    /* Compute Pixel-Detector Profile */
    PixelDetector_profile = ComputePixelProfile3DParallel(&sinoparams, &imgparams);  /* pixel-detector profile function */
    /* Compute Forward Matrix */
    A = ComputeSysMatrix3DParallel(&sinoparams, &imgparams, PixelDetector_profile);
    
    /* Write out System Matrix */
    if(WriteSysMatrix2D(cmdline.SysMatrixFileName, A))
    {  fprintf(stderr, "Error in writing out System Matrix to file %s through function WriteSysMatrix2D \n", cmdline.SysMatrixFileName);
       exit(-1);
    }
    /* Free System Matrix */
    if(FreeSysMatrix2D(A))
    {  fprintf(stderr, "Error System Matrix memory could not be freed through function FreeSysMatrix2D \n");
       exit(-1);
    }
    
	return 0;
}


void readCmdLineSysGen(int argc, char *argv[], struct CmdLineSysGen *cmdline)
{
    char ch;

    if(argc<7)
    {
        if(argc==2 && CmdLineHelpCheck(argv[1]))
        {
            PrintCmdLineUsage(argv[0]);
            exit(-1);
        }
        else
        {
            fprintf(stderr, "\nError : Improper Command line for exec-program %s, Number of arguments lower than needed \n",argv[0]);
            PrintCmdLineUsage(argv[0]);
            exit(-1);
        }
    }

    /* get options */
    while ((ch = getopt(argc, argv, "i:j:m:")) != EOF)
    {
        switch (ch)
        {
            case 'i':
            {
                sprintf(cmdline->imgparamsFileName, "%s", optarg);
                break;
            }
            case 'j':
            {
                sprintf(cmdline->sinoparamsFileName, "%s", optarg);
                break;
            }
            case 'm':
            {
                sprintf(cmdline->SysMatrixFileName, "%s", optarg);
                break;
            }
            default:
            {
                fprintf(stderr, "\nError : Unrecognized Command-line Symbol for exec-program %s \n",argv[0]);
                PrintCmdLineUsage(argv[0]);
                exit(-1);
                break;
            }
        }
    }
}


void PrintCmdLineUsage(char *ExecFileName)
{
    fprintf(stdout, "\nBASELINE MBIR RECONSTRUCTION SOFTWARE FOR 3D PARALLEL-BEAM CT \n");
    fprintf(stdout, "build time: %s, %s\n", __DATE__,  __TIME__);
    fprintf(stdout, "\nCommand line Format for Executable File %s : \n", ExecFileName);
    fprintf(stdout, "%s ./<Executable File Name>  -i <InputFileName>[.imgparams] -j <InputFileName>[.sinoparams] -m <OutputFileName>[.2Dsysmatrix] \n\n",ExecFileName);
    fprintf(stdout, "Note : The file extensions above enclosed in \"[ ]\" symbols are necessary \n");
    fprintf(stdout, "but should be omitted from the command line arguments\n");
}

/* check argument for common help switches */
int CmdLineHelpCheck(char *string)
{
    if( (strcmp(string,"-h")==0) || (strcmp(string,"-help")==0) || (strcmp(string,"--help")==0) || (strcmp(string,"help")==0) )
        return 1;
    else
        return 0;
}

