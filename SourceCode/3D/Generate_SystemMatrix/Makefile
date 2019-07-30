CC = gcc
CFLAGS = -Wall -pedantic
UtilsDir = ../../../Utilities

all: ct clean

clean:
	rm *.o $(UtilsDir)/MemoryAllocation/*.o $(UtilsDir)/3D/*.o $(UtilsDir)/2D/*.o

OBJ = A_comp_3D.o $(UtilsDir)/MemoryAllocation/allocate.o $(UtilsDir)/3D/MBIRModularUtils_3D.o $(UtilsDir)/2D/MBIRModularUtils_2D.o 

ct: Gen_SysMatrix.o $(OBJ)
	$(CC) $(CFLAGS) -o GenerateSystemMatrix3DParallel Gen_SysMatrix.o $(OBJ) -lm


