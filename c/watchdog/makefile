CC = gcc
FLAGS = -ansi -pedantic-errors -Wall -Wextra -g
LIBLOC = ../../ds/lib/
LINKFLAG = -lds -L$(LIBLOC) -Wl,-rpath=$(LIBLOC)  

all: 
	$(CC) $(FLAGS) wd.c wd_test.c -lpthread $(LINKFLAG) -o wd

process: 
	$(CC) $(FLAGS) process_wd.c  -lpthread $(LINKFLAG) -o process_wd
	
clean:
	rm -f wd process_wd
	
.PHONY: clean 	
