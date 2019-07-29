# OpenMBIR
## BASELINE MBIR RECONSTRUCTION SOFTWARE FOR 2D and 3DPARALLEL-BEAM  CT

## HARDWARE REQUIREMENTS:
   RAM requirement is 128MB for a quick demo (“Demo_Fast.sh” Shepp-Logan Phantom Data)
   RAM requirement is 4GB or a slow demo,  run the script “Demo_Slow.sh” (FeOOH Data)

## SOFTWARE REQUIREMENTS:
  Operating system: Microsoft Windows, Mac OS X, Linux.  
  Compiler: GCC compiler.
  MATLAB

## INSTRUCTIONS:

1) Compile the code-package:
   Run the scripts “Compile/2D/Compile.sh" and "Compile/3D/Compile.sh" to compile the 2D and 3D code packages. 
   
2) Execute the script on sample data sets:
   (a) For a quick demo, run the script “Demo_Fast.sh” under “Demo/<2D or 3D>” (2D - Shepp-Logan Phantom Data and 3D - FeOOH Data)
   (b) For a slow demo,  run the script “Demo_Slow.sh” under “Demo/<2D or 3D>” (2D - 2 slices of FeOOH Data   and 3D - 16 slices of FeOOH Data)  

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
 
