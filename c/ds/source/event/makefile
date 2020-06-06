IDIR =../../include
CC= gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra -g -I $(IDIR)
TARGET = event

ODIR= ../../obj

_OBJ = event_test.o event.o uid.o tests.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

event : $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ 
