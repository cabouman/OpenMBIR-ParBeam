CC=gcc
FLAGS=-Wall -pedantic -lm

src=../src
bin=../bin

UTILS=$(src)/allocate.o $(src)/MBIRModularUtils_2D.o $(src)/MBIRModularUtils_3D.o
SRC_Gen_SysMatrix_2D = $(src)/A_comp_2D.o $(src)/Gen_SysMatrix_2D.o
SRC_Gen_SysMatrix_3D = $(src)/A_comp_3D.o $(src)/Gen_SysMatrix_3D.o 
SRC_MBIR_2D = $(src)/icd_2D.o $(src)/initialize_2D.o $(src)/recon_2D.o $(src)/mbir_2D.o
SRC_MBIR_3D = $(src)/icd_3D.o $(src)/initialize_3D.o $(src)/recon_3D.o $(src)/mbir_3D.o


# --- meta labels -------------------------------------------------

all: main
	rm -f $(src)/*.o

main: utils Gen_SysMatrix_2D mbir_2D Gen_SysMatrix_3D mbir_3D

clean:
	rm -f $(src)/*.o $(bin)/*

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
	



