#ifndef WRITECODE
#define WRITECODE

#include <stdio.h>
#include "codector.h"

int EmitByte(Code *codearr, size_t *ip, unsigned char cmd_code);

int EmitImm(Code *codearr, size_t *ip, double imm);

int EmitReg(Code *codearr, size_t *ip, unsigned char regnum);

void WriteSign(Code *codearr);

#endif
