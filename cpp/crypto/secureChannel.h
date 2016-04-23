#pragma once
#include <unistd.h>

class SecureChannel {
	public:
		virtual ssize_t send(const void *buf, int flags);
		virtual ssize_t recv(void *buf, int flags);
		virtual char* encrypt(char* enc, const char* msg);
		virtual char* decrypt(char* msg, const char* enc);
		virtual char* mac(char* mac, const char* msg);
		virtual bool verify(const char* msg, const char* mac);
		virtual char* macEncrypt(char* enc, const char* msg);
		virtual char* decryptVerify(char* msg, const char* enc);
};
