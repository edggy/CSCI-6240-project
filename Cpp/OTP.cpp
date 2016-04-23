#include "otp.h"

const int PACKET_DATA_SIZE = 32;
const int PACKET_CHECKSUM_SIZE = PACKET_DATA_SIZE;
const int PACKET_LENGTH = PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH;

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
}

