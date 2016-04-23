#pragma once
#include "field.h"
#include "secureChannel.h"

class OneTwoOT {
	private:
		unsigned long long length;
		unsigned long long generator;
		Field field;
		SecureChannel chan;
		
	public:
		void send(const char* msg1, const char* msg2);
		char* recv(char* output, bool bit);
}

