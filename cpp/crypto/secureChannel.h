#pragma once
#include <unistd.h>

class SecureChannel {
	public:
		virtual ssize_t csend(const void *buf, int flags)=0;
		virtual ssize_t crecv(void *buf, int flags)=0;
		virtual char* encrypt(char* enc, const char* msg)=0;
		virtual char* decrypt(char* msg, const char* enc)=0;
		virtual char* mac(char* mac, const char* msg)=0;
		virtual bool verify(const char* msg, const char* mac)=0;
		virtual char* macEncrypt(char* enc, const char* msg)=0;
		virtual char* decryptVerify(char* msg, const char* enc)=0;
		virtual char* encrypt(char* enc, const char* msg, const char* key, unsigned long long size)=0;
		virtual char* decrypt(char* msg, const char* enc, const char* key, unsigned long long size)=0;
		virtual char* mac(char* mac, const char* msg, const char* key, unsigned long long size)=0;
		virtual bool verify(const char* msg, const char* mac, const char* key, unsigned long long size)=0;
		virtual char* macEncrypt(char* enc, const char* msg, const char* key, unsigned long long size)=0;
		virtual char* decryptVerify(char* msg, const char* enc, const char* key, unsigned long long size)=0;
};
