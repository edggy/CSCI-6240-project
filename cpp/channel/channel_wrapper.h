#pragma once

#include <string.h>

using std;

class ChannelWrapper: public SecureChannel{
	
	public:
		ssize_t send(const void *buf, size_t len, int flags);
		ssize_t recv(void *buf, size_t len, int flags);
		string encrypt(string enc, const string msg, const string key, unsigned long long size);
		string decrypt(string msg, const string enc, const string key, unsigned long long size);
		string mac(string mac, const string msg, const string key, unsigned long long size);
		string verify(bool valid, const string msg, const string mac, const string key, unsigned long long size);
		string macEncrypt(string digest, const string msg, const string key, unsigned long long size);
		string decryptVerify(string msg, const string digest, const string key, unsigned long long size);

	private:

};