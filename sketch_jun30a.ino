extern "C" {
  #include "network.h"
  #include <string.h>
  #include <stdarg.h>
  #include "log.h"
}
#define ulong u_int64_t

const static char *SETUP_FUNC = "setup";
const static char *LOOP_FUNC = "loop";

/**
* waits for a character input and returns it
**/
char getChar() {
  char tmp = (char)Serial.read();
  while (tmp == -1) tmp = (char) Serial.read();
  return tmp;
}

int readStr(char **ret, char *prompt, ulong cap) {
  if (!ret || !*ret) return 1;
  ulong i = 0;
  char tmp;
  while (i < 1) {
    Serial.print(prompt);
    while ((tmp = getChar()) != '\n') {
      if (i >= cap) {
        *ret = realloc(*ret, ++cap); // wasteful, but at least conserves space
        if (!*ret) {
          printErrMsg("readStr", "realloc");
          return 2;
        }
      }
      (*ret)[i++] = tmp;
    }
  }
  if (i >= cap) {
    *ret = realloc(*ret, ++cap); // wasteful, but at least conserves space
    if (!*ret) {
      printErrMsg("readStr", "realloc");
      return 2;
    }
  }
  (*ret)[i] = '\0';
  return 0;
}

void begForReset() {
  Serial.println(RESET_MSG);
  for (;;);
}

char rxPorts[NODES];

void setup() {
  // put your setup code here, to run[NODES] once:
  Serial.begin(9600);

  for (int i = 2; i < NODES*2+2; i += 2) {
    pinMode(i, OUTPUT);
    pinMode(i+1, INPUT);
  }

  Serial.println("\n");
  ulong cap = 10;
  char* name = malloc(cap);
  if (!name) {
    printErrMsg(SETUP_FUNC, "alloc name"); // please reset
    begForReset();
  }
  
  do {
    readStr(&name, "name>", cap);
  } while (setName(name));
  free(name);

  Serial.println("\n{tx, rx}: {2, 3}, {4, 5}");
  getPorts(rxPorts);
}

char **split(const char del, const char *str, unsigned int *retSize) {
  const unsigned int strlenVal = strlen(str);
  *retSize = 1;
  unsigned int i = 0;
  for (; i < strlenVal; i++) {
    if (str[i] == del)
      (*retSize)++;
  }
  if (*retSize == 1) {
    char **ret = malloc(sizeof(char *));
    *ret = strcpy(malloc(strlenVal), str);
    return ret;
  }
  char **ret = malloc(*retSize * sizeof(char *));
  if (!ret) {
    return NULL;
  }
  unsigned int splitIndexes[*retSize - 1], splitArrIdx = 0;
  i = 0;
  while (splitArrIdx <= *retSize - 1) {
    if (str[i] == del || !str[i]) {
      splitIndexes[splitArrIdx++] = i;
    }
    i++;
  }
  splitArrIdx = 0;
  unsigned int strSize;
  for (i = 0; i < strlenVal; i++) {
    if (splitArrIdx == *retSize) {
      strSize = strlenVal - splitIndexes[splitArrIdx - 1] + 1;
    } else
      strSize = splitIndexes[splitArrIdx] -
                (splitArrIdx ? splitIndexes[splitArrIdx - 1] + 1 : 0) + 1;
    ret[splitArrIdx] = malloc(strSize);
    if (!ret[splitArrIdx])
      return NULL;
    memcpy(ret[splitArrIdx], str + i,
           splitIndexes[splitArrIdx] -
               (splitArrIdx ? splitIndexes[splitArrIdx - 1] : 0));
    ret[splitArrIdx][splitIndexes[splitArrIdx] - i] = '\0';
    i = splitIndexes[splitArrIdx++];
  }
  return ret;
}

void freeStringArr(char **arr, unsigned int size) {
  for (unsigned int i = 0; i < size; i++) {
    free(arr[i]);
  }
  free(arr);
}

struct SerialData sdata;
struct Packet p;
char *command = malloc(10);
char sendPrompt = 1;

void loop() {
  char rxRet;
  char portsPair[NODES]; // rx/tx
  if (sendPrompt) {
    Serial.print("\n>");
    sendPrompt = 0;
  }
  if (Serial.available()) {
    sendPrompt = 1;
    if (readStr(&command, "", 10)) {
      printErrMsg(LOOP_FUNC, "com read");
    } else {
      unsigned int argc;
      char **args = split(' ', command, &argc);
      if (!args) {
        printErrMsg(LOOP_FUNC, "com parse");
      } else {
        if (!strcmp("set_name", args[0])) {
          //logMsg(LOOP_FUNC, "info", "set_name com detected");
          if (argc < 2)
            printErrMsg(LOOP_FUNC, "missing name");
          else if (setName(args[1])) {
            printErrMsg(LOOP_FUNC, "set_name fail");
          } else ok(LOOP_FUNC);
        } else if (!strcmp("discover", args[0]))
          disc();
        else if (!strcmp("send", args[0])) {
          if (argc < 3) 
            printErrMsg(LOOP_FUNC, "missing msg/recv");
          else {
            send(args[1], args[2]);
          }
        } else if (!strcmp("stats", args[0])) {
          logMsgF(LOOP_FUNC, LOG_INFO, "Name: %s", name);
          logMsg(LOOP_FUNC, LOG_INFO, "displaying nodes...");
          for (int i = 0; i < NODES; i++) {
            logMsgF(LOOP_FUNC, LOG_INFO, "%s: %b(rx) %b(tx)", (nodes[i].name ? nodes[i].name : "NULL"), nodes[i].rxPort, nodes[i].txPort);
          }
        } else printErrMsg(LOOP_FUNC, "unknown com");
        for (int i = 0; i < argc; i++)
          free(args[i]);
        free(args);
      }
    }
  }
  for (int i = 0; i < NODES; i++) {
    rxRet = rx(nodes[i].txPort, nodes[i].rxPort, &sdata);
    digitalWrite(nodes[i].txPort, LOW); // hopefully this is a catch-all
    if (!rxRet) {
      p = parse(sdata);
      handle(&p, nodes[i].txPort, nodes[i].rxPort);
      digitalWrite(nodes[i].txPort, LOW); // hopefully this is a catch-all
      free(sdata.data);
      freePacket(&p);
      ok(LOOP_FUNC);
    } else if (rxRet != 2) {
      printErrMsg(LOOP_FUNC, "rx fail");
    }
  }
}