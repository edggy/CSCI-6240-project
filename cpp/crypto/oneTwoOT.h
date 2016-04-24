#pragma once
#include "field.h"
#include "secureChannel.h"

class OneTwoOT {
	private:
		unsigned long long length;
		const char* generator;
		Field* field;
		SecureChannel* chan;
		
	public:
		OneTwoOT(unsigned long long len, const char* gen, Field* fld, SecureChannel* sc);
		void otsend(const char* msg0, const char* msg1);
		void otsend(const char* msg0, const char* msg1, unsigned long long length, const char* generator, Field* field);
		char* otrecv(char* output, bool bit);
		char* otrecv(char* output, bool bit, unsigned long long length, const char* generator, Field* field);
};

