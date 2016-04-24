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
	TYPE key1, key2, key3, val;

	for(NORMALTUPLE tup : table)
	{
		key1 = (std::get<0>(tup) == 0) ? out.first : out.second;
		key2 = (std::get<1>(tup) == 0) ? out.first : out.second;
		key3 = (std::get<2>(tup) == 0) ? out.first : out.second;
		val = dummy::OTP(key1, dummy::OTP(key2, key3));
		gtable.push_back( std::make_pair(val, dummy::computeMac(val)) );
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
		result = dummy::OTP(key, pair.first);
		if (dummy::computeMac(result) == pair.second)
		{
			return pair.first;
		}
	}
}



void MillionaireCircuit::generateCircuit()
{
	// NORMALTABLE	and_gate = { {0,0,0}, {0,1,0}, {1,0,0}, {1,0,0} };

	// gates.push_back( NormalGate(and_gate) );
}




