IDIR =../../include
CC= gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra -I$(IDIR)
CRFLAGS = -DNDEBUG
CDFLAGS = -g
TARGET = hash

all : clean release

OBJ = hash.o  hash_test.o d_list.o tests.o
%.o: %.c hash.h
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
	rm -f hash.o hash_test.o 
