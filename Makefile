CC = gcc

LIBS = -lm 
#ajouter -O3 a la fin
CCFLAGS = -Wall -ggdb -g3

OBJ = fcHeuristic.o backTrack.o forwardChecking.o timer.o futoshiki.o

all: futoshiki

futoshiki: futoshiki.c $(OBJ)
	$(CC) $(CCFLAGS) -o futoshiki $(OBJ)
	
%.o: %.c
	$(CC) $(CCFLAGS) -c $<
	
.c.o:
	$(CC) -c $(CFLAGS) $<

.PHONY : clean

clean:
	- rm -f $(OBJ)
	- rm -f futoshiki
