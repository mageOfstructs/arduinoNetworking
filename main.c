// #include "bits.h"
// #include <malloc.h>
// #include <stdio.h>

// int main(void) {
//   int *data = malloc(sizeof(int));
//   const int size = sizeof(int) * 8;
//   *data = 395457;
//   printDataDebug(data, size);
//   reverse(data, size);
//   printDataDebug(data, size);
//   free(data);
//   return 0;
// }

// int main(void) {
//   const char *str = "test";
//   const unsigned int len = strlen(str);
//   const unsigned int cap = (len / 4 + 8) + ((len % 4 + 8) != 0 ? 1 : 0);
//   int *data = strToBits(str);
//   printDataDebug(data, 40);
//   char *str_converted;
//   u_int64_t end = getString(data, 0, 40, &str_converted);
//   printf("%s", str_converted);
//   return 0;
// }

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

// int main(int argc, char *argv[]) {
//   u_int64_t testDataSize = 3 * BITS_IN_CONTAINER;
//   int *testData = malloc(testDataSize / 8);
//   memset(testData, 0, testDataSize / 8);
//   char *name = "Helo";
//   *testData = 1;
//   for (int i = 0; i < 5 * 8; i++) {
//     setBitRaw(testData, i + 4, ((name[i / 8]) >> (i % 8)) & 1);
//     setBitRaw(testData, i + 4 + 5 * 8, ((name[i / 8]) >> (i % 8)) & 1);
//   }
//   // printData(testData, 10 * BITS_IN_CONTAINER);
//   // char *testStr = malloc(5 * sizeof(char));
//   // int len = getString(testData, 4, 10 * BITS_IN_CONTAINER, testStr);
//   // if (testStr) {
//   //   printf("Sender: %s\n", testStr);
//   //   free(testStr);
//   // }
//   struct SerialData sdata = {testDataSize, testData};
//   struct Packet *p = parse(sdata);
//   printf("Sender: %s\n", p->sender);
//   printf("Receiver: %s\n", p->receiver);

//   struct SerialData *testSerialize = serialize(*p);
//   printf("Compare: \n");
//   printDataDebug(testData, sdata.size);
//   printDataDebug(testSerialize->data, testSerialize->size);
//   free(p);
//   p = parse(*testSerialize);
//   printf("Sender: %s\n", p->sender);
//   printf("Receiver: %s\n", p->receiver);

//   free(p);
//   return 0;
// }
