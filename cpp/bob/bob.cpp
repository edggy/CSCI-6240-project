#include <iostream>
#include <stdlib.h>
#include "../lib/tcp.h"
#include "../lib/common.h"
#include "../circuit/circuit.h"
#include "../OT/OT.h"
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>

int main(int argc, char ** argv){
  if (argc<2){
    std::cout<<"Usage: "<<argv[0]<<" <proxy port>"<<std::endl;
    return 0;
  }
  int proxy_port = atoi(argv[1]);

  TCP alice_conn;

  alice_conn.connect_to_server(proxy_port);
  if(!alice_conn.connected()){
    std::cout<<"unable to connect to alice_conn"<<std::endl;
  }else{
    std::string bob_key = readRand(16); // 128 bits
    std::string encrypted_bob_key = encryptWithBothKeys("alice.pub", "bob.key", bob_key);
    //std::cout << encrypted_bob_key << std::endl;
    alice_conn.send_data(str2hexstr(encrypted_bob_key));
    std::string encrypted_alice_key = hexstr2str(alice_conn.recv_data());
    std::string alice_key = decryptWithBothKeys("alice.pub", "bob.key", encrypted_alice_key);

    std::string session_key = session_hash(bob_key, alice_key);
    std::string aes_key = hexstr2str(session_key.substr(0,32));
    std::string aes_iv = hexstr2str(session_key.substr(32,32));
    std::string hmac_password = session_key.substr(64,32);
    std::string init_nonce = session_key.substr(96,32);

    alice_conn.set_keys(aes_key, aes_iv, hmac_password, init_nonce);
    OT ot(&alice_conn);
    std::cout<<ot.recv(false)<<std::endl;
  }

  alice_conn.disconnect();

  return 1;
}
