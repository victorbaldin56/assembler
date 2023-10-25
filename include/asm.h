#ifndef VASM
#define VASM

#include <stdio.h>
#include <limits.h>
#include "codector.h"

#define DEBUG

#ifdef DEBUG
#define ON_DEBUG(x) x
#else
#define ON_DEBUG(x)
#endif

#define ERR_MESSAGE(file, line, msg, err_flag)                            \
    fprintf(stderr, "error: file %s, line %zu: %s\n", file, line, msg);   \
    err_flag = ASSEMBLE_FAILURE;

enum cmd_error {
    NO_ERR,
    NO_ARG,
    INCORRECT_ARG,
    UNKNOWN,
    EMIT_FAILURE,
};

enum asm_error {
    SUCCESS,
    BUF_ALLOC_FAILURE,
    LINEARR_ALLOC_FAILURE,
    FILE_CREAT_FAILURE,
    ASSEMBLE_FAILURE,
};

asm_error Assemble(const char *input, const char *output);

const size_t CMDSIZ = sizeof(double);               ///< size of each command in bytes
const size_t INIT_CODE_SIZE = 16;                   ///< initial size of code array

typedef struct {
    char opcode;         ///< operation code
    const char *name;    ///< name of the command
    unsigned char imm;   ///< can command has immediate constant argument
    unsigned char reg;   ///< can command has register argument
} Command;

#endif // VASM
