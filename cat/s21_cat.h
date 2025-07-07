#ifndef _CAT_H_
#define _CAT_H_

#include <stdbool.h>

#define BUFFER_SIZE 4000

typedef struct {
  bool b;
  bool E;
  bool n;
  bool s;
  bool T;
  bool v;
} Flags;

void setFlag(char opt, Flags* flags);

Flags parseFlags(int argc, char** argv);

void handleNubmeredNonBlank(char* line, int* lineNumber);

void handleNumbered(char* line, int* lineNumber);

void handleSqueezeBlank(char* line, bool* prevIsBlank);

void handleShowInvisible(char* line);

void handleShowTabs(char* line);

void handleShowTabsAndInvisible(char* line);

void handleShowEOF(char* line);

void handleShowEOFAndInvisible(char* line);

void printLineWithFlags(Flags* flags, char* buffer, int* lineNumber,
                        bool* prevIsBlank);

void catFile(Flags* flags, const char* filename, bool* prevIsBlank);

void catFiles(Flags* flags, int argc, char** argv);

#endif