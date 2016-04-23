#include <sys/types.h>
#include <sys/socket.h>

#include "oneTwoOT.h"
#include "otp.h"

void otSend(const char* msg1, const char* msg2, unsigned long long length, const char* generator, Field field, int sockfd) {
	// Choose a random n-byte x
	char* x = new char[length];
	char* X = new char[length];
	char* Y = new char[length];
	char* K0 = new char[length];
	char* K1 = new char[length];
	char* C0 = new char[length];
	char* C1 = new char[length];
	
	x = getRandomData(x, length);
	
	// X = g^x mod n
	
	X = field.pow(X, g, x);
	
	// Send X to Bob
	int sent = self._socket.send(sockfd, X, length, 0);
	
	// Recieve Y from Bob
	
	Y = self._socket.recv(sockfd, Y, length, 0);
	
	// K0 = Y^x mod n
	K0 = field.pow(K0, Y, x)
	
	// K1 = (Y/X)^x mod n
	K1 = field.pow(K1, field.div(K1, Y, X), x)
	
	// C0 = Enc_K0(message0)
	C0 = xorCharArray(C0, message0, K0, length)
	
	//C1 = Enc_K1(message1)
	C1 = xorCharArray(C1, message1, K1, length)

	// Send C0 and C1 to Bob
	int sent = self._socket.send(sockfd, C0, length, 0);
	int sent = self._socket.send(sockfd, C1, length, 0);
}

char* otRecv(char* output, bool bit, unsigned long long length, unsigned long long generator, Field field) {
	
}