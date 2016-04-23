#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "cryptopp/rsa.h"
#include "cryptopp/files.h"
#include "cryptopp/osrng.h"

using namespace std;

void Save(const string& filename, const CryptoPP::BufferedTransformation& bt){
	CryptoPP::FileSink file(filename.c_str());

	bt.CopyTo(file);
	file.MessageEnd();
}

void SavePrivateKey(const string& filename, const CryptoPP::PrivateKey& key){
  CryptoPP::ByteQueue queue;
  key.Save(queue);

  Save(filename, queue);
}

void SavePublicKey(const string& filename, const CryptoPP::PublicKey& key){
  CryptoPP::ByteQueue queue;
  key.Save(queue);

  Save(filename, queue);
}

int main(){
  CryptoPP::AutoSeededRandomPool rng;

  CryptoPP::RSA::PrivateKey alice_priv;
  CryptoPP::RSA::PrivateKey bob_priv;

  printf("Generating Alice priv key\n");
  alice_priv.GenerateRandomWithKeySize(rng, 2048);
  printf("Generating Alice pub key\n");
  CryptoPP::RSA::PublicKey alice_pub(alice_priv);

  printf("Generating Bob priv key\n");  
  bob_priv.GenerateRandomWithKeySize(rng, 2048);
  printf("Generating Bob pub key\n");  
  CryptoPP::RSA::PublicKey bob_pub(bob_priv);

  printf("Saving keys\n");
  SavePrivateKey("bin/alice.key", alice_priv);
  SavePublicKey("bin/alice.pub", alice_pub);
  SavePrivateKey("bin/bob.key", bob_priv);
  SavePublicKey("bin/bob.pub", bob_pub);
}
