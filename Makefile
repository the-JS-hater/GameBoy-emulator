GCC=clang
FLAGS=-Werror -Wall -O3 -march=native
LIBS=-lraylib -lm

OUT=out
BIN=gameboy
SRCS=$(wildcard src/*.c)
HDR=$(wildcard include/*.h)

build: 
	$(GCC) $(FLAGS) $(SRCS) $(HDRS) $(LIBS) -o $(OUT)/$(BIN)


