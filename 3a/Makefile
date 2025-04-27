CFLAGS=-g -Wall -Wextra
CC=gcc
OBJ=obj
TARGET=table

SOURCES=$(wildcard *.c)
OBJECTS=$(patsubst %.c,$(OBJ)/%.o,$(SOURCES))

$(TARGET) : $(OBJECTS)
	mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(CFLAGS)

clean :
	rm -fr $(OBJ)
	rm -frv $(TARGET)

$(OBJ)/%.o : %.c
	mkdir -p $(OBJ)
	$(CC) -o $@ $< -c $(CFLAGS)
