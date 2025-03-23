#include "protocol.h"
#include "string.h"
#include "log.h"

#define NODES 2

struct Node {
  char *name;
  char txPort;
  char rxPort;
};

extern char *name; // pls no break
extern struct Node nodes[];

int setName(const char *name);
void getPorts(char ret[]);
char getPortsFromNodeIdx(int i, char *retArr);
void handle(struct Packet *p, char txPort, char rxPort);
void disc();
void send(char *recv, char *msg);