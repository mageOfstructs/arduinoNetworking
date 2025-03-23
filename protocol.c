#include "protocol.h"
#include <stdio.h>
#include <string.h>

const struct Packet NULL_PACKET = {
  NULL_TYPE,
  0,
  0,
  {}
};

struct Packet parse(struct SerialData sdata) {
  logMsg(PARSE_FUNC, LOG_INFO, "data:");
  printDataDebug(sdata.data, sdata.size);
  struct Packet ret;

  int tmp[1] = {0};
  getBitArea(sdata.data, 0, 4, tmp);
  ret.type = *tmp;

  // ret.sender = malloc(10 * sizeof(char));
  // if (!ret.sender) {
  //   printErrMsg(PARSE_FUNC, "Sender alloc fail");
  //   return NULL_PACKET;
  // }
  // ret.receiver = malloc(10 * sizeof(char));
  // if (!ret.receiver) {
  //   printErrMsg(PARSE_FUNC, "Receiver alloc fail");
  //   free(ret.sender);
  //   return NULL_PACKET;
  // }
  u_int64_t receiver_start =
      getString(sdata.data, 4, sdata.size, &ret.sender) + 8;
  // printf("Sender? %s\n", ret->sender);

  if (receiver_start == -1) {
    printErrMsg(PARSE_FUNC, "sender fail");
    free(ret.sender);
    free(ret.receiver);
    return NULL_PACKET;
  }
  // causes mem leak
  // printDataDebug(getBitArea(sdata->data, receiver_start, sdata->size),
  //                sdata->size - receiver_start);

  u_int64_t bodyStart =
      getString(sdata.data, receiver_start, sdata.size, &ret.receiver) + 8; // make sure we skip the null byte (which is hopefully there in the first place)
  // causes mem leak
  // printDataDebug(getBitArea(sdata->data, bodyStart, sdata->size),
  //                sdata->size - bodyStart);
  if (bodyStart == -1) {
    printErrMsg(PARSE_FUNC, "receiver fail");
    free(ret.sender);
    free(ret.receiver);
    return NULL_PACKET;
  }

  ret.body.size = sdata.size - bodyStart;
  ret.body.data = malloc((sdata.size - bodyStart) / BITS_IN_CONTAINER + 1);
  if (!ret.body.data) {
    printErrMsg(PARSE_FUNC, "Body alloc fail");
    free(ret.sender);
    free(ret.receiver);
    return NULL_PACKET;
  }
  getBitArea(sdata.data, bodyStart, sdata.size, ret.body.data);

  logMsgF(PARSE_FUNC, LOG_INFO, "Type: %d", ret.type);
  logMsgF(PARSE_FUNC, LOG_INFO, "Sender: %s", ret.sender);
  logMsgF(PARSE_FUNC, LOG_INFO, "Recv: %s", ret.receiver);
  logMsg(PARSE_FUNC, LOG_INFO, "Body:");
  printDataDebug(ret.body.data, ret.body.size);

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

struct SerialData serialize(struct Packet p) {
  struct SerialData ret = {0, NULL};
  // if (!ret) {
  //   printErrMsg("serialize", "serial buf alloc fail");
  //   return NULL;
  // }
  const u_int64_t sizeSender = strlen(p.sender), sizeReceiver = strlen(p.receiver);
  ret.size = sizeSender * 8 + sizeReceiver * 8 + p.body.size + 8 + 8 + 4; // include null bytes and type
  if (ret.size <= 16) {
    logMsg("serialize", LOG_WARN, "empty packet");
    return NULL_DATA;
  }
  ret.data = malloc(ret.size / BITS_IN_CONTAINER + (ret.size % BITS_IN_CONTAINER == 0 ? 0 : 1));
  if (!ret.data) {
    printErrMsg("serialize", "data buf alloc fail");
    return NULL_DATA;
  }

  memset(ret.data, 0, ret.size / BITS_IN_CONTAINER + (ret.size % BITS_IN_CONTAINER == 0 ? 0 : 1));
  // for (int i = 0; i < 4; i++) {
  //   setBitRaw(ret->data, i, (p->type >> i) & 1);
  // }
  ret.data[0] = p.type;
  logMsg("serialize", LOG_INFO, "stream with type:");
  printDataDebug(ret.data, 4);
  unsigned int offset = 4;
  for (int i = 0; i < sizeSender * 8; i++) { // sender
    setBitRaw(ret.data, i + offset, (p.sender[i / 8] >> (i % 8)) & 1);
  }
  offset += sizeSender * 8;
  nullChar(ret.data, offset);
  offset += 8;
  // printStringBinary(p.sender);
  // printDataDebug(ret.data, ret->size);
  for (int i = 0; i < sizeReceiver * 8; i++) { // receiver
    setBitRaw(ret.data, i + offset,
              (p.receiver[i / 8] >> (i % 8)) & 1);
  }
  offset += sizeReceiver * 8;
  nullChar(ret.data, offset);
  offset += 8;
  // printDataDebug(ret->data, ret->size);
  for (int i = 0; i < p.body.size; i++) {
    setBitRaw(ret.data, i + offset,
              (p.body.data[i / BITS_IN_CONTAINER] >> i % BITS_IN_CONTAINER) &
                  1);
  }

  logMsgF("serialize", LOG_INFO, "Size: %d", ret.size);
  logMsg("serialize", LOG_INFO, "Data:");
  printDataDebug(ret.data, ret.size);
  return ret;
}

void freePacket(struct Packet *p) {
  free(p->receiver);
  free(p->sender);
  free(p->body.data);
  ok("freePacket");
}