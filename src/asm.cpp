#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include "asm.h"
#include "codector.h"
#include "writecode.h"
#include "parse.h"
#include "colors.h"

static const size_t MAXCMD = 10;
static const size_t MAXREG = 4;

static const int PERMS = 0777;                            ///< permissions for created file

static const size_t MAXSTR = 1000;                        ///< max string lenght

static CmdError compile_cmd(char *cmd, Code *codearr, size_t *ip, Labels *labels);

static int push_label(Labels *labels, const char *label, size_t *ip);

static double find_label(Labels *labels, const char *label);

static CmdError compile_args(const char *cmd, Code *codearr, size_t *ip, Labels *labels);

static AsmError compilation_run(LineArray *text, Code *codearr, const char *inp_filename);

AsmError Assemble(const char *inp_filename, const char *outp_filename) {
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

    AsmError err_flag = compilation_run(&text, &codearr, inp_filename);

    if (err_flag != SUCCESS) {
        remove(outp_filename);
    }

    else {
        int outp_fd = creat(outp_filename, PERMS);

        if (outp_fd < 0) {
            perror("Assemble");
            return FILE_CREAT_FAILURE;
        }

        write(outp_fd, codearr.code, codearr.size);
    }

    free(text.lines);
    free(buf);

    CodeDtor(&codearr);

    return err_flag;
}

static AsmError compilation_run(LineArray *text, Code *codearr, const char *inp_filename) {
    CODE_ASSERT(codearr);
    assert(text);
    assert(inp_filename);

    size_t ip = SIGNATURE_SIZE;

    WriteSign(codearr);

    Labels labels = {};

    for (size_t lp = 0; lp < text->size; lp++) {
        switch (compile_cmd(text->lines[lp], codearr, &ip, &labels)) {
            case NO_ERR:
            {
                break;
            }

            case INCORRECT_ARG:
            {
                ASM_RAISE_ERR("incorrect argument");
            }

            case NO_ARG:
            {
                ASM_RAISE_ERR("argument expected");
            }

            case UNKNOWN:
            {
                ASM_RAISE_ERR("unknown command");
            }

            case EMIT_FAILURE:
            {
                ASM_RAISE_ERR("crash due to memory limit");
            }

            case NUM_LABELS_EXCEED:
            {
                ASM_RAISE_ERR("max number of labels exceed");
            }

            case LABEL_NOT_FOUND:
            {
                ASM_RAISE_ERR("label not found");
            }

            default:
            {
                ASM_RAISE_ERR("unknown command");
            }
        }
    }

    return SUCCESS;
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
        return compile_args(cmd, codearr, ip, labels);  \
    }

static CmdError compile_cmd(char *cmd, Code *codearr, size_t *ip, Labels *labels) {
    assert(cmd);
    CODE_ASSERT(codearr);
    assert(ip);
    assert(labels);

    char cmd_name[MAXCMD] = {};

    if (sscanf(cmd, "%s", cmd_name) > 0) {
        if (strchr(cmd_name, ':')) {
            if (push_label(labels, cmd_name, ip) != 0) return NUM_LABELS_EXCEED;

            return NO_ERR;
        }

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

static int push_label(Labels *labels, const char *label, size_t *ip) {
    assert(labels);
    assert(label);
    assert(ip);

    if (labels->count >= NUM_LABELS) return -1;

    strncpy(labels->names[labels->count], label, LABEL_LENGHT);
    labels->addrs[labels->count] = (double)(*ip) - SIGNATURE_SIZE;
    labels->count++;

    return 0;
}

static double find_label(Labels *labels, const char *label) {
    assert(labels);
    assert(label);

    for (size_t lcount = 0; lcount < NUM_LABELS; lcount++) {
        if (!strcmp(label, labels->names[lcount])) {
            return labels->addrs[lcount];
        }
    }

    return NAN;
}

static CmdError compile_args(const char *cmd, Code *codearr, size_t *ip, Labels *labels) {
    assert(cmd);
    CODE_ASSERT(codearr);
    assert(ip);
    assert(labels);

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

    // if all this doesnt work, search for label

    char label[LABEL_LENGHT] = {};

    if (sscanf(cmd, "j%*s %s", label) > 0) {
        double addr = find_label(labels, label);

        if (!isfinite(addr)) return LABEL_NOT_FOUND;

        codearr->code[pos] |= IMM;
        (*ip)++;
        EmitImm_(codearr, ip, addr);
        return NO_ERR;
    }

    return INCORRECT_ARG;
}
