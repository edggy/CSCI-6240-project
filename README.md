# CSCI-6240-project
Optional project for CSCI 6240

The actual, functional code is in the cpp folder (C++ code). There is also a Python folder, that was more of a side project of trying to implement the same functionality in Python.




To run the CPP version on Ubuntu or Windows (Cygwin) do the following:

Navigate to the Makefile and run

    make all

This will install the CryptoPP library used and compile everything.

For a Mac, you must install the CryptoPP library separately. The command should be

    brew install libcryptopp
    
Or something of the sort (sorry). 

If you already have the library installed just run

	make all-no-install

and this will compile the program and run a quick RSA key generation program.

Then to run open up two terminals and in one run

    cd bin/
    ./alice.o 9999 <cash>

And in the other run

	cd bin/
	./bob.o 9999 <cash>

Make sure to run Alice first. If you get a port error try changing the port number (the parameter). 



The Python code:
	gmpy2 is required for the 1-2 OT
	python -m pip install gmpy2