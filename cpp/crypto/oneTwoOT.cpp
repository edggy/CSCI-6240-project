#include <sys/types.h>
#include <sys/socket.h>

#include "oneTwoOT.h"
#include "otp.h"

OneTwoOT::OneTwoOT(unsigned long long len, const char* gen, Field* fld, SecureChannel* sc) {
	this->length = len;
	this->generator = gen;
	this->field = fld;
	this->chan = sc;
}

void OneTwoOT::otsend(const char* msg0, const char* msg1) {
	this->otsend(msg0, msg1, this->length, this->generator, this->field);
}
void OneTwoOT::otsend(const char* msg0, const char* msg1, unsigned long long length, const char* generator, Field* field) {

	char* x = new char[length];
	char* X = new char[length];
	char* Y = new char[length];
	char* K0 = new char[length];
	char* K1 = new char[length];
	char* C0 = new char[length];
	char* C1 = new char[length];
	
	// Choose a random n-byte x
	x = getRandomData(x, length);
	
	// X = g^x mod n
	X = field->pow(X, generator, x);
	
	// Send X to Bob
	int sent = this->chan->csend(X, 0);
	
	// Recieve Y from Bob
	
	this->chan->crecv(Y, 0);
	
	// K0 = Y^x mod n
	K0 = field->pow(K0, Y, x);
	
	// K1 = (Y/X)^x mod n
	K1 = field->pow(K1, field->div(K1, Y, X), x);
	
	// C0 = Enc_K0(message0)
	C0 = this->chan->encrypt(C0, msg0, K0, length);
	
	//C1 = Enc_K1(message1)
	C1 = this->chan->encrypt(C1, msg1, K1, length);

	// Send C0 and C1 to Bob
	this->chan->csend(C0, 0);
	this->chan->csend(C1, 0);
	
	delete[] x;
	delete[] X;
	delete[] Y;
	delete[] K0;
	delete[] K1;
	delete[] C0;
	delete[] C1;
}
char* OneTwoOT::otrecv(char* output, bool bit) {
	return this->otrecv(output, bit, this->length, this->generator, this->field);
}
char* OneTwoOT::otrecv(char* output, bool bit, unsigned long long length, const char* generator, Field* field) {
	char* y = new char[length];
	char* X = new char[length];
	char* gy = new char[length];
	char* Y = new char[length];
	char* Kb = new char[length];
	char* C0 = new char[length];
	char* C1 = new char[length];
	char* Cb = new char[length];
	
	// Choose a random n-byte x
	y = getRandomData(y, length);
	
	// Recieve X from Alice
	this->chan->crecv(X, 0);

	// gy = g^y mod n
	gy = field->pow(gy, generator, y);
	
	if(bit) {
		// b = 1, Y = X * gy mod n = X * g^y mod n
		Y = field->mul(Y, X, gy);
	}
	else {
		// b = 0, Y = gy = g^y mod n
		Y = gy;
	}
	
	// Send Alice Y
	int size = this->chan->csend(Y, 0);
	
	// Kb = X^y mod n
	Kb = field->pow(Kb, X, y);

	// Get C0 and C1 from Alice
	this->chan->crecv(C0, 0);
	this->chan->crecv(C0, 0);
	
	if(bit) {
		// b = 1, Cb = C1
		Cb = C1;
	}
	else {
		// b = 0, Cb = C0
		Cb = C0;
	}
	
	delete[] y;
	delete[] X;
	delete[] gy;
	delete[] Y;
	delete[] Kb;
	delete[] C0;
	delete[] C1;
	delete[] Cb;
	
	// return Dec_Kb(Cb)
	return chan->decrypt(Cb, Kb);
}

