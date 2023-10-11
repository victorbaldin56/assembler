#include "asm.h"
#include "parse.h"
#include <assert.h>
#include "colors.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <limits.h>

static const size_t MAXCMD = 10;
static const size_t MAXREG = 4;

static const int PERMS = 0666;                            ///< permissions for created file

static const size_t MAXSTR = 1000;                        ///< max string lenght

static cmd_error parse_cmd(char *cmd, int outp_fd);

static cmd_error recognize_reg_name(const char *reg_name, int outp_fd);

static unsigned long long code(const char opcode, const char mask);

asm_error Assemble(const char *inp_filename, const char *outp_filename) {
    assert(inp_filename);
    assert(outp_filename);

    char *buf = ReadFile(inp_filename);

    if (!buf) {
        fprintf(stderr, "Buffer allocation failed\n");
        return BUF_ALLOC_FAILURE;
    }

    LineArray text = {};

    if (!ParseBuf(buf, &text)) {
        fprintf(stderr, "Code index failed\n");
        return LINEARR_ALLOC_FAILURE;
    }

    int outp_fd = creat(outp_filename, PERMS);

    if (outp_fd < 0) {
        perror("Assemble");
        return FILE_CREAT_FAILURE;
    }

    asm_error err_flag = SUCCESS;

    for (size_t lp = 0; lp < text.size; lp++) {
        switch (parse_cmd(text.lines[lp], outp_fd)) {
            case NO_ERR:
                break;

            case NO_ARG:
                ERR_MESSAGE(inp_filename, lp, "argument expected", err_flag);
                break;

            case EXTRA_ARG:
                ERR_MESSAGE(inp_filename, lp, "extra argument", err_flag);
                break;

            case UNKNOWN:
                ERR_MESSAGE(inp_filename, lp, "unknown command", err_flag);
                break;

            default:
                ERR_MESSAGE(inp_filename, lp, "unknown command", err_flag);
                break;
        }
    }

    free(text.lines);
    free(buf);
    return err_flag;
}

static cmd_error parse_cmd(char *cmd, int outp_fd) {
    assert(cmd);
    assert(outp_fd >= 0);

    // getting rid of comments
    char *comblock = strchr(cmd, ';');

    if (comblock) {
        *comblock = '\0';
    }

    char cmd_name[MAXCMD] = {};

    if (sscanf(cmd, "%s", cmd_name)) {

        for (size_t cp = 0; cp < sizeof(CMD_LIST) / sizeof(Command); cp++) {

            if (!strcmp(cmd_name, CMD_LIST[cp].name)) {

                unsigned long long cmd_code = 0;

                if (CMD_LIST[cp].imm && CMD_LIST[cp].reg) {
                    double immed = 0; ///< immediate constant argument

                    if (sscanf(cmd, "%*s %lf", &immed)) {
                        cmd_code = code(CMD_LIST[cp].opcode, IMM);
                        write(outp_fd, (char *)&cmd_code, CMDSIZ);
                        ON_DEBUG(printf("immed argument = %lf\n", immed));
                        write(outp_fd, (char *)&immed, CMDSIZ);

                        return NO_ERR;
                    }

                    char reg_name[MAXREG] = {};

                    if (sscanf(cmd, "%*s %s", reg_name)) {
                        unsigned long long cmd_code = code(CMD_LIST[cp].opcode, REG);
                        write(outp_fd, (char *)&cmd_code, CMDSIZ);
                        return recognize_reg_name(reg_name, outp_fd);
                    }

                }

                else if (CMD_LIST[cp].reg && !CMD_LIST[cp].imm) {
                    cmd_code = code(CMD_LIST[cp].opcode, REG);
                    write(outp_fd, (char *)&cmd_code, CMDSIZ);

                    char reg_name[MAXREG] = {};

                    if (sscanf(cmd, "%*s %s", reg_name)) {
                        return recognize_reg_name(reg_name, outp_fd);
                    }

                    return NO_ARG;
                }

                else {
                    cmd_code = code(CMD_LIST[cp].opcode, 0);
                    write(outp_fd, (char *)&cmd_code, CMDSIZ);
                    return NO_ERR;
                }

            }
        }
    }

    return NO_ERR;
}

static cmd_error recognize_reg_name(const char *reg_name, int outp_fd) {
    assert(reg_name);

    unsigned long long reg_num = NUM_REGS + 1;

    if (!strcmp(reg_name, "rax")) {
        reg_num = (unsigned long long)RAX;
    }

    else if (!strcmp(reg_name, "rbx")) {
        reg_num = (unsigned long long)RBX;
        ON_DEBUG(printf("reg_num = %llu\n", reg_num));
    }

    else if (!strcmp(reg_name, "rcx")) {
        reg_num = (unsigned long long)RCX;
    }

    else if (!strcmp(reg_name, "rdx")) {
        reg_num = (unsigned long long)RDX;
    }

    else {
        return INCORRECT_ARG;
    }

    write(outp_fd, (char *)&reg_num, CMDSIZ);

    return NO_ERR;
}

static unsigned long long code(const char opcode, const char mask) {
    long long value = ((opcode | mask) << BITOFFSET);
    return value;
}
