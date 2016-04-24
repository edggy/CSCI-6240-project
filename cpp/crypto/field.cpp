#include "field.h"
#include "../lib/cryptopp/integer.h"
#include "../lib/cryptopp/nbtheory.h"
#include <sstream>

Field::Field(CryptoPP::Integer modulus) {
	this->modulus = modulus;
}

CryptoPP::Integer Field::pow(const CryptoPP::Integer base, const CryptoPP::Integer exp) {
	return a_exp_b_mod_c(base, exp, modulus);
}

CryptoPP::Integer Field::mul(const CryptoPP::Integer a, const CryptoPP::Integer b) {
	return a_times_b_mod_c(a, b, modulus);
}

CryptoPP::Integer Field::div(const CryptoPP::Integer a, const CryptoPP::Integer b) {
	return output;
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

char* to_char_arr(const CryptoPP::Integer num){

	std::stringstream ss;
	ss << num;
	return ss.str().c_str();
}

CryptoPP::Integer to_crytopp_int(const char* num){
	return CryptoPP::Integer(num);
}