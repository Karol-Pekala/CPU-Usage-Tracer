CC=gcc
CFLAGS=-Wall -Wextra
LIBS=-lpthread
EXEC=MAINgcc
DEPS=queue.h CPUtracker.h string-queue.h
OBJ=main.c CPUtracker.c queue.c string-queue.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) $(LIBS) -o $(EXEC) 

clang: CC=clang-11 
clang: CFLAGS=-Weverything -Wno-disabled-macro-expansion -Wno-strict-prototypes -Wno-missing-prototypes -Wno-padded
clang: EXEC=MAINclang
clang:
	$(CC) $(OBJ) $(CFLAGS) $(LIBS) -o $(EXEC) 
	
test: OBJ=test.c CPUtracker.c queue.c string-queue.c
test: EXEC=test
test:
	$(CC) $(OBJ) $(CFLAGS) $(LIBS) -o $(EXEC) 

clean:
	rm MAINgcc MAINclang test log
