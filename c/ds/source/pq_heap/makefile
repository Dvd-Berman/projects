IDIR =../../include
CC= gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra -I$(IDIR)
CRFLAGS = -DNDEBUG
CDFLAGS = -g
TARGET = pq_heap

all : clean release

OBJ = pq_heap.o pq_heap_test.o  heapify.o vector.o tests.o
%.o: %.c pq_heap.h
	$(CC) $(CFLAGS) -c $< -o $@

release: add_release_flags $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lm
	cp *.o ../sched

add_release_flags:
	$(eval CFLAGS += $(CRFLAGS))

debug: clean add_debug_flags $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lm
	cp *.o ../sched
	
add_debug_flags:
	$(eval CFLAGS += $(CDFLAGS))
	
.PHONY: clean add_release_flags add_debug_flags

clean:
	rm -f pq_heap.o pq_heap__test.o 
