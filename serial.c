#include "serial.h"

const struct SerialData NULL_DATA = {0, NULL};

const unsigned long DELAY_SERIAL = ((1. / 8) * 1000);

inline char getBit(struct SerialData sdata, int i) {
  return getBitRaw(sdata.data, i);
}
char getBitFromInt(ulong sdata, int i) { return (sdata >> i) & 1; }

char sync(char txPort, char rxPort) {
  digitalWrite(txPort, HIGH);
  unsigned long i = millis();
  while (!digitalRead(rxPort) && millis() - i < TIMEOUT)
    ;
  digitalWrite(txPort, LOW);
  return millis() - i >= TIMEOUT;
}

char tx(struct SerialData sdata, char txPort, char rxPort) {
  logMsg("tx", LOG_INFO, "tx enter");
  printDataDebug(sdata.data, sdata.size);
  if (sync(txPort, rxPort)) {
    printErrMsg("tx", "timeout");
    return 1;
  }
  // logMsg("tx", LOG_INFO, "finish sync");
  for (int i = 0; i < SIZE_IN_BITS; i++) {
    digitalWrite(txPort, getBitFromInt(sdata.size, i));
    delay(DELAY_SERIAL);
  }
  delay(100);
  logMsg("tx", LOG_INFO, "finish size tx");
  if (sync(txPort, rxPort)) {
    printErrMsg("tx", "timeout2");
    return 2;
  }
  // delay(14);
  char tmp;
  for (unsigned int i = 0; i < sdata.size; i++) {
    // logMsgF("tx", LOG_INFO, "Tx write %d", getBit(sdata, i));
    tmp = getBit(sdata, i);
    digitalWrite(txPort, tmp);
    printBit(tmp + '0');
    delay(DELAY_SERIAL); // prev: 15
  }
  digitalWrite(txPort, LOW);
  logMsg("tx", LOG_INFO, "finish data tx");
  return 0;
}

char rx(char txPort, char rxPort, struct SerialData *ret) {
  if (!ret)
    return 1;
  if (!digitalRead(rxPort))
    return 2;
  logMsgF("rx", LOG_INFO, "rx on %d", rxPort);
  unsigned short size = 0;
  digitalWrite(txPort, HIGH);
  delay(10);
  for (int i = 0; i < SIZE_IN_BITS; i++) { // size tx
    size |= digitalRead(rxPort) << i;
    delay(DELAY_SERIAL);
  }
  digitalWrite(txPort, LOW);
  for (int i = 0; i < SIZE_IN_BITS; i++) {
    printBit((size >> i & 1) + '0');
  }
  if (!size) {
    logMsg("rx", LOG_WARN, "empty msg");
    return 3;
  }
  ret->size = size;
  logMsgF("rx", LOG_INFO, "%l", ret->size);
  int *data = malloc((size / SIZE_OF_CONTAINER + 1) * sizeof(int));
  if (!data) {
    return 4;
  }
  memset(data, 0, (size / SIZE_OF_CONTAINER + 1) * sizeof(int));
  digitalWrite(txPort, HIGH);
  // delay(16);
  delay(DELAY_SERIAL); // does this really have to be THAT high? If yes, change
                       // back to DELAY_SERIAL
  digitalWrite(txPort, LOW);
  for (unsigned char i = 0; i < ret->size; i++) { // data tx
    data[i / SIZE_OF_CONTAINER] |= (digitalRead(rxPort) & 1)
                                   << (i % SIZE_OF_CONTAINER);
    delay(DELAY_SERIAL);
  }
  digitalWrite(txPort, LOW);
  logMsg("rx", LOG_INFO, "res:");
  ret->data = data;
  printDataDebug(ret->data, ret->size);
  return 0;
}