CC = gcc

CFLAGS := -std=c11
CFLAGS := $(CFLAGS) -O3
CFLAGS := $(CFLAGS) -Wall

all: mbir_3D Gen_SysMatrix_3D clean

clean:
	rm *.o

#OBJ = allocate.o MBIRModularUtils.o A_comp_3D.o icd_3D.o initialize_3D.o recon_3D.o 
OBJ = allocate.o MBIRModularUtils.o

mbir_3D: mbir_3D.o icd_3D.o initialize_3D.o recon_3D.o $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ -lm
	mv $@ ../bin

Gen_SysMatrix_3D: Gen_SysMatrix_3D.o A_comp_3D.o $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ -lm
	mv $@ ../bin

#%.o: %.c
#	$(CC) -c $(CFLAGS) $< -o $@
