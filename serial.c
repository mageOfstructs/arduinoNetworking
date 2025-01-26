#include "serial.h"
#include <malloc.h>
#include <stdio.h>
#include <sys/types.h>

char checkForTX(int rxPort) { return digitalRead(rxPort); }
char getBit(struct SerialData *data, u_int64_t i) {
  return getBitRaw(data->data, i);
}

struct SerialData *rx(int rxPort, int txPort) {
  struct SerialData *ret = malloc(sizeof(struct SerialData));
  if (!ret) {
    printErrMsg("rx: ", ALLOC_ERR_MSG);
    return NULL;
  }
  u_int64_t cap = START_CAPACITY;
  ret->data = malloc(cap);
  ret->size = 0;
  u_int64_t index = 0;
  char cur_bit, insertBit = 0;

  digitalWrite(txPort, HIGH); // tell them we're ready
  char prev_bit = digitalRead(rxPort);
  for (;;) {
    delay(DELAY);
    cur_bit = digitalRead(rxPort);
    if (cur_bit == prev_bit && cur_bit == HIGH) {
      break;
    }
    if (insertBit) {
      if (ret->size >= cap) {
        cap = (u_int64_t)(cap * GROWTH);
        ret->data = realloc(ret->data, cap);
        if (!ret->data) {
          printErrMsg("rx: ", "failed to enlarge arr");
          return NULL;
        }
      }
      setBitRaw(ret->data, index++, cur_bit);
      ret->size++;
    }
    insertBit = !insertBit;
  }
  digitalWrite(txPort, LOW);
  return ret;
}

char tx(struct SerialData *data, int txPort, int rxPort) {
  digitalWrite(txPort, HIGH);
  while (!digitalRead(rxPort))
    ;
  for (int i = 0; i < data->size; i++) {
    delay(DELAY);
    digitalWrite(txPort, LOW);
    delay(DELAY);
    digitalWrite(txPort, getBit(data, i));
  }
  free(data);
  return 0;
}
