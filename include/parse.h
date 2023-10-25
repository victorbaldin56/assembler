#ifndef PARSE
#define PARSE

#include <stdio.h>

/// @brief Holds an array of poineters to parsed buffer
typedef struct {
    char **lines;
    size_t size;
} LineArray;

LineArray *ParseBuf(char *buf, LineArray *lines);

char *ReadFile(const char *filename);

ssize_t fsize(const char *filename);

/// @brief counts a number of occurencies to the given string
size_t strcount_(const char *str, const char chr);

#endif
