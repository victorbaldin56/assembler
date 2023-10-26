#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "codector.h"
#include "writecode.h"
#include "codedump.h"

static const size_t COEFF = 2;
static const size_t SIGNSIZ = 6; ///< size of signature

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

    ON_DEBUG(fprintf(stderr, "EmitImm: imm = %lf, *ip = %zu\n", imm, *ip));

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

    if (*ip >= codearr->size - 1) {
        if (!CodeRealloc(codearr, COEFF)) return -1;
    }

    codearr->code[*ip] = regnum;
    (*ip)++;

    return 0;
}

void WriteSign(Code *codearr, size_t *ip) {
    CODE_ASSERT(codearr);
    assert(ip);

    *(int32_t *)codearr->code = SIGNATURE;
    (*ip) += sizeof(int32_t);

    codearr->code[*ip] = VERSION;
    (*ip)++;
}
