#ifndef FIELD_H
#define FIELD_H

#include "../lib/cryptopp/integer.h"

class Field {
	private:
		CryptoPP::Integer modulus;
	public:
		Field();
		Field(CryptoPP::Integer modulus);
		CryptoPP::Integer pow(const CryptoPP::Integer base, const CryptoPP::Integer exp);
		CryptoPP::Integer mul(const CryptoPP::Integer a, const CryptoPP::Integer b);
		CryptoPP::Integer div(const CryptoPP::Integer a, const CryptoPP::Integer b);
		CryptoPP::Integer add(const CryptoPP::Integer a, const CryptoPP::Integer b);
		CryptoPP::Integer sub(const CryptoPP::Integer a, const CryptoPP::Integer b);
		CryptoPP::Integer mod(const CryptoPP::Integer num);
};

std::string int2str(const CryptoPP::Integer num);
CryptoPP::Integer str2int(const std::string num);
#endif
