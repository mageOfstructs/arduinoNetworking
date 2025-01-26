#include "bits.h"
#include "log.h"
#include "serial.h"
enum TYPES { DISCOVER = 0x0, SET_NAME = 0x1, MSG = 0x2 };

struct Packet {
  enum TYPES type;
  char *sender;
  char *receiver;
  struct SerialData body;
};

struct Packet *parse(struct SerialData sdata);
struct SerialData *serialize(struct Packet p);
