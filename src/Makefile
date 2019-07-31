CC=gcc
FLAGS=-Wall -pedantic -lm


bin=../bin

UTILS=allocate.o MBIRModularUtils_2D.o MBIRModularUtils_3D.o
SRC_Gen_SysMatrix_2D = A_comp_2D.o Gen_SysMatrix_2D.o
SRC_Gen_SysMatrix_3D = A_comp_3D.o Gen_SysMatrix_3D.o 
SRC_MBIR_2D = icd_2D.o initialize_2D.o recon_2D.o mbir_2D.o
SRC_MBIR_3D = icd_3D.o initialize_3D.o recon_3D.o mbir_3D.o


# --- meta labels -------------------------------------------------

all: main
	rm -f *.o

main: utils Gen_SysMatrix_2D mbir_2D Gen_SysMatrix_3D mbir_3D

clean:
	rm -f *.o $(bin)/*

# --- linking -------------------------------------------------

Gen_SysMatrix_2D:  utils
	$(CC) $(FLAGS) $(UTILS) $(SRC_Gen_SysMatrix_2D) -o $(bin)/Gen_SysMatrix_2D

Gen_SysMatrix_3D:  utils
	$(CC) $(FLAGS) $(UTILS) $(SRC_Gen_SysMatrix_3D) -o $(bin)/Gen_SysMatrix_3D

mbir_2D:  utils
	$(CC) $(FLAGS) $(UTILS) $(SRC_MBIR_2D) -o $(bin)/mbir_2D

mbir_3D:  utils
	$(CC) $(FLAGS) $(UTILS) $(SRC_MBIR_3D) -o $(bin)/mbir_3D

# --- compiling -------------------------------------------------

utils: $(UTILS) $(SRC_Gen_SysMatrix_2D) $(SRC_MBIR_2D) $(SRC_Gen_SysMatrix_3D) $(SRC_MBIR_3D)
	




