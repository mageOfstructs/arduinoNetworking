#include <Arduino.h>
extern "C" {
  #include "log.h"
}
// #include <stdio.h>

extern "C" void logMsg(char *fname, char *type, char *msg);
extern "C" void logMsgF(char *fname, char *type, char *msg, ...);
extern "C" void displayMsg(char *sender, char *msg);
extern "C" void printErrMsg(char *fname, char *msg);
extern "C" void printBit(char bit);
extern "C" void writeWrapper(char val);
extern "C" void ok(char *fname);

// void printErrMsg(char *fname, char *msg) { printf("%s: err %s", fname, msg); }

extern "C" void writeWrapper(char val) {
  Serial.write(val);
}

extern "C" void logBegin(char *fname, char *type) {
  Serial.print(fname);
  Serial.print(" ");
  Serial.print(type);
  Serial.print(": ");
}

extern "C" void logMsg(char *fname, char *type, char *msg) {
  logBegin(fname, type);
  Serial.println(msg);
  Serial.flush();
}

extern "C" void logMsgF(char *fname, char *type, char *msg, ...) {
  va_list varArgs;
  va_start(varArgs, msg);
  logBegin(fname, type);
  
  while (*msg) {
    if (*msg == '%') {
      // cool formatting stuff
      msg++;
      Serial.flush();
      switch (*msg) {
        case 'd':
        case 'b': // deprecated
          Serial.print(va_arg(varArgs, int));
          break;
        case 's':
          Serial.print(va_arg(varArgs, char*));
          break;
        case 'c':
          Serial.print(va_arg(varArgs, char));
          break;
        case 'l':
          Serial.print((unsigned long) va_arg(varArgs, ulong)); // fine for now
          break;
        default:
          if (*msg)
            Serial.print(*msg);
          else {
            Serial.flush();
            return;
          }
      }
    } else Serial.print(*msg);
    msg++;
    Serial.flush();
  }
  Serial.print('\n');
  Serial.flush();
}

extern "C" void printBit(char bit) {
  Serial.print(bit);
}

extern "C" void ok(char *fname) {
  logMsg(fname, ":ok", "");
}

extern "C" void displayMsg(char *sender, char *msg) {
  Serial.print(msg);
  Serial.print(" from ");
  Serial.println(sender);
}

extern "C" void printErrMsg(char *fname, char *msg) { 
  logMsg(fname, LOG_ERR, msg);
}