PROG_TYPE ?= vector
LIBRARY_TYPE ?= static

SRC_DIR = .
OBJ_DIR = obj
INCLUDE_DIR = .
LIB_NAME = stack_library

STACK_VECTOR_SRC = stack_library_vector.c
STACK_LIST_SRC = stack_library_list.c

ifeq ($(PROG_TYPE), vector)
    STACK_SRC = $(STACK_VECTOR_SRC)
    STACK_OBJ = $(OBJ_DIR)/stack_library_vector.o
else ifeq ($(PROG_TYPE), list)
    STACK_SRC = $(STACK_LIST_SRC)
    STACK_OBJ = $(OBJ_DIR)/stack_library_list.o
else
    $(error Неверная реализация. Используйте 'vector' или 'list')
endif

ifeq ($(LIBRARY_TYPE), static)
    LIB_TARGET = $(OBJ_DIR)/lib$(LIB_NAME).a
    BUILD_FLAGS = -c
    LINK_FLAGS = 
else ifeq ($(LIBRARY_TYPE), dynamic)
    LIB_TARGET = $(OBJ_DIR)/lib$(LIB_NAME).so
    BUILD_FLAGS = -fPIC -c
    LINK_FLAGS = -Wl,-rpath,'$$ORIGIN/$(OBJ_DIR)'
else
    $(error Неверный тип библиотеки. Используйте 'static' или 'dynamic')
endif

CC = gcc
CFLAGS = -g -Wall -Wextra -Wpedantic -I$(INCLUDE_DIR)
LDFLAGS = -L$(OBJ_DIR)
LDLIBS = -l$(LIB_NAME)

all: lib game_of_life

lib: $(LIB_TARGET)

$(LIB_TARGET): $(STACK_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(BUILD_FLAGS) $< -o $(STACK_OBJ)
ifeq ($(LIBRARY_TYPE), static)
	ar rcs $@ $(STACK_OBJ)
else
	$(CC) -shared -o $@ $(STACK_OBJ)
endif

game_of_life: $(OBJ_DIR)/game_of_life.o $(OBJ_DIR)/main.o $(LIB_TARGET)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS) $(LINK_FLAGS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) game_of_life

.PHONY: all lib game_of_life clean
