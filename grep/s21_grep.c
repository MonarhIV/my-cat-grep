#include "s21_grep.h"

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  Params params = parseParams(argc, argv);
  grepFiles(&params);
  freeParams(&params);
  return 0;
}

void freeParams(Params *params) {
  if (params->files != NULL) {
    free(params->files);
  }
  if (params->patterns != NULL) {
    free(params->patterns);
  }
}

void handleErrorAndExit(Params *params, char *reason) {
  fprintf(stderr, "s21_grep: %s\n", reason);
  freeParams(params);
  exit(EXIT_FAILURE);
}

void handleErrorOpeningFileAndExit(Params *params, char *filename) {
  fprintf(stderr, "s21_grep: Error opening file: %s\n", filename);
  freeParams(params);
  exit(EXIT_FAILURE);
}

void parseFilePatterns(Params *params, char *filename) {
  params->f = 1;
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    handleErrorOpeningFileAndExit(params, filename);
  }

  char buffer[BUFFER_SIZE];
  while (fgets(buffer, sizeof(buffer), file)) {
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
    params->patternsCount++;
    params->patterns =
        realloc(params->patterns, params->patternsCount * sizeof(char *));
    if (params->patterns == NULL) {
      fclose(file);
      handleErrorAndExit(params, "Memory error");
    }

    params->patterns[params->patternsCount - 1] = strdup(buffer);
    if (params->patterns[params->patternsCount - 1] == NULL) {
      fclose(file);
      handleErrorAndExit(params, "Memory error");
    }
  }
  fclose(file);
}

void parseEpatterns(Params *params, char *optarg) {
  params->e = 1;
  params->patternsCount++;
  params->patterns =
      realloc(params->patterns, params->patternsCount * sizeof(char *));
  if (params->patterns == NULL) {
    handleErrorAndExit(params, "Memory error");
  }
  params->patterns[params->patternsCount - 1] = optarg;
}

void setOpts(char opt, Params *params) {
  if (opt == 'e')
    parseEpatterns(params, optarg);
  else if (opt == 'i')
    params->i = 1;
  else if (opt == 'v')
    params->v = 1;
  else if (opt == 'c')
    params->c = 1;
  else if (opt == 'l')
    params->l = 1;
  else if (opt == 'n')
    params->n = 1;
  else if (opt == 'h')
    params->h = 1;
  else if (opt == 's')
    params->s = 1;
  else if (opt == 'f')
    parseFilePatterns(params, optarg);
  else if (opt == 'o')
    params->o = 1;
  else if (opt == '?') {
    handleErrorAndExit(params, "Unknown flag");
  } else {
    handleErrorAndExit(params, "Could not parse params");
  }
}

void parseFlagsAndPatterns(int argc, char **argv, Params *params) {
  params->patterns = NULL;
  params->patternsCount = 0;
  char opt;
  while ((opt = getopt(argc, argv, "e:ivclnhsf:o")) != -1) setOpts(opt, params);
  if (params->patternsCount == 0 && optind < argc) {
    params->patternsCount = 1;
    params->patterns = malloc(sizeof(char *));
    if (params->patterns == NULL) {
      handleErrorAndExit(params, "Memory error");
    }
    params->patterns[0] = argv[optind];
    optind++;
  }
  if (params->patternsCount == 0) {
    handleErrorAndExit(params, "Too few args");
  }
  if (params->v) params->o = 0;
}

void parseFiles(int argc, char **argv, Params *params) {
  params->files_count = argc - optind;
  if (params->files_count <= 0) {
    handleErrorAndExit(params, "Too few args");
  }

  params->files = malloc(params->files_count * sizeof(char *));
  if (params->files == NULL) {
    handleErrorAndExit(params, "Memory error");
  }

  for (int i = 0; i < params->files_count; i++) {
    params->files[i] = argv[optind + i];
  }
}

Params parseParams(int argc, char **argv) {
  Params params = {0};
  parseFlagsAndPatterns(argc, argv, &params);
  parseFiles(argc, argv, &params);
  return params;
}
bool matchesAnyPattern(Params *params, const char *line) {
  regex_t regex;
  int reti;
  for (int i = 0; i < params->patternsCount; i++) {
    reti = regcomp(&regex, params->patterns[i], params->i ? REG_ICASE : 0);
    if (reti) {
      handleErrorAndExit(params, "Error compiling regular expression");
    }
    reti = regexec(&regex, line, 0, NULL, 0);
    regfree(&regex);
    if (!reti) return true;
  }
  return false;
}

void handleMatchingPartsOnly(Params *params, const char *line,
                             const char *filename, int *lineNumber) {
  (*lineNumber)++;
  regex_t regex;
  regmatch_t match;

  for (int i = 0; i < params->patternsCount; i++) {
    int reti = regcomp(&regex, params->patterns[i], params->i ? REG_ICASE : 0);
    if (reti) {
      handleErrorAndExit(params, "Error compiling regular expression");
    }

    const char *cursor = line;
    while (regexec(&regex, cursor, 1, &match, 0) == 0) {
      int start = match.rm_so;
      int end = match.rm_eo;
      if (params->files_count > 1 && !params->h) {
        if (params->n)
          printf("%s:%d:%.*s\n", filename, *lineNumber, end - start,
                 cursor + start);
        else
          printf("%s:%.*s\n", filename, end - start, cursor + start);
      } else {
        if (params->n)
          printf("%d:%.*s\n", *lineNumber, end - start, cursor + start);
        else
          printf("%.*s\n", end - start, cursor + start);
      }
      cursor += end;
    }
    regfree(&regex);
  }
}

void hadleOnlyFilenamesAndMatchesCount(Params *params, const char *buffer,
                                       int *matchesCount, int *matches) {
  if (params->v != matchesAnyPattern(params, buffer)) {
    (*matchesCount)++;
    (*matches) = 1;
  }
}
void handleOtherFlags(Params *params, const char *line, const char *filename,
                      int *lineNumber) {
  (*lineNumber)++;
  if (params->v != matchesAnyPattern(params, line)) {
    if (params->files_count > 1 && !params->h) {
      if (params->n) printf("%s:%d:%s", filename, *lineNumber, line);
      if (!params->n) printf("%s:%s", filename, line);
    } else {
      if (params->n) printf("%d:", *lineNumber);
      printf("%s", line);
    }
  }
}

void grepFile(Params *params, const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    if (params->s)
      return;
    else {
      fprintf(stderr, "s21_grep: Error opening file\n");
      return;
    }
  }
  char buffer[BUFFER_SIZE];
  int lineNumber = 0;
  int matchesCount = 0;
  int matches = 0;
  if (params->c || params->l) {
    while (fgets(buffer, sizeof(buffer), file))
      hadleOnlyFilenamesAndMatchesCount(params, buffer, &matchesCount,
                                        &matches);
    if (params->c) {
      if (params->files_count > 1 && !params->h) printf("%s:", filename);
      printf("%d\n", matchesCount);
    }
    if (params->l) {
      if (matches) printf("%s\n", filename);
    }
  }
  if (params->o && !params->c && !params->l) {
    while (fgets(buffer, sizeof(buffer), file))
      handleMatchingPartsOnly(params, buffer, filename, &lineNumber);
  }
  if (!params->c && !params->l && !params->o) {
    while (fgets(buffer, sizeof(buffer), file))
      handleOtherFlags(params, buffer, filename, &lineNumber);
  }
  fclose(file);
}

void grepFiles(Params *params) {
  for (int i = 0; i < params->files_count; i++)
    grepFile(params, params->files[i]);
}
