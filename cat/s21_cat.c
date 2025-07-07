#include "s21_cat.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
  Flags flags = parseFlags(argc, argv);
  catFiles(&flags, argc, argv);
  return 0;
}

void setFlag(char opt, Flags* flags) {
  switch (opt) {
    case 'b':
      flags->b = 1;
      break;
    case 'v':
      flags->v = 1;
      break;
    case 'E':
      flags->E = 1;
      break;
    case 'e':
      flags->E = 1;
      flags->v = 1;
      break;
    case 'n':
      flags->n = 1;
      break;
    case 't':
      flags->T = 1;
      break;
    case 'T':
      flags->T = 1;
      flags->v = 1;
      break;
    case 's':
      flags->s = 1;
      break;
    case '?':
      exit(EXIT_FAILURE);
      break;
    default:
      exit(EXIT_FAILURE);
  }
}

Flags parseFlags(int argc, char** argv) {
  Flags flags = {0};
  int opt;
  static struct option long_options[] = {
      {"number-nonblank", no_argument, NULL, 'b'},
      {"number", no_argument, NULL, 'n'},
      {"squeeze-blank", no_argument, NULL, 's'},
      {0, 0, 0, 0}};
  while ((opt = getopt_long(argc, argv, "bvEentTs", long_options, NULL)) !=
         -1) {
    setFlag(opt, &flags);
  }
  if (optind >= argc) {
    fprintf(stderr, "s21_cat: Too few args\n");
    exit(EXIT_FAILURE);
  }
  return flags;
}

void handleNubmeredNonBlank(char* line, int* lineNumber) {
  if (strcmp(line, "\n") == 0) {
    printf("%s", line);
  } else {
    printf("%6d\t", (*lineNumber)++);
    printf("%s", line);
  }
}

void handleNumbered(char* line, int* lineNumber) {
  printf("%6d\t", (*lineNumber)++);
  printf("%s", line);
}

void handleSqueezeBlank(char* line, bool* prevIsBlank) {
  if (strcmp(line, "\n") == 0 && *prevIsBlank) {
    return;
  } else {
    printf("%s", line);
    *prevIsBlank = 0;
  }
  if (strcmp(line, "\n") == 0) *prevIsBlank = 1;
}

void handleShowInvisible(char* line) {
  for (int i = 0; line[i] != '\0'; i++) {
    if (line[i] > 0 && line[i] < 32 && line[i] != '\t' && line[i] != '\n') {
      putchar('^');
      putchar(line[i] + 64);
    } else if (line[i] == 127) {
      putchar('^');
      putchar('?');
    } else {
      putchar(line[i]);
    }
  }
}

void handleShowTabs(char* line) {
  for (int i = 0; line[i] != '\0'; i++) {
    if (line[i] == '\t') {
      putchar('^');
      putchar('I');
    } else {
      putchar(line[i]);
    }
  }
}

void handleShowTabsAndInvisible(char* line) {
  for (int i = 0; line[i] != '\0'; i++) {
    if (line[i] == '\t') {
      putchar('^');
      putchar('I');
    } else if (line[i] > 0 && line[i] < 32 && line[i] != '\t' &&
               line[i] != '\n') {
      putchar('^');
      putchar(line[i] + 64);
    } else if (line[i] == 127) {
      putchar('^');
      putchar('?');
    } else {
      putchar(line[i]);
    }
  }
}

void handleShowEOF(char* line) {
  for (int i = 0; line[i] != '\0'; i++) {
    if (line[i] == '\n') {
      putchar('$');
      putchar('\n');
    } else {
      putchar(line[i]);
    }
  }
}

void handleShowEOFAndInvisible(char* line) {
  for (int i = 0; line[i] != '\0'; i++) {
    if (line[i] == '\n') {
      putchar('$');
      putchar('\n');
    } else if (line[i] > 0 && line[i] < 32 && line[i] != '\t' &&
               line[i] != '\n') {
      putchar('^');
      putchar(line[i] + 64);
    } else if (line[i] == 127) {
      putchar('^');
      putchar('?');
    } else {
      putchar(line[i]);
    }
  }
}

void printLineWithFlags(Flags* flags, char* buffer, int* lineNumber,
                        bool* prevIsBlank) {
  if (flags->b)
    handleNubmeredNonBlank(buffer, lineNumber);
  else if (flags->n)
    handleNumbered(buffer, lineNumber);
  else if (flags->E && flags->v)
    handleShowEOFAndInvisible(buffer);
  else if (flags->T && flags->v)
    handleShowTabs(buffer);
  else if (flags->E)
    handleShowEOF(buffer);
  else if (flags->T)
    handleShowTabs(buffer);
  else if (flags->s)
    handleSqueezeBlank(buffer, prevIsBlank);
  else if (flags->v)
    handleShowInvisible(buffer);
  else {
    printf("%s", buffer);
  }
}

void catFile(Flags* flags, const char* filename, bool* prevIsBlank) {
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "s21_cat: Error opening file: %s\n", filename);
  } else {
    char buffer[BUFFER_SIZE];
    int lineNumber = 1;
    while (fgets(buffer, sizeof(buffer), file))
      printLineWithFlags(flags, buffer, &lineNumber, prevIsBlank);
  }
  fclose(file);
}

void catFiles(Flags* flags, int argc, char** argv) {
  bool prevIsBlank = false;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      catFile(flags, argv[i], &prevIsBlank);
    }
  }
}
