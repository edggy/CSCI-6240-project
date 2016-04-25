#ifndef TCP_H
#define TCP_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <iomanip>
#include "cryptopp/filters.h"
#include <cryptopp/aes.h>


class TCP{
  private:
    std::string my_nonce;
    std::string their_nonce;

    byte aes_key[ CryptoPP::AES::DEFAULT_KEYLENGTH ], aes_iv[ CryptoPP::AES::BLOCKSIZE ];//TODO initialize in key exchange

    std::string hmac_password;//TODO initialize in key exchange
  public:
    int socketfd;
    TCP(int socketfd_in=-1){
      socketfd=socketfd_in;

      //TODO set these to be random bytes during keygen
      memset( aes_key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH );
      memset( aes_iv, 0x00, CryptoPP::AES::BLOCKSIZE );

      hmac_password="ABVKASJF:JKLAGLSDYFGYISAPABKDSJLADSYFGSHDFLjklsafdjkagyofdbasdfyt";
    };
    bool connected();
    bool connect_to_server(int port);
    bool start_server(int port);
    TCP accept_client();

    void set_keys(std::string aes_key, std::string aes_iv, std::string hmac_password, std::string init_nonce);

    void generateMyNonce();
    std::string getMyNonce();

    void setTheirNonce(std::string n);
    std::string getTheirNonce();

    bool send_encrypted(std::string data);
    bool send_nonce(std::string data);
    bool send_aes(std::string data);
    bool send_hmac(std::string data);
    bool send_data(std::string data);

    std::string recv_encrypted(int num_bytes=8192);
    std::string recv_nonce(int num_bytes=8192);
    std::string recv_aes(int num_bytes=8192);
    std::string recv_hmac(int num_bytes=8192);
    std::string recv_data(int num_bytes=8192);


    void disconnect();

};

#endif
