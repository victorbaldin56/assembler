#include "asm.h"
#include "colors.h"
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf(FG_RED);
        printf("VAssembler: no input files\n");
        printf(RESET);
        return -1;
    }

    if (argc == 4 && !strcmp("-o", argv[3])) {
        return Assembly(argv[1], argv[3]);
    }

    return Assembly(argv[1], "a.out");
}
