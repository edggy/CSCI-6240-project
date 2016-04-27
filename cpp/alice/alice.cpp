#include <iostream>
#include <stdlib.h>
#include "../lib/tcp.h"
#include "../lib/common.h"
#include "../circuit/circuit.h"
#include "../OT/OT.h"

#include <unistd.h>
#include <map>
#include <vector>
#include <stdio.h>
#include <unistd.h>


#include "cryptopp/osrng.h"

using namespace CryptoPP;

void * connection_made(TCP bob_conn){
  //TODO key exchange


  return 0;
}

int main(int argc, char ** argv){
  if (argc<2){
    std::cout<<"Usage: "<<argv[0]<<" <listen port>"<<std::endl;
    return 0;
  }

  TCP server;
  if(server.start_server(atoi(argv[1]))){
    TCP bob_conn = server.accept_client();
    std::string command = hexstr2str(bob_conn.recv_data());
    std::string bob_key = decryptWithBothKeys("bob.pub", "alice.key", command);
    std::string alice_key = readRand(16); // 128 bits
    std::string encrypted_alice_key = encryptWithBothKeys("bob.pub", "alice.key", alice_key);

    bob_conn.send_data(str2hexstr(encrypted_alice_key));
    std::string session_key = session_hash(bob_key, alice_key);
    std::string aes_key = hexstr2str(session_key.substr(0,32));
    std::string aes_iv = hexstr2str(session_key.substr(32,32));
    std::string hmac_password = session_key.substr(64,32);
    std::string init_nonce = session_key.substr(96,32);

    bob_conn.set_keys(aes_key, aes_iv, hmac_password, init_nonce);

    OT ot(&bob_conn);

    ot.send("message 0", "message uno");

    bob_conn.disconnect();
    server.disconnect();
    std::cout<<"client disconnected"<<std::endl;

  }

  return 1;
}
