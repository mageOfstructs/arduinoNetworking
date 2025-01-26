#include <sys/types.h>
#define BITS_IN_CONTAINER (sizeof(int) * 8)

char getBitRaw(int *data, u_int64_t i);
char setBitRaw(int *data, u_int64_t i, char val);
int *getBitArea(int *data, u_int64_t start, u_int64_t end);
char getChar(int *data, u_int64_t index, u_int64_t size);
char *substr(int *data, u_int64_t start, u_int64_t end, u_int64_t size);
u_int64_t getString(int *data, u_int64_t start, u_int64_t size, char **ret);
void printDataDebug(int *data, u_int64_t size);
