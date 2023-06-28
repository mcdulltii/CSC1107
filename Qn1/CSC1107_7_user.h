#include "structs.h"

#ifndef CSC1107_7_USER
#define CSC1107_7_USER

char* getCurrentDateTime();
void sha512_encode(const char* input, unsigned char* output);
void sha384_encode(const char* input, unsigned char* output);
void sha256_encode(const char* input, unsigned char* output);
void sha1_encode(const char* input, unsigned char* output);
void md5_encode(const char* input, unsigned char* output);

#endif
