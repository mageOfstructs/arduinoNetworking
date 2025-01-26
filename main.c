// Test file for Network/Transport layer

#include "bits.h"
#include "protocol.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

// void printDataDebug(int *data, u_int64_t size) {
//   for (u_int64_t i = 0; i < size; i++) {
//     printf("%c", (getBitRaw(data, size - 1 - i) ? '1' : '0'));
//     // printf("%c", data[i / BITS_IN_CONTAINER] | (1 << (i %
//     BITS_IN_CONTAINER))
//     //                  ? '1'
//     //                  : '0');
//   }
//   printf("\n");
// }

int main(int argc, char *argv[]) {
  u_int64_t testDataSize = 3 * BITS_IN_CONTAINER;
  int *testData = malloc(testDataSize / 8);
  memset(testData, 0, testDataSize / 8);
  char *name = "Helo";
  *testData = 1;
  for (int i = 0; i < 5 * 8; i++) {
    setBitRaw(testData, i + 4, ((name[i / 8]) >> (i % 8)) & 1);
    setBitRaw(testData, i + 4 + 5 * 8, ((name[i / 8]) >> (i % 8)) & 1);
  }
  // printData(testData, 10 * BITS_IN_CONTAINER);
  // char *testStr = malloc(5 * sizeof(char));
  // int len = getString(testData, 4, 10 * BITS_IN_CONTAINER, testStr);
  // if (testStr) {
  //   printf("Sender: %s\n", testStr);
  //   free(testStr);
  // }
  struct SerialData sdata = {testDataSize, testData};
  struct Packet *p = parse(sdata);
  printf("Sender: %s\n", p->sender);
  printf("Receiver: %s\n", p->receiver);

  struct SerialData *testSerialize = serialize(*p);
  printf("Compare: \n");
  printDataDebug(testData, sdata.size);
  printDataDebug(testSerialize->data, testSerialize->size);
  free(p);
  p = parse(*testSerialize);
  printf("Sender: %s\n", p->sender);
  printf("Receiver: %s\n", p->receiver);

  free(p);
  return 0;
}
