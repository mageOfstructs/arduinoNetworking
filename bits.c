#include "bits.h"
#include "log.h"
#include <malloc.h>
#include <stdio.h>
#include <sys/types.h>

void printDataDebug(int *data, u_int64_t size) {
  for (u_int64_t i = 0; i < size; i++) {
    printf("%c", (getBitRaw(data, size - 1 - i) ? '1' : '0'));
  }
  printf("\n");
}
char getBitRaw(int *data, u_int64_t i) {
  return data[i / BITS_IN_CONTAINER] >> (i % BITS_IN_CONTAINER) & 1;
}
char setBitRaw(int *data, u_int64_t i, char val) {
  data[i / BITS_IN_CONTAINER] |=
      (val & 1) // we just take the LSB to avoid unneeded behavior
      << (i % BITS_IN_CONTAINER);
  return 0;
}
int *getBitArea(int *data, u_int64_t start, u_int64_t end) {
  if (start > end) {
    return NULL;
  }
  int *ret = malloc((end - start) / BITS_IN_CONTAINER + 1);
  if (!ret) {
    printErrMsg("getBitArea: ", ALLOC_ERR_MSG);
    return NULL;
  }
  for (u_int64_t i = start; i < end; i++) {
    setBitRaw(ret, i - start, getBitRaw(data, i));
  }
  return ret;
}

char getChar(int *data, u_int64_t index, u_int64_t size) {
  char ret = 0;
  if (index + 7 >= size)
    return -1;
  for (int i = 0; i < sizeof(char) * 8; i++) {
    ret |= getBitRaw(data, index + i) << i;
  }
  return ret;
}

char *substr(int *data, u_int64_t start, u_int64_t end, u_int64_t size) {
  if ((end - start) % 8 != 0) {
    printErrMsg("substr", "invalid indices!");
    return NULL;
  }
  char *ret = malloc(sizeof(char) * ((end - start) / 8 + 1));
  if (!ret) {
    printErrMsg("substr", "failed to allocate return buffer");
    return NULL;
  }
  for (u_int64_t i = start; i < end; i += 8) {
    ret[(i - start) / 8] = getChar(data, i, size);
    if (ret[(i - start) / 8] == -1) {
      printf("substr: Error for char at bits %lu-%lu\n", i, i + 8);
    }
  }
  ret[(end - start) / 8] = '\0';
  return ret;
}

u_int64_t getString(int *data, u_int64_t start, u_int64_t size, char **ret) {
  if (start >= size) {
    printErrMsg("getString", "invalid message");
    return -1;
  }
  u_int64_t end_idx = start, ret_idx = 0;
  char tmp = getChar(data, end_idx, size);
  while (end_idx < size && tmp && tmp != -1) {
    // printf("%c ", tmp);
    // ret[ret_idx++] = tmp;
    end_idx += 8;
    tmp = getChar(data, end_idx, size);
  }
  *ret = substr(data, start, end_idx, size);
  // ret[ret_idx] = '\0';
  // ret = substr(data, start, end_idx, size);
  return end_idx;
}
