
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "otp.h"

OTP::OTP(int filefd, int socketfd, int size) {
	this->file = filefd;
	this->socket = socketfd;
	this->size = size;
}

OTP::OTP(char* padFileName, int socketfd, int size) {
	this->file = open(padFileName, 0);
	this->socket = socketfd;
	this->size = size;
}

ssize_t OTP::send(const void *buf, int flags) {
	char* enc = char[this->size * 2];
	enc = this->macEncrypt(enc, (char*) buf);
	return send(this->socket, enc, this->size * 2, flags);
}

ssize_t OTP::recv(void *buf, int flags) {
	char* enc = char[this->size * 2];
	enc = recv(this->socket, enc, this->size * 2, flags);
	return this->decryptVerify(buf, enc);
}
char* OTP::encrypt(char* enc, const char* msg) {
	char* key = char[this->size]
	key = getEndOfFile(key, this->file, this->size, true);
	
	return xorCharArray(enc, msg, key, this->size);
}

char* OTP:encrypt(char* enc, const char* msg, const char* key, unsigned long long size) {
	return xorCharArray(enc, msg, key, size);
}
char* OTP::decrypt(char* msg, const char* enc) {
	char* key = char[this->size]
	key = getEndOfFile(key, this->file, this->size, true);
	
	return xorCharArray(msg, enc, key, this.size);
}
char* OTP::decrypt(char* msg, const char* enc, const char* key, unsigned long long size) {
	return xorCharArray(msg, enc, key, size); 
}
char* OTP::mac(char* mac, const char* msg) {
	char key[this->size * 2];
	key = getEndOfFile(key, this->file, this->size * 2, true);
	return this->mac(mac, msg, key, this->size)
}
char* OTP::mac(char* mac, const char* msg, const char* key, unsigned long long size) {
	// Size is the size of the msg, key must be 2*size
	char key1[size];
	char key2[size];
	key1 = splitCharArray(key1, key2, key, size, size * 2);
	char inter[size];
	inter = andCharArray(inter, msg, key1, size);
	return orCharArray(mac, inter, key2, size);

}
bool OTP::verify(const char* msg, const char* mac) {
	char key[this->size * 2];
	key = getEndOfFile(key, this->file, this->size * 2, false);
	bool result = this->verify(msg, mac, key, this->size);
	if(result) {
		key = getEndOfFile(key, this->file, this->size * 2, true);
	}
	return result;
}
bool OTP::verify(const char* msg, const char* mac, const char* key, unsigned long long size) {
	// Size is the size of the msg, key must be 2*size
	char calcMac[size];
	calcMac = this->mac(calcMac, msg, key, size);
	bool result = equalCharArray(mac, calcMac, size);
	return result;
}

char* OTP::macEncrypt(char* enc, const char* msg) {
	// Size is the size of the msg, key must be 4*size
	char* key = char[this.size * 4]
	key = getEndOfFile(key, this.file, this.size * 4, true);
	
	char* key1 = char[this.size * 2];
	char* key2 = char[this.size * 2];
	key1 = splitCharArray(key1, key2, key, this.size * 2, this.size * 2);
	char* mac = char[this.size];
	mac = this.mac(mac, msg, key1, this.size);
	char* digest = char[this.size * 2];
	digest = concatCharArray(digest, msg, mac, this.size, this.size)
	char* enc = char[this.size * 2];
	return this.encrypt(enc, digest, key2, this.size * 2);
}

char* OTP::decryptVerify(char* msg, const char* enc) {
	// Size is the size of the msg, key must be 4*size
	char key[this->size * 4]
	key = getEndOfFile(key, this->file, this->size * 4, false);
	
	char key1[this->size * 2];
	char key2[this->size * 2];
	key1 = splitCharArray(key1, key2, key, this->size * 2, this->size * 2);
	
	char digest[this->size * 2];
	digest = this->decrypt(digest, enc, key2, this->size * 2);
	
	char msg[this->size];
	char mac[this->size];
	msg = splitCharArray(msg, mac, digest, this->size, this->size);
	
	bool valid = this->verify(msg, mac, key1, this->size);
	if(valid) {
		// Delete the bytes used only if verification is successful
		key = getEndOfFile(key, this->file, this->size * 4, true);
	}
	else {
		msg = NULL;
	}
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
	char buffer[size];
	
	ssize_t dataRead = read(fd, buffer, size);
	
	for(unsigned long long i = 0; i < size; i++) {
		output[i] = buffer[size - i];
	}
	
	if(truncate) {
		ftruncate(fd, offset);
	}
	return output;
}

