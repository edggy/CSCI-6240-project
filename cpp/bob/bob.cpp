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


std::vector<std::string> input_chooser_ot(OT ot, int bits, int input){
  std::vector<std::string> result;
  result.push_back(ot.recv(input%2==1).substr(0,16));

  input /=2;
  for(int i=1;i<bits;i++){
    result.push_back(ot.recv(input%2==1).substr(0,16));
    result.push_back(ot.recv(input%2==1).substr(0,16));
    input /= 2;
  }
  return result;
}

int main(int argc, char ** argv){
  if (argc<2){
    std::cout<<"Usage: "<<argv[0]<<" <alice port>"<<std::endl;
    return 0;
  }
  int alice_port = atoi(argv[1]);
  int input = atoi(argv[2]);

  MillionaireCircuit bob(BITS);

  TCP alice_conn;

  alice_conn.connect_to_server(alice_port);
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

    std::vector<std::string> serial_gates, alice_input, bob_input;

    std::cout<<"Receiving gates..."<<std::endl;

    for(int i=0;i<4*BITS-3;i++){
      serial_gates.push_back( alice_conn.recv_encrypted().substr(0,320));
      alice_conn.send_encrypted("ACK");
    }

    std::cout<<"Receiving inputs..."<<std::endl;

    for(int i=0;i<2*BITS-1;i++){
      alice_input.push_back( alice_conn.recv_encrypted().substr(0,16));
      alice_conn.send_encrypted("ACK");
    }

    std::cout<<"Selecting inputs..."<<std::endl;

    bob_input = input_chooser_ot(ot, BITS, input);

    std::cout<<"Calculating answer..."<<std::endl;

    std::string output = bob.unserialize(serial_gates, alice_input, bob_input);
    alice_conn.send_encrypted(output);
    std::cout<<"And the result is: "<<alice_conn.recv_encrypted()<<std::endl;


  }

  alice_conn.disconnect();

  return 1;
}
