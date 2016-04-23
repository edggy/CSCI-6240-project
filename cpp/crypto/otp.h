#pragma once
#include <unistd.h>

#include "secureChannel.h"

class OTP: public SecureChannel {
	private:
		int file;
		int socket;
		int size;
	public:
		ssize_t send(const void *buf, int flags);
		ssize_t recv(void *buf, int flags);
		char* encrypt(char* enc, const char* msg);
		char* decrypt(char* msg, const char* enc);
		char* mac(char* mac, const char* msg);
		bool verify(const char* msg, const char* mac);
		char* macEncrypt(char* enc, const char* msg);
		char* decryptVerify(char* msg, const char* enc);
};

// Does binary xor between 2 strings
char* xorCharArray(char* output, const char* first, const char* second, unsigned long long size);

// Does binary and between 2 strings
char* andCharArray(char* output, const char* first, const char* second, unsigned long long size);

// Does binary or between 2 strings
char* orCharArray(char* output, const char* first, const char* second, unsigned long long size);

// Concatinates the left char* to the right one
char* concatCharArray(char* output, const char* left, const char* right, unsigned long long sizel, unsigned long long sizer);

// Splits a charArray into two, left = charArray[0:splitPoint], right = charArray[splitPoint:size]
char* splitCharArray(char* left, char* right, const char* charArray, unsigned long long splitPoint, unsigned long long size);

bool equalCharArray(const char* charArray1, const char* charArray2, unsigned long long size);

char* getBytes(char* charArray, unsigned long long size);

char* getRandomData(char* output, unsigned long long size);

char* getEndOfFile(char* output, int fd, unsigned long long size);
	
	