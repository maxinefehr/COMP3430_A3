
CC = gcc

BASEFLAGS = -Wall #-std=c99#-pthread 
NODEBUG_FLAGS = -dNDEBUG 
DEBUG_FLAGS = -g

LDLIBS = #-lcurses -pthread

OBJS = main.o fat32.o

EXE = fat32 

debug: CFLAGS = $(BASEFLAGS) $(DEBUG_FLAGS)
debug: $(EXE)

release: CFLAGS = $(BASEFLAGS) $(NODEBUG_FLAGS) 
release: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(EXE) $(LDLIBS)

main.o: main.c 
	$(CC) $(CFLAGS) -c main.c

fat32.o: fat32.c fat32.h
	$(CC) $(CFLAGS) -c fat32.c

#shell.o: shell.c shell.h
#	$(CC) $(CFLAGS) -c shell.c

clean:
	rm -f $(OBJS)
	rm -f *~
#	rm -f $(EXE)

run:
	./$(EXE)
