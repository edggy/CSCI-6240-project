#include "circuit.h"
#include <string>
#include <iostream>
#include <assert.h>

int main(int argc, char* argv[])
{

  std::cout << "Start basic test" << std::endl;

  Gate test1;
  NORMTABLE table;
  table.push_back(false);
  table.push_back(false);
  table.push_back(false);
  table.push_back(true);
  test1.n_table = table;
  test1.garble();


  Gate test2;
  test2.unserialize(test1.serialize());
  test2.setIn0(test1.w_in0.second);
  test2.setIn1(test1.w_in1.second);

  test2.eval();
  if (test2.g_out==test1.w_out.first){
    std::cout<<"true"<<std::endl;
  }else if (test2.g_out==test1.w_out.second){
    std::cout<<"false"<<std::endl;
  }else{
    std::cout<<"neither :("<<std::endl;
  }
	// MillionaireCircuit m;
	// m.generateCircuit();
}