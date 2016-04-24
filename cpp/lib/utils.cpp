#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include "utils.h"
#include "cryptopp/osrng.h"
#include "cryptopp/sha.h"
#include "cryptopp/modes.h"
#include "cryptopp/aes.h"
#include "cryptopp/filters.h"

using namespace CryptoPP;

void split(const std::string& str, const std::string& delim, std::vector<std::string>& parts) {
  parts.clear();
  size_t start=0;
  size_t end = str.find(delim);
  while (end !=std::string::npos) {
    parts.push_back(str.substr(start, end-start));
    start = end+1;
    end = str.find(delim, start);
  }
  parts.push_back(str.substr(start));
}

std::string str2hexstr(std::string str){
  std::string hexstr;
  char hexchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  for (unsigned long x=0; x<str.size(); x++){
    hexstr+=hexchars[(str[x]&0xf0) >> 4];
    hexstr+=hexchars[str[x]&0x0f];
  }
  return hexstr;
}

std::string hexstr2str(std::string hexstr){
  std::string str;
  for (unsigned long x=0; x<hexstr.size(); x+=2){
    str.push_back((char)(int)strtol(hexstr.substr(x,2).c_str(), NULL, 16));
  }
  return str;
}

bool validNumber(std::string num){
  //check that if when you convert to an int and back again it matches
  std::ostringstream os ;
  os << atoi(num.c_str()) ;
  return num == os.str();
}

std::string auth_hash(std::string salt, std::string card_id, std::string pin){
  CryptoPP::SHA512 hash;
  hash.Update((byte *) salt.c_str(), salt.length());
  hash.Update((byte *) card_id.c_str(), card_id.length());
  hash.Update((byte *) pin.c_str(), pin.length());
  char hash_buff[64];
  hash.Final((byte *)hash_buff);
  return str2hexstr(std::string(hash_buff, 64));
}

std::string hmac_hash(std::string password, std::string message){
  CryptoPP::SHA512 hash;
  hash.Update((byte *) password.c_str(), password.length());
  hash.Update((byte *) message.c_str(), message.length());
  char hash_buff[64];
  hash.Final((byte *)hash_buff);
  return str2hexstr(std::string(hash_buff, 64));
}

std::string session_hash(std::string atm_key, std::string bank_key){
  CryptoPP::SHA512 hash;
  hash.Update((byte *) atm_key.c_str(), atm_key.length());
  hash.Update((byte *) bank_key.c_str(), bank_key.length());
  char hash_buff[64];
  hash.Final((byte *)hash_buff);
  return str2hexstr(std::string(hash_buff, 64));
}

//read random bytes from /dev/random
std::string readRand( int bytes ){
  FILE* file = fopen("/dev/urandom", "r"); //open the file
  char buffer[bytes]; //create a buffer of the desired size
  char * res = fgets(buffer, bytes, file); //read the bytes
  if (res == NULL) {
    _exit(1); //exit on error
  }
  fclose(file); //close the file

  std::string str;
  str.assign(buffer, bytes); //push the bytse into a string
  return str;
}

void load(const std::string& filename, BufferedTransformation& bt){
  FileSource file(filename.c_str(), true /*pumpAll*/);

  file.TransferTo(bt);
  bt.MessageEnd();
}

void loadPrivateKey(const std::string& filename, PrivateKey& key){
  ByteQueue queue;

  load(filename, queue);
  key.Load(queue);
}

void loadPublicKey(const std::string& filename, PublicKey& key){
  ByteQueue queue;

  load(filename, queue);
  key.Load(queue);
}

std::string encryptWithBothKeys(std::string key_name1, std::string key_name2, std::string plain){
  std::string cipher, signature;
  AutoSeededRandomPool rng;

  RSA::PublicKey pub_key;
  RSA::PrivateKey priv_key;
  loadPublicKey(key_name1, pub_key);
  loadPrivateKey(key_name2, priv_key);

  RSAES_OAEP_SHA_Encryptor e_pub(pub_key);
  RSASSA_PKCS1v15_SHA_Signer signer(priv_key);

  StringSource ss1(plain, true,
    new PK_EncryptorFilter(rng, e_pub,
      new StringSink(cipher)
    )
  );

  StringSource ss2(cipher, true,
    new SignerFilter(rng, signer,
      new StringSink(signature)
    )
  );

  return cipher+signature;
}

std::string decryptWithBothKeys(std::string key_name1, std::string key_name2, std::string message){
  std::string cipher = message.substr(0,256);
  std::string recovered;
  AutoSeededRandomPool rng;

  RSA::PublicKey pub_key;
  RSA::PrivateKey priv_key;
  loadPublicKey(key_name1, pub_key);
  loadPrivateKey(key_name2, priv_key);

  RSAES_OAEP_SHA_Decryptor d_priv(priv_key);
  RSASSA_PKCS1v15_SHA_Verifier verifier(pub_key);

  StringSource ss1(cipher, true,
    new PK_DecryptorFilter(rng, d_priv,
      new StringSink(recovered)
    )
  );

  StringSource ss2(message, true,
    new SignatureVerificationFilter(
      verifier, NULL,
      SignatureVerificationFilter::THROW_EXCEPTION
    )
  );

  return recovered;
}

std::string aesEncrypt(std::string plaintext, byte key[], byte iv[]){
    std::string ciphertext;
    //
    // Create Cipher Text
    //
    CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );

    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink( ciphertext ) );
    stfEncryptor.Put( reinterpret_cast<const unsigned char*>( plaintext.c_str() ), plaintext.length() + 1 );
    stfEncryptor.MessageEnd();

    return ciphertext;
}

std::string aesDecrypt(std::string ciphertext, byte key[], byte iv[]){
    std::string decryptedtext;
    //
    // Decrypt
    //
    CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption( aesDecryption, iv );

    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink( decryptedtext ) );
    stfDecryptor.Put( reinterpret_cast<const unsigned char*>( ciphertext.c_str() ), ciphertext.size() );
    stfDecryptor.MessageEnd();

    return decryptedtext;
}

std::string recv_aes_encrypted(int sock, std::string hmac_password, std::string aes_key, std::string aes_iv, std::string my_nonce, std::string &their_nonce){

  // Read message
  char buffer[MAX_MSG_SIZE+1];

  int len = recv(sock, buffer, MAX_MSG_SIZE, 0);
  if(len > 0){
    buffer[len] = 0;
  }else{
    return "";
  }

  // Toss into string structure
  std::string enc_content(buffer);
  enc_content = hexstr2str(enc_content);

  // Process HMAC
  std::vector<std::string> hmac_data;
  split(enc_content, "|", hmac_data);

  // Check MAC and store ciphertext
  std::string ciphertext = "";
  if(hmac_data.size() >= 2 && hmac_hash(hmac_password, hmac_data[0]) == hmac_data[1]){
    ciphertext = hmac_data[0];
  }else{
    return "";
  }

  // Decrypt message
  ciphertext = hexstr2str(ciphertext);
  std::string plaintext;
  if(ciphertext != ""){
    byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], iv[CryptoPP::AES::BLOCKSIZE];
    memcpy(key, aes_key.c_str(), CryptoPP::AES::DEFAULT_KEYLENGTH);
    memcpy(iv, aes_iv.c_str(), CryptoPP::AES::BLOCKSIZE);
    plaintext = aesDecrypt(ciphertext, key, iv);
    plaintext = plaintext.substr(0, plaintext.size()-1);
  }else{
    return "";
  }

  plaintext = hexstr2str(plaintext);

  // Check nonce and return plaintext
  std::vector<std::string> nonce_data;
  split(plaintext, "|", nonce_data);  
  if(nonce_data.size() >= 3 && nonce_data[2] == my_nonce){
    their_nonce = nonce_data[1];
    return nonce_data[0];
  }else{
    return "";
  }
}

bool send_aes_encrypted(int sock, std::string msg, std::string aes_key, std::string aes_iv, std::string hmac_password, std::string their_nonce, std::string &my_nonce){

  // Pad message
  if(msg.length() < MAX_MSG_SIZE){

  }

  // Generate nonce
  std::string random_noise = str2hexstr(readRand(16));
  my_nonce = random_noise;

  // Wrap message with nonce
  std::string wrapped_message = str2hexstr(msg + "|" + my_nonce + "|" + their_nonce);

  // Encrypt message with AES
  byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], iv[CryptoPP::AES::BLOCKSIZE];
  memcpy(key, aes_key.c_str(), 16);
  memcpy(iv, aes_iv.c_str(), 16);
  std::string encrypted_message = aesEncrypt(wrapped_message, key, iv);
  encrypted_message = str2hexstr(encrypted_message);

  // Sign message with HMAC
  std::string signed_message = encrypted_message + "|" + hmac_hash(hmac_password, encrypted_message);
  signed_message = str2hexstr(signed_message);

  // Send message
  if(sock >= 0){
    if(send(sock, signed_message.c_str(), signed_message.length(), 0) >= 0){
      return true;
    }else{
      return false;
    }
  }else{
    return false;
  }
}

/*
int main(int argc, char* argv[]) {
    //Key and IV setup
    //AES encryption uses a secret key of a variable length (128-bit, 196-bit or 256-
    //bit). This key is secretly exchanged between two parties before communication
    //begins. DEFAULT_KEYLENGTH= 16 bytes
    byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ], iv[ CryptoPP::AES::BLOCKSIZE ];
    memset( key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH );
    memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );

    std::string plaintext = "Now is the time for all good men to come to the aide...";

    std::string ciphertext = aesEncrypt(plaintext, key, iv);
    std::string decrypted = aesDecrypt(ciphertext, key, iv);

    std::cout << plaintext << std::endl;
    std::cout << ciphertext << std::endl;
    std::cout << decrypted << std::endl;
    return 0;
}
*/
