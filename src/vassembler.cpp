#include "vasm.h"
#include <assert.h>
#include "colors.h"
#include <string.h>

static const size_t MAXSTR = 1000; ///< max string lenght
static const size_t MAXCMD = 10;   ///< max command name leght

/// @brief Contains possible compilation issues
enum cmd_error {
    SUCCESS,
    NO_ARG,
    EXTRA_ARG,
    UNKNOWN,
};

static cmd_error parse_cmd(const char *cmd, FILE *outptr);

int Assembly(const char *input, const char *output) {
    assert(input);
    assert(output);

    FILE *inptr = fopen(input, "r");
    FILE *outptr = fopen(output, "w");

    if (!inptr || !outptr) {
        perror("VAssembler");
        return -1;
    }

    int errflag = 0;
    char cmd[MAXSTR] = {};
    size_t line_count = 0;

    while (fgets(cmd, MAXSTR, inptr)) {
        line_count++;
        cmd_error err_code = parse_cmd(cmd, outptr);

        if (err_code) {
            printf("VAssembler:");
            printf(FG_RED);
            printf("error:\nIn file %s:%zu\t%s\n", input, line_count, cmd);
            printf(RESET);

            switch (err_code) {
                case NO_ARG:
                    printf("Expected an argument\n");
                    break;
                case EXTRA_ARG:
                    printf("Extra argument\n");
                    break;
                default:
                    printf("Unknown command\n");
                    break;
            }

            errflag = -1;
        }
    }

    return errflag;
}

static cmd_error parse_cmd(const char *cmd, FILE *outptr) {
    assert(cmd);
    assert(outptr);

    char cmd_name[MAXCMD] = {};

    // skipping empty strings
    if (sscanf(cmd, "%s", cmd_name) == EOF) {
        return SUCCESS;
    }

    if (!sscanf(cmd, "%s", cmd_name)) {
        return UNKNOWN;
    }

    double arg = 0;

    for (size_t i = 0; i < sizeof(CMD_LIST) / sizeof(Command); i++) {
        if (!strcmp(cmd_name, CMD_LIST[i].name)) {
            if (CMD_LIST[i].nargs) {
                if (sscanf(cmd, "%*s %lf", &arg) <= 0) {
                    return NO_ARG;
                }

                fprintf(outptr, "%d %lf\n", CMD_LIST[i].code, arg);
                return SUCCESS;
            }

            fprintf(outptr, "%d\n", CMD_LIST[i].code);
            return SUCCESS;
        }
    }

    return UNKNOWN;
}
