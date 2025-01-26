#include "protocol.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

/**
 * IMPORTANT: sdata WILL BE UNUSABLE AFTER CALLING THIS FUNCTION
 **/
struct Packet *parse(struct SerialData sdata) {
  // if (sdata->size < )
  printf("Serial data: ");
  printDataDebug(sdata.data, sdata.size);
  struct Packet *ret = malloc(sizeof(struct Packet));
  if (!ret) {
    printErrMsg("parse: ", ALLOC_ERR_MSG);
    return NULL;
  }

  int *tmp = getBitArea(sdata.data, 0, 4);
  ret->type = *tmp;
  free(tmp);

  ret->sender = malloc(10 * sizeof(char));
  if (!ret->sender) {
    printErrMsg("parse:", "Sender allocation failed!\n");
    free(ret);
    return NULL;
  }
  ret->receiver = malloc(10 * sizeof(char));
  if (!ret->receiver) {
    printErrMsg("parse:", "Receiver allocation failed!\n");
    free(ret->sender);
    free(ret);
    return NULL;
  }
  u_int64_t receiver_start =
      getString(sdata.data, 4, sdata.size, &ret->sender) + 8;
  // printf("Sender? %s\n", ret->sender);
  printDataDebug(getBitArea(sdata.data, receiver_start, sdata.size),
                 sdata.size - receiver_start);

  u_int64_t bodyStart =
      getString(sdata.data, receiver_start, sdata.size, &ret->receiver);
  printDataDebug(getBitArea(sdata.data, bodyStart, sdata.size),
                 sdata.size - bodyStart);

  ret->body.size = sdata.size - bodyStart;
  ret->body.data = getBitArea(sdata.data, bodyStart, sdata.size);
  if (!ret->body.data) { // TODO: more error handling
    printErrMsg("parse", "Body allocation failed!");
    free(ret->sender);
    free(ret->receiver);
    free(ret);
    return NULL;
  }

  free(sdata.data);
  return ret;
}

void printStringBinary(char *str) {
  for (int i = strlen(str) * 8 - 1; i > -1; i--) {
    // if (i % 8 == 0)
    //   printf(" ");
    printf("%c", str[i / 8] >> i % 8 & 1 ? '1' : '0');
  }
  printf("\n");
}

/**
 * IMPORTANT: p will be unusable after this!!!
 **/
struct SerialData *serialize(struct Packet p) {
  struct SerialData *ret = malloc(sizeof(struct SerialData));
  if (!ret) {
    printErrMsg("serialize", "failed to allocate serial data buffer");
    return NULL;
  }
  u_int32_t sizeSender = strlen(p.sender), sizeReceiver = strlen(p.receiver);
  ret->size = sizeSender * 8 + sizeReceiver * 8 + p.body.size + 8;
  ret->data = malloc(ret->size);
  if (!ret->data) {
    printErrMsg("serialize", "failed to allocate data buffer!");
    free(ret);
    return NULL;
  }

  // for (int i = 0; i < 4; i++) {
  //   setBitRaw(ret->data, i, (p.type >> i) & 1);
  // }
  ret->data[0] = p.type;
  for (int i = 0; i < sizeSender * 8; i++) {
    setBitRaw(ret->data, i + 4, (p.sender[i / 8] >> (i % 8)) & 1);
  }
  printStringBinary(p.sender);
  printDataDebug(ret->data, ret->size);
  for (int i = 0; i < sizeReceiver * 8; i++) {
    setBitRaw(ret->data, i + sizeSender * 8 + 12,
              (p.receiver[i / 8] >> i % 8) & 1);
  }
  printDataDebug(ret->data, ret->size);
  for (int i = 0; i < p.body.size; i++) {
    setBitRaw(ret->data, i + 12 + sizeSender * 8 + sizeReceiver * 8,
              (p.body.data[i / BITS_IN_CONTAINER] >> i % BITS_IN_CONTAINER) &
                  1);
  }
  printDataDebug(ret->data, ret->size);

  free(p.body.data);
  return ret;
}
