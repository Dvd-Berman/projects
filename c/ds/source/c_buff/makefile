
project = c_buff
program = $(project)
file1 = $(project)
file2 = $(project)_test

all: vlgrun

vlgrun: clean compile
	valgrind --leak-check=yes --track-origins=yes ./$(program)

$(program): $(file1).o $(file2).o 

clean:
	rm -f *.o
	rm -rf test
	rm -f core
	rm -f vgcore.*

run: clean $(program)
	./$(program)

cgdb:
	cgdb ./$(program) ./core
	
vlgfull: clean $(program)
	valgrind --leak-check=yes --track-origins=yes --log-file=./"valgrind result.log" --xtree-memory=full --xtree-memory-file="valgrind memmory result.txt" ./$(program)
	
compile: 
	gcc -ansi -pedantic-errors -Wall -Wextra -g $(file1).c $(file2).c 
