CC = gcc
CFLAGS = -Wall -pedantic
UtilsDir = ../../../Utilities

all: ct clean

clean:
	rm *.o $(UtilsDir)/MemoryAllocation/*.o $(UtilsDir)/2D/*.o

OBJ = icd_2D.o initialize_2D.o recon_2D.o $(UtilsDir)/MemoryAllocation/allocate.o $(UtilsDir)/2D/MBIRModularUtils_2D.o

ct: mbir_2D.o $(OBJ)
	$(CC) $(CFLAGS) -o MBIR_2DCT mbir_2D.o $(OBJ) -lm


