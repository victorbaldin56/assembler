#ifndef VASM
#define VASM

#include <stdio.h>
#include <limits.h>
#include "VM.h"

#define DEBUG

#ifdef DEBUG
#define ON_DEBUG(x) x
#else
#define ON_DEBUG(x)
#endif

#define ERR_MESSAGE(file, line, msg, err_flag)                          \
    fprintf(stderr, "error: file %s, line %zu: %s", file, line, msg);   \
    err_flag = ASSEMBLE_FAILURE;                                        \

enum cmd_error {
    NO_ERR,
    NO_ARG,
    EXTRA_ARG,
    INCORRECT_ARG,
    UNKNOWN,
};

enum asm_error {
    SUCCESS,
    BUF_ALLOC_FAILURE,
    LINEARR_ALLOC_FAILURE,
    FILE_CREAT_FAILURE,
    ASSEMBLE_FAILURE,
};

asm_error Assemble(const char *input, const char *output);

const size_t CMDSIZ = sizeof(double);              ///< size of each command in bytes
const size_t BITOFFSET = CHAR_BIT *  CMDSIZ; ///< offset to get 8-byte command

enum Regs {
    RAX = 0x01,
    RBX = 0x02,
    RCX = 0x03,
    RDX = 0x04,
};

typedef struct {
    char opcode;         ///< operation code
    const char *name;    ///< name of the command
    unsigned char imm;   ///< can command has immediate constant argument
    unsigned char reg;   ///< can command has register argument
} Command;

const Command CMD_LIST[] = {
    {HALT, "hlt" , 0, 0},
    {IN  , "in"  , 0, 0},
    {OUT , "out" , 0, 0},
    {PUSH, "push", 1, 1},
    {ADD , "add" , 0, 0},
    {SUB,  "sub" , 0, 0},
    {MULT, "mult", 0, 0},
    {DIV , "div" , 0, 0},
    {SQRT, "sqrt", 0, 0},
    {POP , "pop",  0, 1}};

#endif // VASM
