# CSCI-6240-project
Optional project for CSCI 6240

gmpy2 is required for the 1-2 OT
python -m pip install gmpy2

To run the CPP version do the following:

Navigate to the Makefile and run

    make all

This will install the CryptoPP library used.

If you already have the library installed just run

	make all-no-install

and this will compile the program and run a quick RSA key generation program.

Then to run open up two terminals and in one run

    cd bin/
    ./alice.o 9999

And in the other run

	cd bin/
	./bob.o 9999

Make sure to run Alice first. If you get a port error try changing the port number (the parameter). 
