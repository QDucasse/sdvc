#ifndef sdvu_disassembler_h
#define sdvu_disassembler_h

#include "chunk.h"
#include "register.h"
#include "table.h"
#include "value.h"


void disassembleInstruction(uint32_t bitInstruction);
void showTableState(Table* table);
void showRegisterState(Register* registers);
void disassembleChunk(Chunk* chunk);


#endif
