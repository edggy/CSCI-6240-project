#include "circuit.h"
#include <iostream>
#include <assert.h>

int main(int argc, char* argv[])
{

	std::cout << "Start basic test" << std::endl;
	WIRE i1 = {0,1}, i2 = {0,1}, o = {0,1},  ig1 = {0,1}, ig2 = {0,1}, og = {0,1};
	std::cout << "Example wire: " << int(i1.first) << int(i1.second) << std::endl;
	//And gate
	NORMALTABLE t;
	t.push_back(NORMALTUPLE {0,0,0});
	t.push_back(NORMALTUPLE {0,1,0});
	t.push_back(NORMALTUPLE {1,0,0});
	t.push_back(NORMALTUPLE {1,1,1});
	NormalGate g (t);

	MillionaireCircuit m;
	int n = 20;
	ig1 = m.garbleWire(n);

	std::cout << "garbled wire 1: " << int(ig1.first) << int(ig1.second) << std::endl;
	ig2 = m.garbleWire(n);
	std::cout << "garbled wire 2: " << int(ig2.first) << int(ig2.second) << std::endl;
	og = m.garbleWire(n);
	std::cout << "garbled out wire: " << int(og.first) << int(og.second) << std::endl;

	GarbledGate g1 = g.garble(ig1, ig2, og);

	TYPE in1 = ig1.second;
	TYPE in2 = ig2.second;
	std::cout << "Garbled Input: " << int(in1) << int(in2) << std::endl;
	TYPE outg = g1.computeGate(in1, in2);
	std::cout << "Garbled Output: " << int(outg) << std::endl;
	std::cout << "Output: " << int(m.getResult(o, og, outg)) << std::endl;


	// MillionaireCircuit m;
	// m.generateCircuit();
}