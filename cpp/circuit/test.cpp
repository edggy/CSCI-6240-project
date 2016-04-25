#include "circuit.h"
#include <string>
#include <iostream>
#include <assert.h>

int main(int argc, char* argv[])
{

  std::cout << "Start basic test" << std::endl;

  MillionaireCircuit alice, bob;

  alice.generateCircuit();

  std::vector<std::string> gates = alice.serializeGates();
  std::vector<std::string> alice_input, bob_input;

  alice_input.push_back(alice.alice_wires[0].second);//first represents 0
  bob_input.push_back(alice.bob_wires[0].second);   //second represents 1

  std::string output = bob.unserialize(gates, alice_input, bob_input);

  WIRE owire = alice.getOutputWire();

  if (output==owire.first){
    std::cout<<"true"<<std::endl;
  }else if (output==owire.second){
    std::cout<<"false"<<std::endl;
  }else{
    std::cout<<"neither :("<<std::endl;
  }
	// MillionaireCircuit m;
	// m.generateCircuit();
}