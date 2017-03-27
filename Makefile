
CC = gcc

BASEFLAGS = -Wall #-pthread 
NODEBUG_FLAGS = -dNDEBUG 
DEBUG_FLAGS = -g

LDLIBS = #-lcurses -pthread

OBJS = main.o 

EXE = a3 

debug: CFLAGS = $(BASEFLAGS) $(DEBUG_FLAGS)
debug: $(EXE)

release: CFLAGS = $(BASEFLAGS) $(NODEBUG_FLAGS) 
release: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(EXE) $(LDLIBS)

main.o: main.c 
	$(CC) $(CFLAGS) -c main.c

#console.o: console.c console.h
#	$(CC) $(CFLAGS) -c console.c

#shell.o: shell.c shell.h
#	$(CC) $(CFLAGS) -c shell.c

clean:
	rm -f $(OBJS)
	rm -f *~
#	rm -f $(EXE)

run:
	./$(EXE)
