CC = gcc
CFLAGS = -Wall -pedantic

all: ct clean

clean:
	rm *.o ../../*.o

OBJ = A_comp_2D.o ../../allocate.o ../../MBIRModularUtils_2D.o 

ct: Gen_SysMatrix_2D.o $(OBJ)
	$(CC) $(CFLAGS) -o GenerateSystemMatrix2D Gen_SysMatrix_2D.o $(OBJ) -lm


