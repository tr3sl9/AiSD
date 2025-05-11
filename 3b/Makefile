CFLAGS=-g -Wall -Wextra -Werror -lreadline -Wno-implicit-fallthrough
CC=gcc
OBJ=obj
LIB_DIR=lib
LIB_TARGET=$(LIB_DIR)/libtable.a
TARGET=table

SOURCES=$(filter-out main.c, $(wildcard *.c))
OBJECTS=$(patsubst %.c,$(OBJ)/%.o,$(SOURCES))

$(TARGET) : $(OBJ)/main.o $(LIB_TARGET)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(CFLAGS)

$(LIB_TARGET) : $(OBJECTS)
	@mkdir -p $(LIB_DIR)
	ar rcs $@ $^

clean :
	rm -fr $(OBJ)
	rm -frv $(TARGET)
	rm -fr $(LIB_TARGET)
	rm -fr $(LIB_DIR)

$(OBJ)/%.o : %.c
	@mkdir -p $(OBJ)
	$(CC) -o $@ $< -c $(CFLAGS)

.PHONY: clean
