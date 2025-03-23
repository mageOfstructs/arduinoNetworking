#include "/home/jason/.arduino15/packages/arduino/hardware/avr/1.8.6/cores/arduino/Arduino.h"
#include "bits.h"
#include "log.h"

#define SIZE_IN_BITS 16
#define SIZE_OF_CONTAINER (sizeof(int) * 8)

#define TIMEOUT 5000

struct SerialData {
  ulong size;
  int *data;
};

extern const struct SerialData NULL_DATA;

/**
 * transmits sdata over a serial connection
 * Note: This function will block execution until an error occurs or the data
 *has been trasmitted
 * txPort has to be in OUTPUT mode
 * rxPort has to be in INPUT mode
 * returns: 0 (if success)
 **/
char tx(struct SerialData sdata, char txPort, char rxPort);

/**
 * receives data over serial connection
 * non-blocking
 * returns: 0 (if success), 2 (if there is no tx request)
 **/
char rx(char txPort, char rxPort, struct SerialData *ret);
