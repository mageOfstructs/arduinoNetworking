#include "bits.h"
#include "log.h"
#include "serial.h"

const static char *PARSE_FUNC = "parse";

enum TYPES { DISCOVER = 0x0, SET_NAME = 0x1, MSG = 0x2, NULL_TYPE=0xf };

struct Packet {
  enum TYPES type;
  char *sender;
  char *receiver;
  struct SerialData body;
};

extern const struct Packet NULL_PACKET;

struct Packet parse(struct SerialData sdata);
struct SerialData serialize(struct Packet p);
void freePacket(struct Packet *p);