#include <iostream>
#include <iomanip>

#include "CryptoPP/modes.h"
#include "CryptoPP/aes.h"
#include "CryptoPP/filters.h"

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


int main(int argc, char* argv[]) {
    //Key and IV setup
    //AES encryption uses a secret key of a variable length (128-bit, 196-bit or 256-
    //bit). This key is secretly exchanged between two parties before communication
    //begins. DEFAULT_KEYLENGTH= 16 bytes
    byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ], iv[ CryptoPP::AES::BLOCKSIZE ];
    memset( key, 0xAB, CryptoPP::AES::DEFAULT_KEYLENGTH );
    memset( iv, 0xCD, CryptoPP::AES::BLOCKSIZE );
    std::cout << CryptoPP::AES::DEFAULT_KEYLENGTH << std::endl;
    std::cout << CryptoPP::AES::BLOCKSIZE << std::endl;
    std::string plaintext = "Now is the time for all good men to come to the aide...";
    std::string ciphertext = aesEncrypt(plaintext, key, iv);
    std::string decrypted = aesDecrypt(ciphertext, key, iv);
    std::cout << plaintext << std::endl;
    std::cout << ciphertext << std::endl;
    std::cout << decrypted << std::endl;
    return 0;
}
