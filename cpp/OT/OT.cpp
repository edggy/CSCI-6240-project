#ifndef OT_CPP
#define OT_CPP
#include "OT.h"
#include "../lib/common.h"
#include "field.h"

OT::OT(TCP* sc){
	chan = sc;
	field = Field(CryptoPP::Integer("5570373270183181665098052481109678989411"));
	generator = CryptoPP::Integer(31337);
}

void OT::send(std::string msg0, std::string msg1){
	CryptoPP::Integer x = str2int(readRand(16));
	CryptoPP::Integer gx = field.pow(generator,x);
	std::cout<<x<<" "<<gx<<" "<<str2int(int2str(x))<<std::endl;
	chan->send_encrypted(int2str(gx));
	CryptoPP::Integer ydat = str2int(chan->recv_encrypted());
	CryptoPP::Integer K0 = field.pow(ydat, x);
	CryptoPP::Integer K1 = field.pow(field.div(ydat, gx), x);
	std::string C0 = xorby(msg0, int2str(K0), 16);
	std::string C1 = xorby(msg1, int2str(K1), 16);
	chan->send_encrypted(C0);
	chan->send_encrypted(C1);
}

std::string OT::recv(bool bit){
	CryptoPP::Integer y = str2int(readRand(16));
	CryptoPP::Integer gy = field.pow(generator,y);
	CryptoPP::Integer gx = str2int(chan->recv_encrypted());
	std::cout<<y<<" "<<gy<<" "<<gx<<std::endl;
	CryptoPP::Integer ydat;
	if(bit){
		ydat = gy;
	}else{
		ydat = field.mul(gx, gy);
	}
	chan->send_encrypted(int2str(ydat));
	CryptoPP::Integer gxy = field.pow(gx,y);
	std::string C0 =chan->recv_encrypted();
	std::string C1 =chan->recv_encrypted();
	if(bit){
		return xorby(C1, int2str(gxy), 16);
	}else{
		return xorby(C0, int2str(gxy), 16);
	}
}


/*

Alice:
chooses random x
calculates g^x and sends

Bob:
receives g^x
chooses random y
if he chooses 0:
	sends g^y
if 1:
	sends g^(x+y)

A:
calculates either g^(xy) or g^(x(x+y))
encrypts message 0 with it and sends
calculates either g^(x(y-x)) or g^(xy)
encrypts message 1 with it and sends

B:
decrypts selected message with g^(xy)

*/
#endif