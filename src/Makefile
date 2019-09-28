CC=gcc
FLAGS=-Wall -pedantic


bin=../bin

UTILS=allocate.o MBIRModularUtils_2D.o MBIRModularUtils_3D.o
SRC_Gen_SysMatrix_3D = A_comp_3D.o Gen_SysMatrix_3D.o 
SRC_MBIR_3D = icd_3D.o initialize_3D.o recon_3D.o mbir_3D.o


# --- meta labels -------------------------------------------------

all: main
	rm -f *.o

main: utils Gen_SysMatrix_3D mbir_3D

clean:
	rm -f *.o $(bin)/*

# --- linking -------------------------------------------------

Gen_SysMatrix_3D:  utils
	$(CC) $(FLAGS) $(UTILS) $(SRC_Gen_SysMatrix_3D) -o $(bin)/Gen_SysMatrix_3D -lm

mbir_3D:  utils
	$(CC) $(FLAGS) $(UTILS) $(SRC_MBIR_3D) -o $(bin)/mbir_3D -lm

# --- compiling -------------------------------------------------

utils: $(UTILS) $(SRC_Gen_SysMatrix_3D) $(SRC_MBIR_3D)
	




