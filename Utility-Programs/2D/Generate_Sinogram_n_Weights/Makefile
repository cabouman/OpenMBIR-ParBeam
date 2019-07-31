/CC = gc2
CFLAGS = -Wall -pedantic
SRCDir = ../../../src

all: ct 

clean:
	rm *.o $(SRCDir)/*.o $(SRCDir)/2D/*.o 

OBJ = sino_and_weights.o $(SRCDir)/allocate.o $(SRCDir)/MBIRModularUtils_2D.o 

ct: Gen_Sinogram_and_Weights.o $(OBJ)
	$(CC) $(CFLAGS) -o Gen_Projections_and_Weights Gen_Sinogram_and_Weights.o $(OBJ) -lm


