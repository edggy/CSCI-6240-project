#pragma once

#include <stdlib.h>
#include <vector>
#include <string>
#include <utility> 		// for std::pair
#include <cstdlib>      // std::rand, std::srand
#include "../lib/common.h"

//Waiting until sure these work
// #include "../crypto/otp.h"
// #include "../crypto/oneTwoOT.h"
// #include "../crypto/secureChannel.h"

//no idea what the datatypes are...
//#define TYPE unsigned long long
#define BITS 1

#define KSIZE 16

#define GARBTYPE std::string
#define MACTYPE std::string

#define GARBPAIR std::pair<GARBTYPE, MACTYPE>
#define WIRE std::pair<GARBTYPE, GARBTYPE>

#define GARBTABLE std::vector<GARBPAIR>
// k0, k1, out

#define NORMTYPE bool

#define NORMTABLE std::vector<NORMTYPE>
// A normal gate's mapping



class Gate
{
public:
	Gate();
	Gate(NORMTABLE table);
	Gate(std::string serial);
	void setIn0(Gate *parent);
	void setIn0(NORMTYPE in);
	void setIn0(GARBTYPE in);
	void setIn1(Gate *parent);
	void setIn1(NORMTYPE in);
	void setIn1(GARBTYPE in);
	void garble();
	void eval();
	std::string serialize();
	void unserialize(std::string serial);

	Gate *parent0, *parent1, *child;
	NORMTYPE n_in0, n_in1, n_out;
	WIRE w_in0, w_in1, w_out;
	GARBTYPE g_in0, g_in1, g_out;
	NORMTABLE n_table;

private:
	GARBTABLE g_table;
};

class MillionaireCircuit
{
public:
	MillionaireCircuit() = default;
	void generateCircuit();
	std::vector<std::string> serializeGates();
	std::string unserialize(std::vector<std::string> s_gates, std::vector<std::string> alice_inputs, std::vector<std::string> bob_inputs);
	WIRE getOutputWire();

	std::vector<WIRE> alice_wires;
	std::vector<WIRE> bob_wires;

private:
	std::vector<Gate> gates;
	NORMTABLE eqls_table, and_table, or_table, gt_table;
};



/*
class GarbledGate : Gate
{
public:
	GarbledGate(GARBLEDTABLE g) :
		table(g) {}
	TYPE computeGate(TYPE k0, TYPE k1);
private:
	GARBLEDTABLE table;
};


class NormalGate : Gate
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
	WIRE defaultWire() { return {0,1}; }
	WIRE garbleWire(TYPE_KEY field) { return std::make_pair(dummy::myrandom(field), dummy::myrandom(field)); }
	// virtual void garble();
	// virtual void compute();
	TYPE getResult(WIRE original, WIRE garbled, TYPE val);
protected:
	std::vector<NormalGate> gates;
	std::vector<WIRE> wires;
};

class MillionaireCircuit : public Circuit
{
public:
	MillionaireCircuit() = default;
	void generateCircuit();
private:
	NORMALTABLE eqls_table, and_table, or_table, gt_table;
};

*/