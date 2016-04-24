#pragma once

#include <stdlib.h>
#include <vector>
#include <utility> 		// for std::pair
#include <tuple>		// for std::tuple; c++ 11 only
#include <cstdlib>      // std::rand, std::srand

//Waiting until sure these work
// #include "../crypto/otp.h"
// #include "../crypto/oneTwoOT.h"
// #include "../crypto/secureChannel.h"

//no idea what the datatypes are...
//#define TYPE unsigned long long 

#define TYPE unsigned char
#define TYPE_KEY TYPE
#define TYPE_MAC TYPE // do they need to be the same?

#define WIRE std::pair<TYPE, TYPE>
// k0, k1, out
#define NORMALTUPLE std::tuple<TYPE, TYPE, TYPE>
#define NORMALTABLE	std::vector<NORMALTUPLE>
// A normal gate's mapping


#define GARBLEDPAIR std::pair<TYPE, TYPE_MAC>
#define GARBLEDTABLE std::vector<GARBLEDPAIR>

//Dummy functions that will be removed once everything else works
namespace dummy
{
	// OTP
	TYPE OTP(TYPE i, TYPE_KEY k);
	// Random. Assume TYPE == TYPE_KEY
	TYPE myrandom (TYPE_KEY field);

	// Compute Mac
	TYPE_MAC computeMac(TYPE i);
}



// class Gate
// {
// public:

// private:
// 	Gate *prev1, *prev2, *next;
// 	WIRE *in1, *in2, *out;
// };


class GarbledGate
{
public:	
	GarbledGate(GARBLEDTABLE g) :
		table(g) {}
	TYPE computeGate(TYPE k0, TYPE k1);
private:
	GARBLEDTABLE table;
};


class NormalGate
{
public:
	NormalGate(NORMALTABLE t) :
		table(t) {}
	TYPE computeGate(TYPE k0, TYPE k1);
	GarbledGate garble(WIRE k0, WIRE k1, WIRE out);
private:
	NORMALTABLE table;
};


class Circuit
{
public:
	Circuit() = default;
	~Circuit() = default;

	WIRE garbleWire(TYPE_KEY field) { return std::make_pair(dummy::myrandom(field), dummy::myrandom(field)); }
	// virtual void garble();
	// virtual void compute();
	TYPE getResult(WIRE original, WIRE garbled, TYPE val);
protected:
	// std::vector<NormalGate> gates;
	// std::vector<WIRE> wires;
};

class MillionaireCircuit : public Circuit
{
public:
	// MillionaireCircuit() =
	void generateCircuit();
private:

};