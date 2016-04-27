#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <string>
#include "cryptopp/sha.h"
#include "cryptopp/rsa.h"
#include "cryptopp/files.h"

void split(const std::string& str, const std::string& delim, std::vector<std::string>& parts);

std::string readRand( int bytes );
std::string str2hexstr(std::string str );
std::string hexstr2str(std::string hexstr);
bool validNumber(std::string num);
std::string auth_hash(std::string salt, std::string card_id, std::string pin);
std::string hmac_hash(std::string password, std::string message);
std::string session_hash(std::string atm_key, std::string bank_key);
std::string hash(std::string in);
void load(const std::string& filename, CryptoPP::BufferedTransformation& bt);
void loadPrivateKey(const std::string& filename, CryptoPP::PrivateKey& key);
void loadPublicKey(const std::string& filename, CryptoPP::PublicKey& key);
std::string encryptWithBothKeys(std::string key1, std::string key2, std::string plain);
std::string decryptWithBothKeys(std::string key1, std::string key2, std::string cipher);
std::string aesEncrypt(std::string plaintext, byte key[], byte iv[]);
std::string aesDecrypt(std::string plaintext, byte key[], byte iv[]);

std::string xorby(std::string a, std::string b, int n);


#endif
