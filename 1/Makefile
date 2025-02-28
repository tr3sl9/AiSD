CFLAGS=-g -Wall -Wextra
CC=gcc
OBJ=obj
TARGET=a

SOURCES=$(wildcard *.c)
OBJECTS=$(patsubst %.c,$(OBJ)/%.o,$(SOURCES))

clean :
	rm -fr $(OBJ)
	rm -frv $(TARGET)

$(TARGET) : $(OBJECTS)
	mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJ)/%.o : %.c
	mkdir -p $(OBJ)
	$(CC) -o $@ $< -c $(CFLAGS)
