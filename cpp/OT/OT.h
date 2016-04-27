#ifndef OT_H
#define OT_H
#include "field.h"
#include "../lib/tcp.h"

class OT {
	private:
		Field field;
		CryptoPP::Integer generator;
		TCP* chan;

	public:
		OT(TCP* sc);
		void send(std::string msg0, std::string msg1);
		std::string recv(bool bit);
};
#endif
