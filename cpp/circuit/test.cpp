#include "circuit.h"
#include <string>
#include <iostream>
#include <assert.h>

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

int main(int argc, char* argv[])
{
  if (argc<4){
    std::cout<<"Usage: "<<argv[0]<<" <bits> <alice moneys> <bob cash420>"<<std::endl;
    return 0;
  }

  int bits = atoi(argv[1]);

  MillionaireCircuit alice(atoi(argv[1])), bob(atoi(argv[1]));

  alice.generateCircuit();

  std::vector<std::string> gates = alice.serializeGates();
  std::vector<std::string> alice_input, bob_input;

  alice_input = input_chooser(alice.alice_wires, bits, atoi(argv[2]));
  bob_input = input_chooser(alice.bob_wires, bits, atoi(argv[3]));

  std::string output = bob.unserialize(gates, alice_input, bob_input);

  Wire *owire = alice.getOutputWire();
  if (output==owire->zero){
    std::cout<<"false"<<std::endl;
  }else if (output==owire->one){
    std::cout<<"true"<<std::endl;
  }else{
    std::cout<<"neither :("<<std::endl;
  }
	// MillionaireCircuit m;
	// m.generateCircuit();
  return 1;
}