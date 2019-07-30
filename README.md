# OpenMBIR
## BASELINE MBIR RECONSTRUCTION SOFTWARE FOR 2D and 3D PARALLEL-BEAM  CT

## HARDWARE REQUIREMENTS:
   RAM requirement is 128MB for a quick demo (“Demo_Fast.sh” Shepp-Logan Phantom Data)
   RAM requirement is 4GB or a slow demo,  run the script “Demo_Slow.sh” (FeOOH Data)

## SOFTWARE REQUIREMENTS:
  Operating system: Microsoft Windows, Mac OS X, Linux.  
  Compiler: GCC compiler.
  MATLAB

## INSTRUCTIONS:

1) Compile the code-package:
   Run the script "./build/build.sh" to compile the 2D and 3D code packages. 
   
2) Run demos:
   (a) For fast demo, run the scripts "Demo/2D/Demo_Fast.sh" or "Demo/3D/Demo_Fast.sh".
   (b) For slow demo, run the scripts "Demo/2D/Demo_Slow.sh" or "Demo/3D/Demo_Slow.sh".

3) Display the result using MATLAB :
   Run the script “DisplayResult.m” located under the directory “DisplayResult/<2D or 3D>/Demo_<TYPE>/”
   where TYPE = either “Fast” or “Slow”


## SUPPLMENTARY DETAILS:

1) Executable files for the 2 portions of the code-package are located under the directories:

   (a) SourceCode/<2D or 3D>/Generate_SystemMatrix/
   (b) SourceCode/<2D or 3D>/MBIR/

2) Run scripts :
  
   (a) The scripts “Demo_<TYPE>.sh” are wrapper (higher-level) scripts to the “run.sh” script under the directory “Data/<2D or 3D>/Demo_<TYPE>/” ;
       where TYPE = either “Fast” , “Slow” or “User”

   (b) The “run.sh” script invokes the executable files through command line for the purposes of :
       (i)   Generation of System Matrix
       (ii) MBIR Reconstruction 

   (c) Format for command lines of the executable files are mentioned in the “run.sh” script, as well as in Documentation
 
