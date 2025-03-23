#include "bits.h"
#include "log.h"
// #include <malloc.h>
// #include <stdio.h>
#include <string.h>

// void printDataDebug(int *data, u_int64_t size) {
//   for (u_int64_t i = 0; i < size; i++) {
//     printf("%c", getBitRaw(data, size - 1 - i) ? '1' : '0');
//   }
//   printf("%c", '\n');
// }
void printDataDebug(int *data, u_int64_t size) {
  for (u_int64_t i = 0; i < size; i++) {
    printBit(getBitRaw(data, size - 1 - i) ? '1' : '0');
  }
  printBit('\n');
}

void printAreaDebug(const int *data, const ulong start, const ulong end) {
  for (ulong i = 0; i < end-start; i++) {
    printBit(getBitRaw(data, end-1-i) + '0');
  }
}

inline char getBitRaw(int *data, u_int64_t i) {
  return (data[i / BITS_IN_CONTAINER] >> (i % BITS_IN_CONTAINER)) & 1;
}
char setBitRaw(int *data, u_int64_t i, char val) {
  if (!val) {
    data[i / BITS_IN_CONTAINER] &= -1 ^ (1 << i % BITS_IN_CONTAINER);
  } else
    data[i / BITS_IN_CONTAINER] |=
        (val & 1) // we just take the LSB to avoid unneeded behavior
        << (i % BITS_IN_CONTAINER);
  return 0;
}

/**
 * ret size needs to be (end - start) / BITS_IN_CONTAINER + 1
 **/
void getBitArea(int *data, u_int64_t start, u_int64_t end, int *ret) {
  if (start > end) {
    printErrMsg("getBitArea", "invalid params");
    return;
  }
  // int *ret = malloc((end - start) / BITS_IN_CONTAINER + 1);
  //  if (!ret) {
  //    printErrMsg("getBitArea: ", ALLOC_ERR_MSG);
  //    return NULL;
  //  }
  for (u_int64_t i = start; i < end; i++) {
    setBitRaw(ret, i - start, getBitRaw(data, i));
  }
}

char getChar(int *data, u_int64_t index, u_int64_t size) {
  char ret = 0;
  // if (index + 7 >= size)
  //   return -1;
  for (unsigned char i = 0; i < 8; i++) {
    //logMsgF("getChar", LOG_INFO, "%l %l", (index+i)/BITS_IN_CONTAINER, (index+i)%BITS_IN_CONTAINER);
    ret |= (getBitRaw(data, index+i)) << i;
  }
  // logMsgF("getChar", LOG_INFO, "indices %l-%l", index, index+8);
  // printAreaDebug(data, index, index+8);
  // printBit(' ');
  printBit(ret);
  return ret;
}

void setChar(int *data, u_int64_t start, char val) {
  if (!data) return;
  for (int i = 0; i < 8; i++) {
    setBitRaw(data, start+i, (val >> (7-i)) & 1);
  }
}

void nullChar(int *data, u_int64_t start) {
  setChar(data, start, 0);
}

/**
* returns the bits [start;end) as a string
**/
char *substr(const int *data, u_int64_t start, u_int64_t end, u_int64_t size) {
  if (end < start || (end - start) % 8 != 0 || end > size) {
    printErrMsg("substr", "invalid indices!");
    return NULL;
  }
  printDataDebug(data, size);
  char *ret = malloc((end - start) / 8 + 1); // THIS STUPID CALL DELETES BITS 32-36
  if (!ret) {
    printErrMsg("substr", "alloc ret buf");
    return NULL;
  }
  printDataDebug(data, size);
  logMsgF("substr", LOG_INFO, "alloc %l bytes", (end - start) / 8 + 1);
  memset(ret, 0, (end-start)/8 + 1);
  unsigned int retIdx = 0;

  for (u_int64_t i = start; i < end; i += 8) {
    logMsgF("substr", LOG_INFO, "%d: ", i);
    ret[retIdx++] = getChar(data, i, size);
    printBit('\n');
    logMsgF("substr", LOG_INFO, "(%d)", ret[i/8]);
  }
  printDataDebug(data, size);
  for (int i = 0; i < (end-start)/8; i++) printBit(ret[i]);
  ret[retIdx] = '\0';
  logMsgF("substr", LOG_INFO, "res: %s", ret);
  return ret;
}

u_int64_t getString(const int *data, const u_int64_t start, const u_int64_t size, char **ret) {
  if (start >= size) {
    printErrMsg("getString", "invalid msg");
    return -1;
  }
  u_int64_t end_idx = start;
  while ((end_idx-start) < size && getChar(data, end_idx, size)) {
    end_idx += 8;
  }
  *ret = substr(data, start, end_idx, size);
  return end_idx;
}

int *strToBits(const char *str, u_int64_t *sizeInBits) {
  const unsigned int len = strlen(str);
  *sizeInBits = len*8 + 8;
  int *ret = malloc((*sizeInBits / BITS_IN_CONTAINER + 1) * sizeof(int));
  if (ret) {
    memset(ret, 0, (*sizeInBits / BITS_IN_CONTAINER + 1) * sizeof(int));
    for (int i = 0; i < *sizeInBits; i++) {
      setBitRaw(ret, i, str[i / 8] >> (i % 8) & 1);
    }
    nullChar(ret, *sizeInBits);
  }
  return ret;
}

void reverse(int *data, u_int64_t size) {
  char tmp;
  if (data)
    for (u_int64_t i = 0; i < size / 2; i++) {
      tmp = getBitRaw(data, i);
      setBitRaw(data, i, getBitRaw(data, size - 1 - i));
      setBitRaw(data, size - 1 - i, tmp);
    }
}
