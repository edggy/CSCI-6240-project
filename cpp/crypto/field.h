#pragma once

class Field {
	private:
		unsigned long long size;
		char* modulus;
	public:
		char* pow(char* output, const char* base, const char* exponent);
		char* mul(char* output, const char* num1, const char* num2);
		char* div(char* output, const char* numerator, const char* denominator);
		char* add(char* output, const char* num1, const char* num2);
		char* sub(char* output, const char* num1, const char* num2);
};