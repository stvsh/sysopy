CC = gcc
CFLAGS = -Wall -std=c99
SRC = ./main.c
LIB = ../zad1

all: static shared dynamic

static:
	$(CC) $(CFLAGS) $(SRC) -L$(LIB) -ltable -o zad3_libstatic

shared:
	$(CC) $(CFLAGS) $(SRC) -L$(LIB) -Wl,-rpath=$(LIB) -ltable -o zad3_libshared

dynamic:
	$(CC) $(CFLAGS) -DDYNAMIC $(SRC) -ldl -o zad3_libdynamic

clean:
	rm zad3_* raport3a.txt
	
test:
	./zad3_libstatic 500000 64 d search 420
	./zad3_libshared 500000 64 d search 420
	./zad3_libdynamic 500000 64 d search 420
	
	./zad3_libstatic 500000 64 d swap 200000
	./zad3_libshared 500000 64 d swap 200000
	./zad3_libdynamic 500000 64 d swap 200000
	
	./zad3_libstatic 500000 64 d alt_swap 200000
	./zad3_libshared 500000 64 d alt_swap 200000
	./zad3_libdynamic 500000 64 d alt_swap 200000

	./zad3_libstatic 500000 64 s search 420
	./zad3_libshared 500000 64 s search 420
	./zad3_libdynamic 500000 64 s search 420
	
	./zad3_libstatic 500000 64 s swap 200000
	./zad3_libshared 500000 64 s swap 200000
	./zad3_libdynamic 500000 64 s swap 200000
	
	./zad3_libstatic 500000 64 s alt_swap 200000
	./zad3_libshared 500000 64 s alt_swap 200000
	./zad3_libdynamic 500000 64 s alt_swap 200000
	