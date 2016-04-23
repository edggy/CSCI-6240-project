#include <unistd.h>

// send command
ssize_t cwrite(int fd, const void *buf, size_t count);

// recv command
ssize_t cread(int fd, void *buf, size_t count);

// Does binary xor between 2 strings
char* xorCharArray(char* output, const char* first, const char* second, unsigned long long size);

// Does binary and between 2 strings
char* andCharArray(char* output, const char* first, const char* second, unsigned long long size);

// Does binary or between 2 strings
char* orCharArray(char* output, const char* first, const char* second, unsigned long long size);

// Concatinates the left char* to the right one
char* concatCharArray(char* output, const char* left, const char* right, unsigned long long sizel, unsigned long long sizer);

// Splits a charArray into two, left = charArray[0:splitPoint], right = charArray[splitPoint:size]
char* splitCharArray(char* left, char* right, const char* charArray, unsigned long long splitPoint, unsigned long long size);

char* getBytes(char* charArray, unsigned long long size);

char* getRandomData(char* output, unsigned long long size);
	
	