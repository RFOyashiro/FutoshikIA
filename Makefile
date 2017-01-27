CC = gcc

LIBS = -lm 
CCFLAGS = -Wall -ggdb

OBJ = 

all: futoshiki

futoshiki: futoshiki.c $(OBJ)
	$(CC) $(CCFLAGS) -o futoshiki.exe futoshiki.c $(OBJ)

#analyseur_lexical.o: analyseur_lexical.c
#	$(CC) $(CCFLAGS) -c analyseur_lexical.c

.PHONY : clean

clean:
	- rm -f $(OBJ)
- rm -f futoshiki
