IDIR =../../include
CC= gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra -I$(IDIR)
CRFLAGS = -DNDEBUG
CDFLAGS = -g
TARGET = bst_node

all : clean release

OBJ = bst_node.o bst_node_test.o tests.o
%.o: %.c bst_node.h
	$(CC) $(CFLAGS) -c $< -o $@

release: add_release_flags $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

add_release_flags:
	$(eval CFLAGS += $(CRFLAGS))

debug: clean add_debug_flags $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) 
	
add_debug_flags:
	$(eval CFLAGS += $(CDFLAGS))
	
.PHONY: clean add_release_flags add_debug_flags

clean:
	rm -f bst_node.o bst_node_test.o 
