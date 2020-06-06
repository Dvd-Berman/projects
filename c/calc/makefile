IDIR =../include
CC= gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra -I$(IDIR)
CRFLAGS = -DNDEBUG
CDFLAGS = -g
TARGET = calc

all : clean release

OBJ = calc.o calc_test.o stack.o tests.o
%.o: %.c calc.h
	$(CC) $(CFLAGS) -c $< -o $@

release: add_release_flags $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lm 

add_release_flags:
	$(eval CFLAGS += $(CRFLAGS))

debug: clean add_debug_flags $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lm
	
add_debug_flags:
	$(eval CFLAGS += $(CDFLAGS))
	
.PHONY: clean add_release_flags add_debug_flags

clean:
	rm -f calc.o calc_test.o 
