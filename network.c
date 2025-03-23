#include "network.h"
#include <string.h>

char *name = NULL;
unsigned int nameLen = 0;

struct Node nodes[] = {
    {NULL, 2, 3},
    {NULL, 4, 5},
};

const struct SerialData EMPTY_BODY = {NULL, NULL};

/**
* finds the corresponding ports to the given name
* returns: 0, on success (ports will be in placed in rx,tx order in ret)
*          1, if not found
**/
char getPortsFromName(char *name, char ret[]) {
  for (int i = 0; i < NODES; i++) {
    if (!strcmp(name, nodes[i].name)) {
      ret[0] = nodes[i].rxPort;
      ret[1] = nodes[i].txPort;
      return 0;
    }
  }
  return 1;
}

char getPortsFromNodeIdx(int i, char *retArr) {
  if (i >= NODES) return -1;
  retArr[0] = nodes[i].rxPort;
  retArr[1] = nodes[i].txPort;
  return 0;
}

void getPorts(char ret[]) {
  for (int i = 0; i < NODES; i++) {
    ret[i] = nodes[i].rxPort;
  }
}

int setName(const char *newName) {
  if (!newName || !*newName) {
    return -1;
  }
  
  if (name) free(name);
  nameLen = strlen(newName);
  name = strcpy(malloc(nameLen), newName);
  return 0;
}

void flood(struct Packet p, char rxPort) {
  struct SerialData sdata = serialize(p);
  for (int i = 0; i < NODES; i++) {
    if (nodes[i].rxPort != rxPort) {
      if (tx(sdata, nodes[i].txPort, nodes[i].rxPort)) {
        printErrMsg("flood", "tx");
      }
    }
  }
  free(sdata.data);
}

char *copyName() {
  return strcpy(malloc(nameLen), name);
}

void disc() {
  struct Packet p = {
    DISCOVER,
    name,
    NULL,
    EMPTY_BODY
  };
  // if (!p) {
  //   printErrMsg("disc", "packet alloc fail");
  //   return;
  // }
  struct SerialData sdata = serialize(p);
  logMsg("disc", LOG_INFO, "data:");
  printDataDebug(sdata.data, sdata.size);
  if (!sdata.size || !sdata.data) {
    printErrMsg("disc", "serialize");
    return;
  }
  // if (!sdata) { // means serialize probably didn't get to freeing the packet
  //   freePacket(p"all"); // hope this doesn't end up causing a double-free
  //   return;
  // }
  for (int i = 0; i < NODES; i++) {
    if (!nodes[i].name) {
      tx(sdata, nodes[i].txPort, nodes[i].rxPort);
      digitalWrite(nodes[i].txPort, LOW); // hopefully this is a catch-all
      struct SerialData resp;
      unsigned long start = millis();
      while (millis()-start < TIMEOUT)
        if (!rx(nodes[i].txPort, nodes[i].rxPort, &resp)) {
          struct Packet respPacket = parse(resp);
          free(resp.data);
          handle(&respPacket, nodes[i].txPort, nodes[i].rxPort);
          freePacket(&respPacket);
          break;
        }
    }
  }
  free(sdata.data);
  ok("disc");
}

void send(char *recv, char *msg) {
  if (!recv || !msg) {
    printErrMsg("send", "invalid args");
    return;
  }
  logMsgF("send", LOG_INFO, "m: %s r: %s", msg, recv);
  struct Packet p = {
    MSG,
    name,
    recv,
    {0, strToBits(msg, &p.body.size)}
  };
  if (!p.body.data) {
    printErrMsg("send", "body alloc");
    return;
  }
  logMsgF("send", LOG_INFO, "size: %d", p.body.size);
  printDataDebug(p.body.data, p.body.size);
  // p->type = MSG;
  // p->sender = copyName(); // the stupid thing gets freed
  // p->receiver = strcpy(malloc(strlen(recv)), recv);
  // struct SerialData tmp = {
  //   0, // filler
  //   strToBits(msg, &tmp.size)
  // };
  // p->body = tmp;
  char ports[2];
  if (getPortsFromName(recv, ports)) {
    flood(p, -1);
  } else {
    struct SerialData sdata = serialize(p);
    if (tx(sdata, ports[1], ports[0]))
      printErrMsg("send", "tx");
    free(sdata.data);
  }
  free(p.body.data);
}

void updateNeighbour(char *sender, char txPort) {
  if (nodes[txPort / 2 - 1].name) free(nodes[txPort / 2 - 1].name);
  nodes[txPort / 2 - 1].name = strcpy(malloc(strlen(sender)), sender);
}

void handle(struct Packet *p, char txPort, char rxPort) {
  struct Packet answer;
  struct SerialData sdata;
  char txRet;
  switch (p->type) {
  case DISCOVER: // send name back
    // answer = malloc(sizeof(struct Packet));
    //logMsgF("handle", "info", "sending SET_NAME to port %d", txPort);
    answer.sender = name;
    answer.receiver = p->sender;
    answer.type = SET_NAME;
    answer.body = EMPTY_BODY;
    sdata = serialize(answer);
    txRet = tx(sdata, txPort, rxPort);
    if (txRet)
      logMsgF("handle", LOG_ERR, "SET_NAME tx fail: %d", txRet);
    free(sdata.data);
    // don't need a break here, since we need to update our name as well
  case SET_NAME: // set name in table
    updateNeighbour(p->sender, txPort);
    break;
  case MSG:
    if (strcmp(p->receiver, name)) {
      int i;
      for (i = 0; i < NODES; i++) {
        if (nodes[i].name && !strcmp(nodes[i].name, p->receiver)) {
          sdata = serialize(*p);
          txRet = tx(sdata, nodes[i].txPort, nodes[i].rxPort);
          if (txRet) logMsgF("handle", LOG_ERR, "MSG tx fail: %d", txRet);
          free(sdata.data);
          break;
        }
      }
      if (i >= NODES)
        flood(*p, rxPort);
    } else {
      char *ret;
      logMsgF("handle", LOG_INFO, "size of msg: %dB", (p->body.size)/8+1);
      //printDataDebug(p->body.data, p->body.size);
      if (getString(p->body.data, 0, p->body.size, &ret) != -1)
        displayMsg(p->sender, ret);
      free(ret);
    }
    break;
  default:
    printErrMsg("handle", "type");
  }
  digitalWrite(txPort, LOW); // hopefully this is a catch-all
  ok("handle");
}

void freeNode(struct Node n) {
  free(n.name);
}