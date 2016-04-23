#include <sys/types.h>
#include <sys/socket.h>

#include "oneTwoOT.h"
#include "otp.h"

OneTwoOT::OneTwoOT(unsigned long long len, unsigned long long gen, Field fld, SecureChannel sc) {
	length = len;
	generator = gen;
	field = fld;
	chan = sc;
}

void OneTwoOT::send(const char* msg1, const char* msg2, unsigned long long length, const char* generator, Field field) {

	char* x = char[length];
	char* X = char[length];
	char* Y = char[length];
	char* K0 = char[length];
	char* K1 = char[length];
	char* C0 = char[length];
	char* C1 = char[length];
	
	// Choose a random n-byte x
	x = getRandomData(x, length);
	
	// X = g^x mod n
	X = field.pow(X, g, x);
	
	// Send X to Bob
	int sent = chan.send(X, length, 0);
	
	// Recieve Y from Bob
	
	Y = chan.recv(Y, length, 0);
	
	// K0 = Y^x mod n
	K0 = field.pow(K0, Y, x)
	
	// K1 = (Y/X)^x mod n
	K1 = field.pow(K1, field.div(K1, Y, X), x)
	
	// C0 = Enc_K0(message0)
	C0 = chan.encrypt(C0, message0, K0, length)
	
	//C1 = Enc_K1(message1)
	C1 = chan.encrypt(C1, message1, K1, length)

	// Send C0 and C1 to Bob
	int sent = chan.send(C0, length, 0);
	int sent = chan.send(C1, length, 0);
}

char* OneTwoOT::recv(char* output, bool bit, unsigned long long length, unsigned long long generator, Field field) {
	char* y = char[length];
	char* X = char[length];
	char* gy = char[length];
	char* Y = char[length];
	char* K0 = char[length];
	char* K1 = char[length];
	char* C0 = char[length];
	char* C1 = char[length];
	
	// Choose a random n-byte x
	y = getRandomData(y, length)
	
	// Recieve X from Alice
	X = chan.recv(X, length, 0)

	// gy = g^y mod n
	gy = field.pow(g, y)
	
	if(b) {
		// b = 1, Y = X * gy mod n = X * g^y mod n
		Y = field.mult(X, gy)
	}
	else {
		// b = 0, Y = gy = g^y mod n
		Y = gy
	}
	
	// Send Alice Y
	int size = chan.send(Y, length, 0)
	
	// Kb = X^y mod n
	Kb = field.pow(X, y)

	// Get C0 and C1 from Alice
	C0 = chan.recv(C0, length, 0)
	C1 = chan.recv(C0, length, 0)
	
	if b:
		// b = 1, Cb = C1
		Cb = C1
	else:
		// b = 0, Cb = C0
		Cb = C0

	
	// return Dec_Kb(Cb)
	return chan.decrypt(Cb, n, Kb)
}

