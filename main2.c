// #include <stdarg.h>
// #include <stdio.h>
// #include <string.h>
// #define ulong unsigned long long
//
// #define SETUP_FUNC "setup"
//
// void logBegin(char *fname, char *type) {
//   printf("%s", fname);
//   printf(" ");
//   printf("%s", type);
//   printf(": ");
// }
//
// void logMsgF(char *fname, char *type, char *msg, ...) {
//   va_list varArgs;
//   va_start(varArgs, msg);
//   logBegin(fname, type);
//
//   while (*msg) {
//     if (*msg == '%') {
//       // cool formatting stuff
//       switch (*(++msg)) {
//       case 'd':
//         printf("%d", va_arg(varArgs, int));
//         msg++; // not ideal
//         break;
//       case 'b':
//         printf("%d", (int)(va_arg(varArgs, char)));
//         msg++; // not ideal
//         break;
//       case 's':
//         printf("%s", va_arg(varArgs, char *));
//         msg++; // not ideal
//         break;
//       case 'l':
//         printf("%lu", (unsigned long)va_arg(varArgs, ulong)); // fine for now
//         msg++;                                                // not ideal
//         break;
//       default:
//         if (*msg)
//           printf("%c", *msg);
//         else
//           goto end;
//       }
//     } else
//       printf("%c", *msg);
//     msg++;
//   }
// end:;
//   printf("\n");
// }
//
// int main(int argc, char *argv[]) {
//   logMsgF("loop", "info", "%s: %d, %d", "test", 1, 4);
//   return 0;
// }
