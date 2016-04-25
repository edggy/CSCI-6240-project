#include "circuit.h"

#include <algorithm>    // std::random_shuffle

//prolly want to remove these eventually
#include <assert.h>     /* assert */
#include <utility>
Gate::Gate(){
	parent0=NULL;
	parent1=NULL;
	child=NULL;
}

Gate::Gate(NORMTABLE table){
	n_table = table;
	parent0=NULL;
	parent1=NULL;
	child=NULL;
}

Gate::Gate(std::string serial){
	parent0=NULL;
	parent1=NULL;
	child=NULL;
	unserialize(serial);
}


void Gate::setIn0(Gate *parent){
	parent0 = parent;
	parent0->child = this;
}

void Gate::setIn0(NORMTYPE in){
	n_in0 = in;
}

void Gate::setIn0(GARBTYPE in){
	g_in0 = in;
}

void Gate::setIn1(Gate *parent){
	parent1 = parent;
	parent1->child = this;
}

void Gate::setIn1(NORMTYPE in){
	n_in1 = in;
}

void Gate::setIn1(GARBTYPE in){
	g_in1 = in;
}

void Gate::garble(){
	GARBTYPE x;
	GARBTYPE key0;
	GARBTYPE key1;
	GARBTYPE key_out;
	if(parent0!=NULL){
		w_in0 = parent0->w_out;
	}else{
		w_in0 = WIRE(readRand(KSIZE),readRand(KSIZE));
	}

	if(parent1!=NULL){
		w_in1 = parent1->w_out;
	}else{
		w_in1 = WIRE(readRand(KSIZE),readRand(KSIZE));
	}

	w_out = WIRE(readRand(KSIZE),readRand(KSIZE));

	for(int i=0;i<2;i++){
		for(int j=0;j<2;j++){
			GARBTYPE key0 = i==0 ? w_in0.first : w_in0.second;
			GARBTYPE key1 = j==0 ? w_in1.first : w_in1.second;
			GARBTYPE key_out = n_table[2*i+j] ? w_out.first : w_out.second;

			x = xorby(key0, xorby(key1, key_out, KSIZE), KSIZE);
			g_table.push_back( std::make_pair(x, hash(key_out)) );
		}
	}

	// randomly shuffle the entries
	std::random_shuffle (g_table.begin(), g_table.end());
}

void Gate::eval(){
	if(parent0!=NULL){
		g_in0 = parent0->g_out;
	}

	if(parent1!=NULL){
		g_in1 = parent1->g_out;
	}

	GARBTYPE key = xorby(g_in0, g_in1, KSIZE);
	for(int i=0; i< g_table.size();i++){
		//check if result = key_out and matches MACs
		GARBTYPE result = xorby(key, g_table[i].first, KSIZE); //g_table[i].first = X;
		if (hash(result) == g_table[i].second) //g_table[i].second = MAC
		{
			g_out = result;
			return;
		}
	}
}

std::string Gate::serialize(){
	std::string result;
	for(int i=0; i< g_table.size();i++){
		result += g_table[i].first + g_table[i].second;
	}
	return result;
}

void Gate::unserialize(std::string serial){
	int i = 0;
	GARBTYPE x;
	MACTYPE mac;
	g_table.clear();
	while(i<serial.length()){
		x = serial.substr(i,KSIZE);
		mac = serial.substr(i+KSIZE,64);
		g_table.push_back(std::make_pair(x, mac));
		i+=KSIZE+64;
	}
}


void MillionaireCircuit::generateCircuit(){
	//Generating default gates needed
	eqls_table.push_back(true);
	eqls_table.push_back(false);
	eqls_table.push_back(false);
	eqls_table.push_back(true);

	and_table.push_back(false);
	and_table.push_back(false);
	and_table.push_back(false);
	and_table.push_back(true);


	or_table.push_back(false);
	or_table.push_back(true);
	or_table.push_back(true);
	or_table.push_back(true);


	gt_table.push_back(false);
	gt_table.push_back(false);
	gt_table.push_back(true);
	gt_table.push_back(false);

	//first elem; LSB comp
	Gate *gtg, *eqg, *andg, *out;
	gates.clear();
	gates.push_back( Gate(gt_table) );
	out = &gates[0];
	out->garble();
	alice_wires.push_back(out->w_in0);
	bob_wires.push_back(out->w_in1);
	//from LSB to MSB
	for (int i = 1; i < BITS; i++)
	{
		//equals
		gates.push_back( Gate(eqls_table) );
		eqg = &gates[gates.size()-1];
		eqg->garble();
		alice_wires.push_back(eqg->w_in0);
		bob_wires.push_back(eqg->w_in1);

		//greater than
		gates.push_back( Gate(gt_table) );
		gtg = &gates[gates.size()-1];
		gtg->garble();
		alice_wires.push_back(gtg->w_in0);
		bob_wires.push_back(gtg->w_in1);

		//and
		gates.push_back( Gate(and_table) );
		andg = &gates[gates.size()-1];
		andg->setIn0(eqg);
		andg->setIn1(out);
		andg->garble();

		//or
		gates.push_back( Gate(or_table) );
		out = &gates[gates.size()-1];
		out->setIn0(gtg);
		out->setIn1(andg);
		andg->garble();
	}
}

std::vector<std::string> MillionaireCircuit::serializeGates(){
	std::vector<std::string> serial;
	for (int i=0;i<gates.size(); i++){
		serial.push_back(gates[i].serialize());
	}
	return serial;
}

std::string MillionaireCircuit::unserialize(std::vector<std::string> s_gates, std::vector<std::string> alice_inputs, std::vector<std::string> bob_inputs){

	Gate *gtg, *eqg, *andg, *out;
	int c = 0;
	int i = 0;
	gates.clear();

	gates.push_back(Gate(s_gates[i]));
	out = &gates[i];
	out->setIn0(alice_inputs[c]);
	out->setIn1(bob_inputs[c]);
	out->eval();
	c++;
	i++;

	//from LSB to MSB
	while (c+1 < alice_inputs.size() && c+1<bob_inputs.size() && i+3<s_gates.size())
	{
		//equals
		gates.push_back( Gate(s_gates[i]) );
		eqg = &gates[i];
		eqg->setIn0(alice_inputs[c]);
		eqg->setIn1(bob_inputs[c]);
		eqg->eval();
		c++;
		i++;

		//greater than
		gates.push_back( Gate(s_gates[i]) );
		gtg = &gates[i];
		eqg->setIn0(alice_inputs[c]);
		eqg->setIn1(bob_inputs[c]);
		eqg->eval();
		c++;
		i++;

		//and
		gates.push_back( Gate(s_gates[i]) );
		andg = &gates[i];
		andg->setIn0(eqg);
		andg->setIn1(out);
		andg->eval();
		i++;

		//or
		gates.push_back( Gate(s_gates[i]) );
		out = &gates[i];
		out->setIn0(gtg);
		out->setIn1(andg);
		out->eval();
		i++;
	}
	return out->g_out;
}

WIRE MillionaireCircuit::getOutputWire(){
	return gates[gates.size()-1].w_out;
}

/*
void Gate::setParents(Gate *prev0, Gate *prev1)
{
	this->prev0 = prev0;
	this->in0 = prev0->out;
	prev0->next = this;

	this->prev1 = prev1;
	this->in1 = prev1->out;
	prev1->next = this;
}

// Not necessary; useful for testing
TYPE Gate::computeGate(TYPE k0, TYPE k1)
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
GarbledGate Gate::garble(WIRE k0, WIRE k1, WIRE out)
{
	GARBLEDTABLE GARBTABLE;
	TYPE key0, key1, key_out, X;

	for(NORMALTUPLE tup : table)
	{
		key0 = (int(std::get<0>(tup)) == 0) ? k0.first : k0.second;
		key1 = (int(std::get<1>(tup)) == 0) ? k1.first : k1.second;
		key_out = (int(std::get<2>(tup)) == 0) ? out.first : out.second;
		X = dummy::OTP(key0, dummy::OTP(key1, key_out));
		GARBTABLE.push_back( std::make_pair(X, dummy::computeMac(key_out)) );
	}

	// randomly shuffle the entries
	std::random_shuffle ( table.begin(), table.end(), dummy::myrandom);
	return GarbledGate(GARBTABLE);
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
	int bits = sizeof(TYPE) * 8;

	//Generating default gates needed
	eqls_table.push_back(NORMALTUPLE {0,0,1});
	eqls_table.push_back(NORMALTUPLE {0,1,0});
	eqls_table.push_back(NORMALTUPLE {1,0,0});
	eqls_table.push_back(NORMALTUPLE {1,1,1});

	and_table.push_back(NORMALTUPLE {0,0,0});
	and_table.push_back(NORMALTUPLE {0,1,0});
	and_table.push_back(NORMALTUPLE {1,0,0});
	and_table.push_back(NORMALTUPLE {1,1,1});

	or_table.push_back(NORMALTUPLE {0,0,0});
	or_table.push_back(NORMALTUPLE {0,1,1});
	or_table.push_back(NORMALTUPLE {1,0,1});
	or_table.push_back(NORMALTUPLE {1,1,1});

	gt_table.push_back(NORMALTUPLE {0,0,0});
	gt_table.push_back(NORMALTUPLE {0,1,0});
	gt_table.push_back(NORMALTUPLE {1,0,1});
	gt_table.push_back(NORMALTUPLE {1,1,0});

	//first elem; MSB comp
	Gate *prev_gate, *prev0, *prev1, *next;
	gates.push_back( Gate(gt_table) );
	prev_gate = &gates[gates.size()-1];
	//from MSB to LSB
	for (int i = n-1; i > 0; i--)
	{
		//bit i comp gate
		prev0 = &gates.push_back( Gate(gt_table) );
		prev0 = &gates[gates.size()-1];
		//loop over
		for(int j = (n-1) - i; j > 0; j--)
		{
			gates.push_back( Gate(eqls_table) );
			prev1 = &gates[gates.size()-1];
			gates.push_back( Gate(and_table) );
			next = &gates[gates.size()-1];
			next.setparents(prev0, prev1);
			prev0 = next;
		}
		gates.push_back(Gate(or_table));
		next = &gates[gates.size()-1];
		next.setparents(prev_gate, prev0);
		prev_gate = next;
	}

}

*/