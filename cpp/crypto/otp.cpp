#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "otp.h"

const int PACKET_DATA_SIZE = 32;
const int PACKET_CHECKSUM_SIZE = PACKET_DATA_SIZE;
const int PACKET_LENGTH = PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH;

OTP::OTP(int filedescriptor) {
	fd = filedescriptor;
}

OTP::OTP(std::String padFileName) {
	
}

ssize_t OTP::send(const void *buf, size_t len, int flags) {
	
}

ssize_t OTP::recv(void *buf, size_t len, int flags) {
	
}
char* OTP::encrypt(char* enc, const char* msg, const char* key, unsigned long long size) {
	
}
char* OTP::decrypt(char* msg, const char* enc, const char* key, unsigned long long size) {
	
}
char* OTP::mac(char* mac, const char* msg, const char* key, unsigned long long size) {
	
}
char* OTP::verify(bool valid, const char* msg, const char* mac, const char* key, unsigned long long size) {
	
}

char* OTP::macEncrypt(char* digest, const char* msg, const char* key, unsigned long long size) {
	
}

char* OTP::decryptVerify(char* msg, const char* digest, const char* key, unsigned long long size) {
	
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
	for(int i = 0; i < sizel; i++) {
		output[i] = left[i];
	}
	for(int i = 0; i < sizer; i++) {
		output[sizel + i] = right[i];
	}
	return output;
}

char* splitCharArray(char* left, char* right, const char* charArray, unsigned long long splitPoint, unsigned long long size) {
	for(int i = 0; i < splitPoint; i++) {
		left[i] = charArray[i];
	}
	for(int i = splitPoint; i < size; i++) {
		right[i - splitPoint] = charArray[i];
	}
	return left;
}

char* getRandomData(char* output, unsigned long long size) {
	int fd = open("/dev/urandom", O_RDONLY);
	read(fd, output, size);
	return output;
}

char* getEndOfFile(char* output, int fd, unsigned long long size) {
	// off_t lseek(int fd, off_t offset, int whence);
	lseek(fd, -size, SEEK_END);
	
	// ssize_t read(int fd, void *buf, size_t count);
	char* buffer = char[size];
	
	ssize_t dataRead = read(fd, buffer, size);
	
	for(unsigned long long i = 0; i < size; i++) {
		output[i] = buffer[size - i];
	}
	return output;
}

