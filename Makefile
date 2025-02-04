CC=g++
CFLAGS=-std=c++17
LIBPATH=./lib

all: lib dtree main
	$(CC) $(CFLAGS) -o tree main.o DTree.o -L$(LIBPATH) $(LIBPATH)/DHelper.a

lib: DHelper.a

DHelper.a: lib/DHelper.cpp
	cd $(LIBPATH) && $(CC) $(CFLAGS) -c DHelper.cpp && \
		ar rvs DHelper.a DHelper.o

dtree:
	$(CC) $(CFLAGS) -I$(LIBPATH) -c DTree.cpp

main:
	$(CC) $(CFLAGS) -I$(LIBPATH) -c main.cpp

debug: CFLAGS += -g
debug: all

clean:
	rm -f $(LIBPATH)/*.a $(LIBPATH)/*.o *.o tree