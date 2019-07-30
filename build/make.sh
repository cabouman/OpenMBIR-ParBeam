

cd "$(dirname $0)"

SOURCES=""
SOURCES="${SOURCES} ../SourceCode/2D/MBIR/mbir_2D.c"
SOURCES="${SOURCES} ../SourceCode/2D/MBIR/icd_2D.c"
SOURCES="${SOURCES} ../SourceCode/2D/MBIR/initialize_2D.c"
SOURCES="${SOURCES} ../SourceCode/2D/MBIR/recon_2D.c"
SOURCES="${SOURCES} ../SourceCode/2D/Generate_SystemMatrix/A_comp_2D.c"
SOURCES="${SOURCES} ../SourceCode/2D/Generate_SystemMatrix/Gen_SysMatrix_2D.c"
SOURCES="${SOURCES} ../SourceCode/MBIRModularUtils_2D.c"
SOURCES="${SOURCES} ../SourceCode/allocate.c"
SOURCES="${SOURCES} ../SourceCode/MBIRModularUtils_3D.c"
SOURCES="${SOURCES} ../SourceCode/3D/MBIR/initialize_3D.c"
SOURCES="${SOURCES} ../SourceCode/3D/MBIR/icd_3D.c"
SOURCES="${SOURCES} ../SourceCode/3D/MBIR/recon_3D.c"
SOURCES="${SOURCES} ../SourceCode/3D/MBIR/mbir_3D.c"
SOURCES="${SOURCES} ../SourceCode/3D/Generate_SystemMatrix/Gen_SysMatrix.c"
SOURCES="${SOURCES} ../SourceCode/3D/Generate_SystemMatrix/A_comp_3D.c"
gcc -Wall -pedantic -c $SOURCES






OBJECTS=""
OBJECTS="${OBJECTS} ../SourceCode/2D/MBIR/mbir_2D.o"
OBJECTS="${OBJECTS} ../SourceCode/2D/MBIR/icd_2D.o"
OBJECTS="${OBJECTS} ../SourceCode/2D/MBIR/initialize_2D.o"
OBJECTS="${OBJECTS} ../SourceCode/2D/MBIR/recon_2D.o"
OBJECTS="${OBJECTS} ../SourceCode/2D/Generate_SystemMatrix/A_comp_2D.o"
OBJECTS="${OBJECTS} ../SourceCode/2D/Generate_SystemMatrix/Gen_SysMatrix_2D.o"
OBJECTS="${OBJECTS} ../SourceCode/MBIRModularUtils_2D.o"
OBJECTS="${OBJECTS} ../SourceCode/allocate.o"
OBJECTS="${OBJECTS} ../SourceCode/MBIRModularUtils_3D.o"
OBJECTS="${OBJECTS} ../SourceCode/3D/MBIR/initialize_3D.o"
OBJECTS="${OBJECTS} ../SourceCode/3D/MBIR/icd_3D.o"
OBJECTS="${OBJECTS} ../SourceCode/3D/MBIR/recon_3D.o"
OBJECTS="${OBJECTS} ../SourceCode/3D/MBIR/mbir_3D.o"
OBJECTS="${OBJECTS} ../SourceCode/3D/Generate_SystemMatrix/Gen_SysMatrix.o"
OBJECTS="${OBJECTS} ../SourceCode/3D/Generate_SystemMatrix/A_comp_3D.o"
rm $OBJECTS






