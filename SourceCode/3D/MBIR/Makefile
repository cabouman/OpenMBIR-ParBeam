CC = gcc
CFLAGS = -Wall -pedantic
UtilsDir = ../../../Utilities

all: ct clean

clean:
	rm *.o $(UtilsDir)/MemoryAllocation/*.o $(UtilsDir)/3D/*.o $(UtilsDir)/2D/*.o

OBJ = icd3d.o initialize.o recon3d.o $(UtilsDir)/MemoryAllocation/allocate.o $(UtilsDir)/3D/MBIRModularUtils_3D.o $(UtilsDir)/2D/MBIRModularUtils_2D.o

ct: mbir_ct.o $(OBJ)
	$(CC) $(CFLAGS) -o MBIR_3DCT mbir_ct.o $(OBJ) -lm


