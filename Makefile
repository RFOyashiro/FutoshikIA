CC = gcc

LIBS = -lm 
#ajouter -o3 a la fin
CCFLAGS = -Wall #-ggdb -g3

OBJ = backTrack.o futoshiki.o

all: futoshiki

futoshiki: futoshiki.c $(OBJ)
	$(CC) $(CCFLAGS) -o futoshiki.exe $(OBJ)

.PHONY : clean

clean:
	- rm -f $(OBJ)
	- rm -f futoshiki
