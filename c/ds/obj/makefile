#Make File Sheet Exercise 1

CC = gcc

CFLAGS = -ansi -pedantic-errors -Wall -Wextra -I$(HEADERS_LOCATION) -I$(OBJECTS_LOCATION)
CFLAGS_FPIC = $(CFLAGS) -fPIC
DEBUG_FLAGS = -g -c
RELEASE_FLAGS = -DNDEBUG -03
LFLAGS = -L. -L$(OBJECTS_LOCATION) -L$(HEADERS_LOCATION) -Wl,-rpath=. -Wl,-rpath=$(HEADERS_LOCATION) -Wl,-rpath=$(OBJECTS_LOCATION)

VPATH = $(OBJECTS_LOCATION) $(HEADERS_LOCATION)
HEADERS_LOCATION = ../../include
OBJECTS_LOCATION = ../../obj


#******************************************************************************
install: libds.so
	cp *.so ../lib

libds.so : copy
	$(CC) $(CFLAGS_FPIC) *.o -shared -o libds.so
# **********************Phony**************************************************	
.PHONY: clean cgdb gdb run vlgfull vlgrun copy

copy:
	cp -u ../source/**/*.o ./
	rm *test*
	
	 
clean:
	rm -f *.so 


