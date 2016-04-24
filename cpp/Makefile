.PHONY: all install compile keygen run clean

all: install compile keygen 

all-no-install: compile keygen 

aes:
	g++ test.cpp -o bin/aes -std=c++0x -pthread -lcryptopp

install:
	sudo apt-get install libcrypto++-utils
	sudo apt-get install libcrypto++9
	sudo apt-get install libcrypto++9-dbg
	sudo apt-get install libcrypto++-dev
	sudo apt-get install libcrypto++-doc
	cd lib/cryptopp; make; make test; make static dynamic cryptest.exe; make libcryptopp.a libcryptopp.so cryptest.exe; sudo make install;

compile:
	mkdir -p bin
	g++ users/alice.cpp lib/utils.cpp -Wall -lcryptopp -o bin/alice.o -pthread
	g++ users/bob.cpp lib/utils.cpp -Wall -lcryptopp -o bin/bob.o -pthread
	# g++ users/alice.cpp lib/utils.cpp channel/channel.cpp channel/channel_wrapper.cpp -Wall -lcryptopp -o bin/alice.o 
	# g++ users/bob.cpp lib/utils.cpp channel/channel.cpp channel/channel_wrapper.cpp -Wall -lcryptopp -o bin/bob.o 

keygen:
	g++ lib/keygen.cpp -Wall -Wextra -lcryptopp -o bin/keygen.o -pthread
	./bin/keygen.o

clean:
	rm -r bin/*
	rm -f *.key
	rm -f *.pub
	rm -f *.o
