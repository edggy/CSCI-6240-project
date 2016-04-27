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

std::vector<std::string> input_chooser(std::vector<Wire *> wires, int bits, int input){
  std::vector<std::string> result;
  if (input%2==1){
    result.push_back(wires[0]->one);
  }else{
    result.push_back(wires[0]->zero);
  }
  input /=2;
  for(int i=1;i<bits;i++){
    if (input%2==1){
      result.push_back(wires[2*i-1]->one);
      result.push_back(wires[2*i]->one);
    }else{
      result.push_back(wires[2*i-1]->zero);
      result.push_back(wires[2*i]->zero);
    }
    input /= 2;
  }
  return result;
}

int main(int argc, char ** argv){
  if (argc<3){
    std::cout<<"Usage: "<<argv[0]<<" <listen port> <the size of your fat stack of cash>"<<std::endl;
    return 0;
  }

  MillionaireCircuit circuit(BITS);

  circuit.generateCircuit();

  std::vector<std::string> gates = circuit.serializeGates();
  std::vector<std::string> alice_input;


  alice_input = input_chooser(circuit.alice_wires, BITS, atoi(argv[2]));

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

    std::cout<<"Sending Gates..."<<std::endl;

    for(int i=0;i<gates.size();i++){
      bob_conn.send_encrypted(gates[i]);
      bob_conn.recv_encrypted();
    }

    std::cout<<"Sending inputs..."<<std::endl;

    for(int i=0;i<alice_input.size();i++){
      bob_conn.send_encrypted(alice_input[i]);
      bob_conn.recv_encrypted();
    }

    std::cout<<"Offering inputs..."<<std::endl;

    for(int i=0;i<circuit.bob_wires.size();i++){
      ot.send(circuit.bob_wires[i]->zero, circuit.bob_wires[i]->one);
    }

    std::string output = bob_conn.recv_encrypted().substr(0,16);

    Wire *owire = circuit.getOutputWire();
    std::string result;
    if (output==owire->zero){
      result = "Bob has at least as much cash as Alice!";
    }else if (output==owire->one){
      result = "Alice has more dough";
    }else{
      result = "Something fucked up...";
    }

    std::cout<<result<<std::endl;

    bob_conn.send_encrypted(result);

    bob_conn.disconnect();
    server.disconnect();
    std::cout<<"client disconnected"<<std::endl;

  }

  return 1;
}
