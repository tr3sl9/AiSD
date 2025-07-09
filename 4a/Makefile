CFLAGS=-g -Wall -Wextra -Werror -Wpedantic -Wno-implicit-fallthrough
LDFLAGS=-lreadline `pkg-config libgvc libcgraph --libs`
CC=gcc

OBJ_CLIENT=obj/client
OBJ_LIB=obj/lib
OBJ_TIMING=obj/timing
SRC_CLIENT=src/client
SRC_LIB=src/lib
SRC_TIMING=src/timing

LIB_TARGET=libbst.a
BST_TARGET=bst
BENCHMARK_TARGET=bst_benchmark

SOURCES_CLIENT=$(wildcard $(SRC_CLIENT)/*.c)
OBJECTS_CLIENT=$(patsubst $(SRC_CLIENT)/%.c,$(OBJ_CLIENT)/%.o,$(SOURCES_CLIENT))

SOURCES_LIB=$(wildcard $(SRC_LIB)/*.c)
OBJECTS_LIB=$(patsubst $(SRC_LIB)/%.c,$(OBJ_LIB)/%.o,$(SOURCES_LIB))

SOURCES_TIMING=$(wildcard $(SRC_TIMING)/*.c)
OBJECTS_TIMING=$(patsubst $(SRC_TIMING)/%.c,$(OBJ_TIMING)/%.o,$(SOURCES_TIMING))

QUEUE_OBJ=$(OBJ_CLIENT)/queue_lib.o
STACK_OBJ=$(OBJ_CLIENT)/stack_lib.o
INFO_OBJ=$(OBJ_CLIENT)/info_struct.o

all: $(BST_TARGET) $(BENCHMARK_TARGET)

$(BST_TARGET): $(OBJECTS_CLIENT) $(LIB_TARGET)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

$(BENCHMARK_TARGET): $(OBJ_TIMING)/bst_benchmark.o $(LIB_TARGET) $(QUEUE_OBJ) $(STACK_OBJ) $(INFO_OBJ)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

$(LIB_TARGET): $(OBJECTS_LIB)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

$(OBJ_CLIENT)/%.o: $(SRC_CLIENT)/%.c
	@mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS)

$(OBJ_LIB)/%.o: $(SRC_LIB)/%.c
	@mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS)

$(OBJ_TIMING)/%.o: $(SRC_TIMING)/%.c
	@mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS) -I$(SRC_CLIENT) -I$(SRC_LIB)

clean:
	rm -fr obj
	rm -fv $(BST_TARGET)
	rm -fv $(BENCHMARK_TARGET)
	rm -fv $(LIB_TARGET)

.PHONY: all clean
