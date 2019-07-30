

cd "$(dirname $0)"

FLAGS="-Wall -pedantic -lm"

SOURCES=""
SOURCES="${SOURCES} ../SourceCode/2D/MBIR/mbir_2D.c"
SOURCES="${SOURCES} ../SourceCode/2D/MBIR/icd_2D.c"
SOURCES="${SOURCES} ../SourceCode/2D/MBIR/initialize_2D.c"
SOURCES="${SOURCES} ../SourceCode/2D/MBIR/recon_2D.c"
SOURCES="${SOURCES} ../SourceCode/2D/Generate_SystemMatrix/A_comp_2D.c"
SOURCES="${SOURCES} ../SourceCode/2D/Generate_SystemMatrix/Gen_SysMatrix_2D.c"

SOURCES="${SOURCES} ../SourceCode/3D/MBIR/initialize_3D.c"
SOURCES="${SOURCES} ../SourceCode/3D/MBIR/icd_3D.c"
SOURCES="${SOURCES} ../SourceCode/3D/MBIR/recon_3D.c"
SOURCES="${SOURCES} ../SourceCode/3D/MBIR/mbir_3D.c"
SOURCES="${SOURCES} ../SourceCode/3D/Generate_SystemMatrix/Gen_SysMatrix_3D.c"
SOURCES="${SOURCES} ../SourceCode/3D/Generate_SystemMatrix/A_comp_3D.c"

SOURCES="${SOURCES} ../SourceCode/allocate.c"
SOURCES="${SOURCES} ../SourceCode/MBIRModularUtils_2D.c"
SOURCES="${SOURCES} ../SourceCode/MBIRModularUtils_3D.c"

gcc $FLAGS -c $SOURCES


UTILS="allocate.o MBIRModularUtils_2D.o MBIRModularUtils_3D.o"

gcc $FLAGS $UTILS A_comp_2D.o Gen_SysMatrix_2D.o                   -o Gen_SysMatrix_2D.bin
gcc $FLAGS $UTILS icd_2D.o initialize_2D.o recon_2D.o mbir_2D.o    -o mbir_2D.bin
gcc $FLAGS $UTILS A_comp_3D.o Gen_SysMatrix_3D.o                   -o Gen_SysMatrix_3D.bin
gcc $FLAGS $UTILS icd_3D.o initialize_3D.o recon_3D.o mbir_3D.o    -o mbir_3D.bin

rm *.o
mv *.bin ../bin








