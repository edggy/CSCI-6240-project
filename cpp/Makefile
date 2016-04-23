.PHONY: all install compile keygen run clean

all: install compile run clean

all-no-install: compile run clean

aes:
	g++ test.cpp -o bin/aes -std=c++0x -pthread -lcryptopp

install:
	sudo apt-get update
	sudo apt-get install g++
	sudo apt-get install libcrypto++-utils
	sudo apt-get install libcrypto++9
	sudo apt-get install libcrypto++9-dbg
	sudo apt-get install libcrypto++-dev
	sudo apt-get install libcrypto++-doc

compile:
	mkdir bin

keygen:
	g++ lib/keygen.cpp -Wall -Wextra -lcryptopp -o bin/keygen.o -pthread
	./bin/keygen.o

run: keygen 


clean:
	rm -f bin/*
	rm -f *.key
	rm -f *.pub
	rm -f *.o