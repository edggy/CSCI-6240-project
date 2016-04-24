
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

#include "otp.h"

#include <iostream>
#include <assert.h>

OTP::OTP(int filefd, int socketfd, int size) {
	this->file = filefd;
	this->socket = socketfd;
	this->size = size;
}

OTP::OTP(const char* padFileName, int socketfd, int size) {
	this->file = open(padFileName, 0);
	this->socket = socketfd;
	this->size = size;
}

ssize_t OTP::csend(const void *buf, int flags) {
	char* enc = new char[this->size * 2];
	enc = this->macEncrypt(enc, (char*) buf);
	ssize_t ret = send(this->socket, enc, this->size * 2, flags);
	
	delete[] enc;
	
	return ret;
}

ssize_t OTP::crecv(void *buf, int flags) {
	char* enc = new char[this->size * 2];
	ssize_t amount = recv(this->socket, enc, this->size * 2, flags);
	buf = (void*) this->decryptVerify((char*) buf, enc);
	
	delete[] enc;
	
	return amount;
}
char* OTP::encrypt(char* enc, const char* msg) {
	char* key = new char[this->size];
	key = getEndOfFile(key, this->file, this->size, true);
	
	enc = xorCharArray(enc, msg, key, this->size);
	delete[] key;
	
	return enc;
}

char* OTP::encrypt(char* enc, const char* msg, const char* key, unsigned long long size) {
	return xorCharArray(enc, msg, key, size);
}
char* OTP::decrypt(char* msg, const char* enc) {
	char* key = new char[this->size];
	key = getEndOfFile(key, this->file, this->size, true);
	
	msg = xorCharArray(msg, enc, key, this->size);
	delete[] key;
	
	return msg;
}
char* OTP::decrypt(char* msg, const char* enc, const char* key, unsigned long long size) {
	return xorCharArray(msg, enc, key, size); 
}
char* OTP::mac(char* mac, const char* msg) {
	char* key = new char[this->size * 2];
	key = getEndOfFile(key, this->file, this->size * 2, true);
	mac = this->mac(mac, msg, key, this->size);
	delete[] key;
	
	return mac;
}
char* OTP::mac(char* mac, const char* msg, const char* key, unsigned long long size) {
	// Size is the size of the msg, key must be 2*size
	const char* key1 = key;
	const char* key2 = key + size;
	// key1 = splitCharArray(key1, key2, key, size, size * 2);
	char* inter = new char[size];
	inter = andCharArray(inter, msg, key1, size);
	mac = orCharArray(mac, inter, key2, size);
	//delete[] key1;
	//delete[] key2;
	delete[] inter;
	return mac;
}
bool OTP::verify(const char* msg, const char* mac) {
	char* key = new char[this->size * 2];
	key = getEndOfFile(key, this->file, this->size * 2, false);
	bool result = this->verify(msg, mac, key, this->size);
	if(result) {
		key = getEndOfFile(key, this->file, this->size * 2, true);
	}
	delete[] key;
	
	return result;
}
bool OTP::verify(const char* msg, const char* mac, const char* key, unsigned long long size) {
	// Size is the size of the msg, key must be 2*size
	char* calcMac = new char[size];
	calcMac = this->mac(calcMac, msg, key, size);
	bool result = equalCharArray(mac, calcMac, size);
	delete[] calcMac;
	
	return result;
}

char* OTP::macEncrypt(char* enc, const char* msg) {
	char* key = new char[this->size * 4];
	
	key = getEndOfFile(key, this->file, this->size * 4, true);
	
	return this->macEncrypt(enc, msg, key, this->size);
}
char* OTP::macEncrypt(char* enc, const char* msg, const char* key, unsigned long long size) {
	// Size is the size of the msg, key must be 4*size
	
	const char* key1 = key;
	const char* key2 = key + size * 2;

	char* digest = new char[size * 2];

	digest = copyCharArray(digest, msg, size);
	
	char* mac = digest + size;

	mac = this->mac(mac, msg, key1, size);

	enc = this->encrypt(enc, digest, key2, size * 2);
	delete[] digest;
	
	return enc;
}
char* OTP::decryptVerify(char* msg, const char* enc) {
	char* key = new char[this->size * 4];
	key = getEndOfFile(key, this->file, this->size * 4, false);
	
	msg = this->decryptVerify(msg, enc, key, this->size);
	
	if(msg != NULL) {
		// Delete the bytes used only if verification is successful
		key = getEndOfFile(key, this->file, this->size * 4, true);
	}
	delete[] key;
	return msg;
	
}
char* OTP::decryptVerify(char* msg, const char* enc, const char* key, unsigned long long size) {
	// Size is the size of the msg, key must be 4*size
	
	const char* key1 = key;
	const char* key2 = key + size * 2;
	
	char* digest = new char[size * 2];

	digest = this->decrypt(digest, enc, key2, size * 2);

	msg = copyCharArray(msg, digest, size);

	char* mac = digest + size;

	bool valid = this->verify(msg, mac, key1, size);
	
	if(!valid) {
		msg = NULL;
	}
	
	delete[] digest;
	
	return msg;
}

char* xorCharArray(char* output, const char* first, const char* second, unsigned long long size) {
	for(unsigned long long i = 0; i < size; i++) {
		output[i] = first[i] ^ second[i];
	}
	return output;
}

char* andCharArray(char* output, const char* first, const char* second, unsigned long long size) {
	for(unsigned long long i = 0; i < size; i++) {
		output[i] = first[i] & second[i];
	}
	return output;
}

char* orCharArray(char* output, const char* first, const char* second, unsigned long long size) {
	for(unsigned long long i = 0; i < size; i++) {
		output[i] = first[i] | second[i];
	}
	return output;
}

char* concatCharArray(char* output, const char* left, const char* right, unsigned long long sizel, unsigned long long sizer) {
	for(unsigned long long i = 0; i < sizel; i++) {
		output[i] = left[i];
	}
	for(unsigned long long i = 0; i < sizer; i++) {
		output[sizel + i] = right[i];
	}
	return output;
}

char* copyCharArray(char* output, const char* charArray, unsigned long long size) {
	for(unsigned long long i = 0; i < size; i++) {
		output[i] = charArray[i];
	}
	return output;
}


char* splitCharArray(char* left, char* right, const char* charArray, unsigned long long splitPoint, unsigned long long size) {
	for(unsigned long long i = 0; i < splitPoint; i++) {
		left[i] = charArray[i];
	}
	for(unsigned long long i = splitPoint; i < size; i++) {
		right[i - splitPoint] = charArray[i];
	}
	return left;
}

bool equalCharArray(const char* charArray1, const char* charArray2, unsigned long long size) {
	for(unsigned long long i = 0; i < size; i++) {
		if(charArray1[i] != charArray2[i]) return false;
	}
	return true;
}

char* getRandomData(char* output, unsigned long long size) {
	int fd = open("/dev/urandom", O_RDONLY);
	read(fd, output, size);
	return output;
}

char* getEndOfFile(char* output, int fd, unsigned long long size, bool truncate) {
	// off_t lseek(int fd, off_t offset, int whence);
	off_t offset = lseek(fd, -size, SEEK_END);
	
	// ssize_t read(int fd, void *buf, size_t count);
	char* buffer = new char[size];
	
	ssize_t dataRead = read(fd, buffer, size);
	
	for(unsigned long long i = 0; i < size; i++) {
		output[i] = buffer[size - i];
	}
	
	if(truncate) {
		ftruncate(fd, offset);
	}
	return output;
}

