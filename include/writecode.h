#ifndef WRITECODE
#define WRITECODE

#include <stdio.h>
#include "codector.h"

void WriteCode(Code *codearr, int outp_fd);

int EmitByte(Code *codearr, size_t *ip, unsigned char cmd_code);

int EmitImm(Code *codearr, size_t *ip, double imm);

int EmitReg(Code *codearr, size_t *ip, unsigned char regnum);

#endif
