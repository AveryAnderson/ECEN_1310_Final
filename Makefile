# Variables
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic -g
PROG = miniMouse
SRC = $(PROG).c maze.c mouse.c
HDR = maze.h mouse.h
OBJ = $(SRC:.c=.o)

# Targets
$(PROG): $(OBJ)
$(OBJ): $(HDR)
TAGS: $(SRC) $(HDR)
	etags $^
.PHONY: clean
clean:
	$(RM) $(PROG) $(OBJ)
