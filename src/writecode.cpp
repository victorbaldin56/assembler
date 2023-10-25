#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "codector.h"
#include "writecode.h"
#include "codedump.h"

static const size_t COEFF = 4;

void WriteCode(Code *codearr, int outp_fd) {
    CODE_ASSERT(codearr);
    assert(outp_fd >= 0);

    write(outp_fd, codearr->code, codearr->size);
}

int EmitByte(Code *codearr, size_t *ip, unsigned char cmd_code) {
    CODE_ASSERT(codearr);
    assert(ip);

    if (*ip >= codearr->size) {
        if (!CodeRealloc(codearr, COEFF)) return -1;
    }

    ON_DEBUG(fprintf(stderr, "EmitByte: code realloc success\n"));

//    CODE_DUMP(codearr);

    codearr->code[*ip] = cmd_code;
    return 0;
}

int EmitImm(Code *codearr, size_t *ip, double imm) {
    CODE_ASSERT(codearr);
    assert(ip);

    ON_DEBUG(fprintf(stderr, "EmitImm: *ip = %zu\n", *ip));

    codearr->code[*ip] |= IMM;

    (*ip)++;

    while (*ip + sizeof(double) >= codearr->size) {
        if (!CodeRealloc(codearr, COEFF)) return -1;
    }

    *(double *)(codearr->code + *ip) = imm;
    (*ip) += sizeof(double);

    return 0;
}

int EmitReg(Code *codearr, size_t *ip, unsigned char regnum) {
    CODE_ASSERT(codearr);
    assert(ip);

    codearr->code[*ip] |= REG;

    if (*ip >= codearr->size - 1) {
        if (!CodeRealloc(codearr, COEFF)) return -1;
    }

    codearr->code[++(*ip)] = regnum;
    (*ip)++;

    return 0;
}