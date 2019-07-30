CC = gcc
CFLAGS = -Wall -pedantic

all: ct clean

clean:
	rm *.o ../../*.o

OBJ = icd_2D.o initialize_2D.o recon_2D.o ../../allocate.o ../../MBIRModularUtils_2D.o

ct: mbir_2D.o $(OBJ)
	$(CC) $(CFLAGS) -o MBIR_2DCT mbir_2D.o $(OBJ) -lm


