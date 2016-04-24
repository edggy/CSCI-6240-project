#include "circuit.h"

int main(int argc, char* argv[])
{
	WIRE i1, i2, o;
	NORMALTABLE t;
	t.push_back(NORMALTUPLE {0,0,0});
	t.push_back(NORMALTUPLE {0,1,0});
	t.push_back(NORMALTUPLE {1,0,0});
	t.push_back(NORMALTUPLE {1,1,1});
	NormalGate g1 (t);

	// MillionaireCircuit m;
	// m.generateCircuit();
}