CC = gcc
CFLAGS = -Wall -pedantic
UtilsDir = ../../../Utilities

all: ct clean

clean:
	rm *.o $(UtilsDir)/MemoryAllocation/*.o $(UtilsDir)/2D/*.o

OBJ = icd2d.o initialize.o recon2d.o $(UtilsDir)/MemoryAllocation/allocate.o $(UtilsDir)/2D/MBIRModularUtils_2D.o

ct: mbir_ct.o $(OBJ)
	$(CC) $(CFLAGS) -o MBIR_2DCT mbir_ct.o $(OBJ) -lm


