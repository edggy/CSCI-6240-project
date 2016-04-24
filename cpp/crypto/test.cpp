#include <iostream>

#include "otp.h"

int main(int argc, char* argv[]) {
	std::cout << "Starting" << std::endl;
	OTP* pad1 = new OTP("pad1.pad", NULL, 64);
	OTP* pad2 = new OTP("pad2.pad", NULL, 64);
	std::cout << "Created pads" << std::endl;
	char* plaintext = "Now is the time for all good men to come to the aide...\0";
	char* ciphertext = new char[64];
	char* decrypted = new char[64];
	
	std::cout << plaintext << std::endl;
    
    ciphertext = pad1->macEncrypt(ciphertext, plaintext);
	std::cout << ciphertext << std::endl;
    decrypted = pad2->decryptVerify(decrypted, ciphertext);
	std::cout << ciphertext << std::endl;
    
    std::cout << decrypted << std::endl;
}