#pragma once
#include "../lib/cryptopp/integer.h"

class Field {
	private:
		CryptoPP::Integer modulus;
	public:
		Field(CryptoPP::Integer modulus);
		CryptoPP::Integer pow(const CryptoPP::Integer base, const CryptoPP::Integer exp);
		CryptoPP::Integer mul(const CryptoPP::Integer a, const CryptoPP::Integer b);
		CryptoPP::Integer div(const CryptoPP::Integer a, const CryptoPP::Integer b);
		CryptoPP::Integer add(const CryptoPP::Integer a, const CryptoPP::Integer b);
		CryptoPP::Integer sub(const CryptoPP::Integer a, const CryptoPP::Integer b);
		CryptoPP::Integer mod(const CryptoPP::Integer num);
};

const char* to_char_arr(const CryptoPP::Integer num);
CryptoPP::Integer to_crytopp_int(const char* num);