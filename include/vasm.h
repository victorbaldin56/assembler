#ifndef VASM
#define VASM

#include <stdio.h>

#define DEBUG

#ifdef DEBUG
#define ON_DEBUG(x) x
#else
#define ON_DEBUG(x)
#endif

int Assembly(const char *input, const char *output);

typedef struct {
    int code;     ///< digital code
    char *name;   ///< name of the command
    size_t nargs; ///< number of inline args
} Command;

const Command CMD_LIST[] = {
    {-1, "hlt" , 0},
    { 0, "in"  , 0},
    { 1, "out" , 0},
    { 2, "push", 1},
    { 3, "add" , 0},
    { 4, "sub" , 0},
    { 5, "mult", 0},
    { 6, "div" , 0},
    { 7, "sqrt", 0}};

#endif // VASM
