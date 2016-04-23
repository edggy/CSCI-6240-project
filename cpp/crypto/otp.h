#pragma once
#include <unistd.h>

#include "secureChannel.h"

class OTP: public SecureChannel {
	private:
		int fd;
	public:
		virtual ssize_t send(const void *buf, size_t len, int flags);
		virtual ssize_t recv(void *buf, size_t len, int flags);
		virtual char* encrypt(char* enc, const char* msg, const char* key, unsigned long long size);
		virtual char* decrypt(char* msg, const char* enc, const char* key, unsigned long long size);
		virtual char* mac(char* mac, const char* msg, const char* key, unsigned long long size);
		virtual char* verify(bool valid, const char* msg, const char* mac, const char* key, unsigned long long size);
		virtual char* macEncrypt(char* digest, const char* msg, const char* key, unsigned long long size);
		virtual char* decryptVerify(char* msg, const char* digest, const char* key, unsigned long long size);
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

char* getBytes(char* charArray, unsigned long long size);

char* getRandomData(char* output, unsigned long long size);

char* getEndOfFile(char* output, int fd, unsigned long long size);
	
	