#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include "asm.h"
#include "codector.h"
#include "writecode.h"
#include "parse.h"
#include "colors.h"

static const size_t MAXCMD = 10;
static const size_t MAXREG = 4;

static const int PERMS = 0777;                            ///< permissions for created file

static const size_t MAXSTR = 1000;                        ///< max string lenght

static cmd_error compile_cmd(char *cmd, Code *codearr, size_t *ip);

static cmd_error compile_args(const char *cmd, Code *codearr, size_t *ip);

asm_error Assemble(const char *inp_filename, const char *outp_filename) {
    assert(inp_filename);
    assert(outp_filename);

    char *buf = ReadFile(inp_filename);

    if (!buf) {
        perror("Assembler");
        fprintf(stderr, "Buffer allocation failed\n");
        return BUF_ALLOC_FAILURE;
    }

    LineArray text = {};

    if (!ParseBuf(buf, &text)) {
        fprintf(stderr, "Code index failed\n");
        return LINEARR_ALLOC_FAILURE;
    }

    Code codearr = {};

    CodeCtor(&codearr, INIT_CODE_SIZE);

    int outp_fd = creat(outp_filename, PERMS);

    if (outp_fd < 0) {
        perror("Assemble");
        return FILE_CREAT_FAILURE;
    }

    asm_error err_flag = SUCCESS;

    size_t ip = 0;

    WriteSign(&codearr, &ip);

    for (size_t lp = 0; lp < text.size; lp++) {
        switch (compile_cmd(text.lines[lp], &codearr, &ip)) {
            case NO_ERR:
            {
                break;
            }

            case INCORRECT_ARG:
            {
                ERR_MESSAGE(inp_filename, lp, "incorrect argument", err_flag);
                break;
            }

            case NO_ARG:
            {
                ERR_MESSAGE(inp_filename, lp, "argument expected", err_flag);
                break;
            }

            case UNKNOWN:
            {
                ERR_MESSAGE(inp_filename, lp, "unknown command", err_flag);
                break;
            }

            case EMIT_FAILURE:
            {
                ERR_MESSAGE(inp_filename, lp, "crash due to memory limit", err_flag);
                break;
            }

            default:
            {
                ERR_MESSAGE(inp_filename, lp, "unknown command", err_flag);
                break;
            }
        }
    }

    WriteCode(&codearr, outp_fd);

    free(text.lines);
    free(buf);

    CodeDtor(&codearr);

    return err_flag;
}

#define DEF_CMD(name, opcode, has_arg, ...)             \
    if (!strcmp(#name, cmd_name)) {                     \
                                                        \
        EmitByte(codearr, ip, opcode);                  \
                                                        \
        if (!has_arg) {                                 \
            (*ip)++;                                    \
            return NO_ERR;                              \
        }                                               \
                                                        \
        return compile_args(cmd, codearr, ip);          \
    }

static cmd_error compile_cmd(char *cmd, Code *codearr, size_t *ip) {
    assert(cmd);
    CODE_ASSERT(codearr);
    assert(ip);

    char cmd_name[MAXCMD] = {};

    if (sscanf(cmd, "%s", cmd_name) > 0) {
        ON_DEBUG(fprintf(stderr, "%s | ", cmd));

        #include "commands.h"

        return UNKNOWN;
    }

    return NO_ERR;
}

#undef DEF_CMD

#define EmitImm_(codearr, ip, imm)                          \
    if (EmitImm(codearr, ip, imm) != 0) {                   \
        ON_DEBUG(fprintf(stderr, "EmitImm failure\n"));     \
        return EMIT_FAILURE;                                \
    }

#define EmitReg_(codearr, ip, regnum)                       \
    if (EmitReg(codearr, ip, regnum) != 0) {                \
        ON_DEBUG(fprintf(stderr, "EmitReg failure\n"));     \
        return EMIT_FAILURE;                                \
    }

static cmd_error compile_args(const char *cmd, Code *codearr, size_t *ip) {
    assert(cmd);
    CODE_ASSERT(codearr);
    assert(ip);

    size_t pos = *ip;

    double imm = 0;
    unsigned char regch = '\0';

    if (sscanf(cmd, "%*s %lf", &imm) > 0) {
        codearr->code[*ip] |= IMM;
        (*ip)++;
        EmitImm_(codearr, ip, imm);

        ON_DEBUG(fprintf(stderr, "%lf\n", imm));
        return NO_ERR;
    }

    else if (sscanf(cmd, "%*s r%cx", &regch) > 0) {
        codearr->code[*ip] |= REG;
        (*ip)++;
        EmitReg_(codearr, ip, regch - 'a' + 1);

        if (sscanf(cmd, "%*s %*s + %lf", &imm) > 0) {
            codearr->code[*ip - 2] |= IMM;
            EmitImm_(codearr, ip, imm);
        }

        return NO_ERR;
    }

    else if (sscanf(cmd, "%*s [ %lf ]", &imm) > 0) {
        codearr->code[*ip] |= (RAM | IMM);
        (*ip)++;
        EmitImm_(codearr, ip, imm);

        return NO_ERR;
    }

    else if (sscanf(cmd, "%*s [ r%cx ]", &regch) > 0) {
        codearr->code[*ip] |= (RAM | REG);
        (*ip)++;
        EmitReg_(codearr, ip, regch - 'a' + 1);

        if (sscanf(cmd, "%*s [ %*s + %lf ]", &imm) > 0) {
            EmitImm_(codearr, ip, imm);
            codearr->code[pos] |= IMM;
        }

        return NO_ERR;
    }

/*    else if (sscanf(cmd, "%*s [ r%cx + %lf ]", &regch, &imm)) {
        codearr->code[*ip] |= (RAM | REG | IMM);
        (*ip)++;
        EmitReg_(codearr, ip, regch - 'a' + 1);
        (*ip)++;
        EmitImm_(codearr, ip, imm);

        return NO_ERR;
    }*/

    return INCORRECT_ARG;
}
