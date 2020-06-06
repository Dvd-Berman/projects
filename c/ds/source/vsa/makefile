IDIR =../../include
CC= gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra -I$(IDIR)
CRFLAGS = -DNDEBUG
CDFLAGS = -g
TARGET = vsa

all : clean release

OBJ = vsa_node.o vsa.o vsa_test.o tests.o
%.o: %.c vsa.h
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
	rm -f vsa.o vsa_test.o 
