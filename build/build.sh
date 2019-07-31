cd "$(dirname $0)"

FLAGS="-Wall -pedantic -lm"


# libs
SOURCES=""
SOURCES="${SOURCES} ../src/icd_2D.c"
SOURCES="${SOURCES} ../src/initialize_2D.c"
SOURCES="${SOURCES} ../src/recon_2D.c"
SOURCES="${SOURCES} ../src/A_comp_2D.c"
SOURCES="${SOURCES} ../src/initialize_3D.c"
SOURCES="${SOURCES} ../src/icd_3D.c"
SOURCES="${SOURCES} ../src/recon_3D.c"
SOURCES="${SOURCES} ../src/A_comp_3D.c"
SOURCES="${SOURCES} ../src/allocate.c"
SOURCES="${SOURCES} ../src/MBIRModularUtils_2D.c"
SOURCES="${SOURCES} ../src/MBIRModularUtils_3D.c"

# mains
SOURCES="${SOURCES} ../src/mbir_2D.c"
SOURCES="${SOURCES} ../src/Gen_SysMatrix_2D.c"
SOURCES="${SOURCES} ../src/mbir_3D.c"
SOURCES="${SOURCES} ../src/Gen_SysMatrix_3D.c"

echo ${SOURCES}

# compile
gcc $FLAGS -c $SOURCES


UTILS="allocate.o MBIRModularUtils_2D.o MBIRModularUtils_3D.o"

gcc $FLAGS $UTILS A_comp_2D.o Gen_SysMatrix_2D.o                   -o Gen_SysMatrix_2D.bin
gcc $FLAGS $UTILS icd_2D.o initialize_2D.o recon_2D.o mbir_2D.o    -o mbir_2D.bin
gcc $FLAGS $UTILS A_comp_3D.o Gen_SysMatrix_3D.o                   -o Gen_SysMatrix_3D.bin
gcc $FLAGS $UTILS icd_3D.o initialize_3D.o recon_3D.o mbir_3D.o    -o mbir_3D.bin

rm *.o
mv *.bin ../bin



