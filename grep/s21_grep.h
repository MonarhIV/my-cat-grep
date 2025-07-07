#include <stdbool.h>

#define BUFFER_SIZE 4000

typedef struct {
  bool e;
  bool i;
  bool v;
  bool c;
  bool l;
  bool n;
  bool h;
  bool s;
  bool f;
  bool o;
  char **patterns;
  int patternsCount;
  char **files;
  int files_count;
} Params;

void freeParams(Params *params);

void handleErrorAndExit(Params *params, char *reason);

void handleErrorOpeningFileAndExit(Params *params, char *filename);

void parseFilePatterns(Params *params, char *filename);

void parseEpatterns(Params *params, char *optarg);

void setOpts(char opt, Params *params);

void parseFlagsAndPatterns(int argc, char **argv, Params *params);

void parseFiles(int argc, char **argv, Params *params);

Params parseParams(int argc, char **argv);

bool matchesAnyPattern(Params *params, const char *line);

void handleMatchingPartsOnly(Params *params, const char *line,
                             const char *filename, int *lineNumber);

void hadleOnlyFilenamesAndMatchesCount(Params *params, const char *buffer,
                                       int *matchesCount, int *matches);

void handleOtherFlags(Params *params, const char *line, const char *filename,
                      int *lineNumber);

void grepFile(Params *params, const char *filename);

void grepFiles(Params *params);
