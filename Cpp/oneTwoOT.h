#include "field.h"

void otSend(const char* msg1, const char* msg2, unsigned long long length, unsigned long long generator, Field field);

char* otRecv(char* output, bool bit, unsigned long long length, unsigned long long generator, Field field);
