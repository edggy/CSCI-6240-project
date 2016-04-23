#include "channel_wrapper.h"
#include "stdio.h"

ChannelWrapper::ChannelWrapper(int socketfd_in = -1, int port, std::string init_nonce){

	// Initialize values
	socketfd = socketfd_in;

    memset( aes_key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH );
    memset( aes_iv, 0x00, CryptoPP::AES::BLOCKSIZE );

    hmac_password="ABVKASJF:JKLAGLSDYFGYISAPABKDSJLADSYFGSHDFLjklsafdjkagyofdbasdfyt";
}

bool ChannelWrapper::start_server(int port){
	return channel.start_server(port);
}

bool ChannelWrapper::connect_to_server(int port){
	return channel.connect_to_server(port);
}

ssize_t ChannelWrapper::send(const void *buf, size_t len, int flags){

}

ssize_t ChannelWrapper::recv(void *buf, size_t len, int flags){

}

string ChannelWrapper::encrypt(string enc, const string msg, const string key, unsigned long long size){

}

string ChannelWrapper::decrypt(string msg, const string enc, const string key, unsigned long long size){

}

string ChannelWrapper::mac(string mac, const string msg, const string key, unsigned long long size){

}

bool ChannelWrapper::verify(bool valid, const string msg, const string mac, const string key, unsigned long long size){

}

string ChannelWrapper::macEncrypt(string digest, const string msg, const string key, unsigned long long size){

}

string ChannelWrapper::decryptVerify(string msg, const string digest, const string key, unsigned long long size){

}