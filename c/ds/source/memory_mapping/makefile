IDIR =../../include
CC= gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra -no-pie -g -I $(IDIR)
TARGET = memory_mapping

ODIR= ../../obj

_DEPS = pq.h tests.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = pq.o sorted_list.o d_list.o tests.o memory_mapping.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)

memory_mapping : $(OBJ) 
	$(CC) -o $@ $^ $(CFLAGS) -ldl

.PHONY: clean

clean:
	rm -f memory_mapping.o
