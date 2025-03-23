#ifndef ulong
#define ulong unsigned long long
#endif

#define ALLOC_ERR_MSG "alloc"
#define RESET_MSG "pls reset"

#define LOG_INFO "info"
#define LOG_WARN "warn"
#define LOG_ERR "err"

void logMsg(char *fname, char *type, char *msg);
void logMsgF(char *fname, char *type, char *msg, ...);
void displayMsg(char *sender, char *msg);
void printErrMsg(char *fname, char *msg);
void printBit(char bit);
void writeWrapper(char val);
void ok(char *fname);