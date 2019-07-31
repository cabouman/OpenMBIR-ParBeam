CC = gcc
CFLAGS = -Wall -pedantic
SRCDir = ../../../src

all: ct clean

clean:
	 rm *.o $(SRCDir)/*.o $(SRCDir)/*.o

OBJ = sino_and_weights.o $(SRCDir)/allocate.o $(SRCDir)/MBIRModularUtils_2D.o $(SRCDir)/MBIRModularUtils_3D.o

ct: Gen_Sinogram_and_Weights.o $(OBJ)
	$(CC) $(CFLAGS)  -o Gen_Projections_and_Weights Gen_Sinogram_and_Weights.o $(OBJ) -lm


