#include "bits.h"
#include "log.h"
#include <sys/types.h>
#define DELAY 100
#define START_CAPACITY 20
#define GROWTH 1.2

struct SerialData {
  u_int64_t size;
  int *data;
};

char checkForTX(int rxPort);
struct SerialData *rx(int rxPort, int txPort);

char tx(struct SerialData *data, int txPort, int rxPort);
