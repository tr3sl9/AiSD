CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = 
LDLIBS = 

.PHONY: all clean

all: table_manager

table_manager: main.o table.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

main.o: main.c table.h
	$(CC) $(CFLAGS) -c $<

table.o: table.c table.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o table_manager
