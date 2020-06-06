IDIR =../../include
CC= gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra -g -I $(IDIR)
TARGET = uid

ODIR= ../../obj

_DEPS = uid.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = uid.o uid_test.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

uid : $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ 
