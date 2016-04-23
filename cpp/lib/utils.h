#pragma once

#include <vector>
#include <string>
#include "cryptopp/sha.h"
#include "cryptopp/rsa.h"
#include "cryptopp/files.h"

// split a string based on a delimiter
void split(const std::string& str, const std::string& delim, std::vector<std::string>& parts);

// creates a feed of random bytes
std::string readRand(int bytes);

// conversions to and from hex
std::string str2hexstr(std::string str);
std::string hexstr2str(std::string hexstr);

// checks if a number is within allowed bounds
bool validNumber(std::string num);

// hashing
std::string auth_hash(std::string salt, std::string card_id, std::string pin);
std::string hmac_hash(std::string password, std::string message);
std::string session_hash(std::string atm_key, std::string bank_key);

// loading keys
void load(const std::string& filename, CryptoPP::BufferedTransformation& bt);
void loadPrivateKey(const std::string& filename, CryptoPP::PrivateKey& key);
void loadPublicKey(const std::string& filename, CryptoPP::PublicKey& key);

// enc and dec with both keys
std::string encryptWithBothKeys(std::string key1, std::string key2, std::string plain);
std::string decryptWithBothKeys(std::string key1, std::string key2, std::string cipher);

// enc and dec with single keys
std::string aesEncrypt(std::string plaintext, byte key[], byte iv[]);
std::string aesDecrypt(std::string plaintext, byte key[], byte iv[]);
