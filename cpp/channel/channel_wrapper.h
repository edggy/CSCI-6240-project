#pragma once

#include <string>
#include "secureChannel.h"

class ChannelWrapper: public SecureChannel{
	
	public:
		int socketfd; 
		ChannelWrapper(int socketfd = -1, int port, std::string init_nonce);

		bool start_server();
		bool connect_to_server();

		ssize_t send(const void *buf, size_t len, int flags);
		ssize_t recv(void *buf, size_t len, int flags);
		char* encrypt(char* enc, const char* msg, const char* key, unsigned long long size);
		char* decrypt(char* msg, const char* enc, const char* key, unsigned long long size);
		char* mac(char* mac, const char* msg, const char* key, unsigned long long size);
		bool verify(bool valid, const char* msg, const char* mac, const char* key, unsigned long long size);
		char* macEncrypt(char* digest, const char* msg, const char* key, unsigned long long size);
		char* decryptVerify(char* msg, const char* digest, const char* key, unsigned long long size);

	private:
		Channel channel;
		std::string my_nonce;
    	std::string their_nonce;

    	byte aes_key[ CryptoPP::AES::DEFAULT_KEYLENGTH ], aes_iv[ CryptoPP::AES::BLOCKSIZE ];

    	std::string hmac_password;
};