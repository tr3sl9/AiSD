CFLAGS=-g -Wall -Wextra
CC=gcc
OBJ=obj
TARGET=a

clean :
	rm -rv a

$(TARGET) : $(OBJ)/main.o $(OBJ)/testing.o $(OBJ)/row.o $(OBJ)/matrix.o $(OBJ)/individual.0
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJ)/%.o : %.c
	mkdir -p $(OBJ)
	$(CC) -o $@ $< -c $(CFLAGS)
