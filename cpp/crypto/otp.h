#pragma once
#include <unistd.h>

#include "secureChannel.h"

class OTP: public SecureChannel {
	private:
		int file;
		int socket;
		int size;
	public:
		OTP(int filefd, int socketfd, int size);
		OTP(const char* padFileName, int socketfd, int size);
		ssize_t csend(const void *buf, int flags);
		ssize_t crecv(void *buf, int flags);
		char* encrypt(char* enc, const char* msg);
		char* encrypt(char* enc, const char* msg, const char* key, unsigned long long size);
		char* decrypt(char* msg, const char* enc);
		char* decrypt(char* msg, const char* enc, const char* key, unsigned long long size);
		char* mac(char* mac, const char* msg);
		char* mac(char* mac, const char* msg, const char* key, unsigned long long size);
		bool verify(const char* msg, const char* mac);
		bool verify(const char* msg, const char* mac, const char* key, unsigned long long size);
		char* macEncrypt(char* enc, const char* msg);
		char* macEncrypt(char* enc, const char* msg, const char* key, unsigned long long size);
		char* decryptVerify(char* msg, const char* enc);
		char* decryptVerify(char* msg, const char* enc, const char* key, unsigned long long size);
};

// Does binary xor between 2 strings
char* xorCharArray(char* output, const char* first, const char* second, unsigned long long size);

// Does binary and between 2 strings
char* andCharArray(char* output, const char* first, const char* second, unsigned long long size);

// Does binary or between 2 strings
char* orCharArray(char* output, const char* first, const char* second, unsigned long long size);

// Concatinates the left char* to the right one
char* concatCharArray(char* output, const char* left, const char* right, unsigned long long sizel, unsigned long long sizer);

char* copyCharArray(char* output, const char* charArray, unsigned long long size);

// Splits a charArray into two, left = charArray[0:splitPoint], right = charArray[splitPoint:size]
char* splitCharArray(char* left, char* right, const char* charArray, unsigned long long splitPoint, unsigned long long size);

bool equalCharArray(const char* charArray1, const char* charArray2, unsigned long long size);

char* getBytes(char* charArray, unsigned long long size);

char* getRandomData(char* output, unsigned long long size);

char* getEndOfFile(char* output, int fd, unsigned long long size, bool truncate);
	
	
