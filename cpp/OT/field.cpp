#ifndef FIELD_CPP
#define FIELD_CPP

#include "field.h"
#include "../lib/cryptopp/integer.h"
#include "../lib/cryptopp/nbtheory.h"
#include <sstream>

Field::Field() {
	this->modulus = CryptoPP::Integer("5570373270183181665098052481109678989411");
}

Field::Field(CryptoPP::Integer mod) {
	this->modulus = mod;
}

CryptoPP::Integer Field::pow(const CryptoPP::Integer base, const CryptoPP::Integer exp) {
	return a_exp_b_mod_c(base, exp, modulus);
}

CryptoPP::Integer Field::mul(const CryptoPP::Integer a, const CryptoPP::Integer b) {
	return a_times_b_mod_c(a, b, modulus);
}

CryptoPP::Integer Field::div(const CryptoPP::Integer a, const CryptoPP::Integer b) {
	return mul(a, b.InverseMod(modulus));
}

CryptoPP::Integer Field::add(const CryptoPP::Integer a, const CryptoPP::Integer b) {
	return (a+b)%modulus;
}

CryptoPP::Integer Field::sub(const CryptoPP::Integer a, const CryptoPP::Integer b) {
	return (a-b)%modulus;
}

CryptoPP::Integer Field::mod(const CryptoPP::Integer num) {
	return num%modulus;
}

std::string int2str(CryptoPP::Integer num){
	byte result[40];
	num.Encode(result,40);
	return std::string((char *)result,64);
}

CryptoPP::Integer str2int(std::string num){
	CryptoPP::Integer result;
	result.Decode((byte *)num.c_str(), 40);
	return result;
}
#endif
