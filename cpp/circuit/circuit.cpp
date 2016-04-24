#include "circuit.h"

#include <algorithm>    // std::random_shuffle

//prolly want to remove these eventually
#include <assert.h>     /* assert */

TYPE dummy::OTP(TYPE i, TYPE_KEY k) { return i ^ k; }

// Random. Assume TYPE == TYPE_KEY
TYPE dummy::myrandom (TYPE_KEY field) { return std::rand() % field;}

// Compute Mac
TYPE_MAC dummy::computeMac(TYPE i) { return i; }


// Gate::Gate(Gate *prev1, Gate *prev2)
// {
// 	// doubly linked. Is this necessary? Not really.
// 	this->prev1 = prev1;
// 	prev1->next = this;
// 	this->prev2 = prev2;
// 	prev2->next = this;
// 	next = nullptr;
// }

TYPE NormalGate::computeGate(TYPE k0, TYPE k1)
{
	for(NORMALTUPLE tup : table)
	{
		if (std::get<0>(tup) == k0 and std::get<1>(tup) == k1)
		{
			return std::get<2>(tup);
		}
	}
}

// Inputted wires are already garbled
GarbledGate NormalGate::garble(WIRE k0, WIRE k1, WIRE out)
{
	GARBLEDTABLE gtable;
	TYPE key0, key1, key_out, X;

	for(NORMALTUPLE tup : table)
	{
		key0 = (int(std::get<0>(tup)) == 0) ? k0.first : k0.second;
		key1 = (int(std::get<1>(tup)) == 0) ? k1.first : k1.second;
		key_out = (int(std::get<2>(tup)) == 0) ? out.first : out.second;
		X = dummy::OTP(key0, dummy::OTP(key1, key_out));
		gtable.push_back( std::make_pair(X, dummy::computeMac(key_out)) );
	}

	// randomly shuffle the entries
	std::random_shuffle ( table.begin(), table.end(), dummy::myrandom);
	return GarbledGate(gtable);
}

TYPE GarbledGate::computeGate(TYPE k0, TYPE k1)
{
	TYPE result;
	TYPE key = dummy::OTP(k0, k1);
	for(GARBLEDPAIR pair : table)
	{
		//check if result = key_out and matches MACs
		result = dummy::OTP(key, pair.first); //pair.first = X;
		if (dummy::computeMac(result) == pair.second) //pair.second = MAC
		{
			return result;
		}
	}
}

TYPE Circuit::getResult(WIRE original, WIRE garbled, TYPE val)
{
	if (val == garbled.first)
	{
		return original.first;
	}
	else if (val == garbled.second)
	{
		return original.second;
	}
	else
	{
		assert(false);
	}
}

void MillionaireCircuit::generateCircuit()
{
	// NORMALTABLE	and_gate = { {0,0,0}, {0,1,0}, {1,0,0}, {1,0,0} };

	// gates.push_back( NormalGate(and_gate) );
}





