#include "network.h"
#include <string.h>

#define NODES 2

extern char *name;

struct Node nodes[] = {
    {NULL, 2, 3},
    {NULL, 4, 5},
};

void flood(struct Packet *p, char rxPort) {
  for (int i = 0; i < NODES; i++) {
    if (nodes[i].rxPort != rxPort) {
      if (tx(serialize(*p), nodes[i].txPort, nodes[i].rxPort)) {
        printErrMsg("flood", "transmit failed");
      }
    }
  }
}

/**
 * IMPORTANT: p WILL BE UNUSABLE AFTER THIS FUNCTION!!
 **/
void handle(struct Packet *p, char txPort, char rxPort) {
  struct Packet *answer;
  switch (p->type) {
  case DISCOVER: // send name back
    answer = malloc(sizeof(struct Packet));
    answer->sender = name;
    answer->receiver = p->sender;
    answer->type = SET_NAME;
  case SET_NAME: // set name in table
    nodes[txPort / 2 - 1].name = p->sender;
    break;
  case MSG:
    if (strcmp(p->sender, name)) {
      int i;
      for (i = 0; i < NODES; i++) {
        if (nodes[i].name && !strcmp(nodes[i].name, p->receiver)) {
          tx(serialize(*p), nodes[i].txPort, nodes[i].rxPort);
          break;
        }
      }
      if (i >= NODES)
        flood(p, rxPort);
    } else {
      char *ret;
      getString(p->body.data, 0, p->body.size, &ret);
      printf("%s from %s", ret, p->sender); // Serial.print
      free(ret);
    }
  default:
    printErrMsg("handle", "invalid type");
  }
  free(p->body.data);
  free(p);
}
