PROJECT = BST
SO_NAMES = BTNode 

CC = gcc
CFLAGS = -Wall -Werror -fPIC
DEBUG_FLAGS = -g 
RELEASE_FLAGS = -DNDEBUG 

SO_DIR := ../../lib
IDIR :=  ../../include

SO_LINK = -L$(SO_DIR) -Wl,-rpath=$(SO_DIR) $(patsubst %,-l$(MODE)%,$(SO_NAMES))
MODE =

.PHONY: all default clean install test $(SO_NAMES) debug_mode release compile debug debug_mode release_mode

all: make_debug release

default: release

compile: lib$(MODE)$(PROJECT).so $(MODE)$(PROJECT)_test.out install

make_debug:
	$(MAKE) debug
debug: debug_mode compile
debug_mode: 
	$(eval CFLAGS += $(DEBUG_FLAGS))
	$(eval MODE = debug_)
	
release: release_mode compile
release_mode: 
	$(eval CFLAGS += $(RELEASE_FLAGS))

lib$(MODE)$(PROJECT).so: $(MODE)$(PROJECT).o $(SO_NAMES)
	$(CC) $(CFLAGS) -shared $(MODE)$< $(SO_LINK) -o lib$(MODE)$(PROJECT).so

$(SO_NAMES): 
	$(MAKE) -C ../$@

$(MODE)%.o: %.c $(PROJECT).h 
	$(CC) $(CFLAGS) -c  $< -o $(MODE)$@

$(MODE)$(PROJECT)_test.out: $(MODE)$(PROJECT)_test.o lib$(MODE)$(PROJECT).so 
	$(CC) $(CFLAGS) $(MODE)$< ../tests/tests.c -L. -Wl,-rpath=. -l$(MODE)$(PROJECT) -o $(MODE)$@

test: 
	./$(MODE)$(PROJECT)_test.out

clean: 
	rm -f *.o
	rm -f *.so
	rm -f *.out
	rm -f *core*	
	
install:
	cp *.so $(SO_DIR)
