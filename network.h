#include "malloc.h"
#include "protocol.h"
#include "string.h"

struct Node {
  char *name;
  char txPort;
  char rxPort;
};

void handle(struct Packet *p, char txPort, char rxPort);
