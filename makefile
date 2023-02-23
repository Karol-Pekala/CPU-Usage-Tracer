CC=gcc
CFLAGS=-Wall -Wextra
LIBS=-lpthread
EXEC=CPU
DEPS=queue.h
OBJ=CPUtracker.c queue.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) $(LIBS) -o$(EXEC) 

