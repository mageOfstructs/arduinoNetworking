#include "log.h"

#ifndef u_int64_t
#define u_int64_t unsigned long long
#endif
#define BITS_IN_CONTAINER (sizeof(int) * 8)

char getBitRaw(int *data, u_int64_t i);
char setBitRaw(int *data, u_int64_t i, char val);
void getBitArea(int *data, u_int64_t start, u_int64_t end, int *ret);
char getChar(int *data, u_int64_t index, u_int64_t size);
char *substr(const int *data, u_int64_t start, u_int64_t end, u_int64_t size);
u_int64_t getString(const int *data, const u_int64_t start, const u_int64_t size, char **ret);
void printDataDebug(int *data, u_int64_t size);
void printAreaDebug(const int *data, const ulong start, const ulong end);
int *strToBits(const char *str, u_int64_t *sizeInBits);
void reverse(int *data, u_int64_t size);
void setChar(int *data, u_int64_t start, char val);
void nullChar(int *data, u_int64_t start);