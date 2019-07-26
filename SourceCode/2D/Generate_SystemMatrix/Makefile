CC = gcc
CFLAGS = -Wall -pedantic
UtilsDir = ../../../Utilities

all: ct clean

clean:
	rm *.o $(UtilsDir)/MemoryAllocation/*.o $(UtilsDir)/2D/*.o

OBJ = A_comp.o $(UtilsDir)/MemoryAllocation/allocate.o $(UtilsDir)/2D/MBIRModularUtils_2D.o 

ct: Gen_SysMatrix.o $(OBJ)
	$(CC) $(CFLAGS) -o GenerateSystemMatrix2D Gen_SysMatrix.o $(OBJ) -lm


