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

#define KSIZE 16

struct GarbPair
{
	std::string ctext;
	std::string mac;
};

struct Wire
{
	std::string zero;
	std::string one;
	/* data */
};

class Gate
{
public:
	Gate();
	Gate(std::vector<bool> table);
	Gate(std::string serial);
	~Gate(){
		delete[] g_table;

		if(parent0 != NULL){
			delete w_in0;
		}

		if(parent1 != NULL){
			delete w_in1;
		}

		delete w_out;
	}
	void init();
	void setIn0(Gate *parent);
	void setIn0(std::string in);
	void setIn1(Gate *parent);
	void setIn1(std::string in);
	void garble();
	void eval();
	std::string serialize();
	void unserialize(std::string serial);

	Gate *parent0, *parent1;
	Wire *w_in0, *w_in1, *w_out;
	std::string g_in0, g_in1, g_out;
	std::vector<bool> n_table;

private:
	GarbPair* g_table;
};

class MillionaireCircuit
{
public:
	MillionaireCircuit(int b){bits=b;}
	~MillionaireCircuit(){
		for(int i=0;i<gates.size();i++){
			delete gates[i];
		}
	}
	void generateCircuit();
	std::vector<std::string> serializeGates();
	std::string unserialize(std::vector<std::string> s_gates, std::vector<std::string> alice_inputs, std::vector<std::string> bob_inputs);
	Wire * getOutputWire();
	void printGates();

	std::vector<Wire*> alice_wires;
	std::vector<Wire*> bob_wires;

private:
	std::vector<Gate *> gates;
	int bits;
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