CC = gcc
CFLAGS = -Wall -pedantic
UtilsDir = ../../../../Utilities

all: ct clean

clean:
	rm *.o $(UtilsDir)/MemoryAllocation/*.o $(UtilsDir)/2D/*.o 

OBJ = sino_and_weights.o $(UtilsDir)/MemoryAllocation/allocate.o $(UtilsDir)/2D/MBIRModularUtils_2D.o

ct: Gen_Sinogram_and_Weights.o $(OBJ)
	$(CC) $(CFLAGS) -o Gen_Projections_and_Weights Gen_Sinogram_and_Weights.o $(OBJ) -lm


