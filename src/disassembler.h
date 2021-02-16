#ifndef sdvu_disassembler_h
#define sdvu_disassembler_h

#include "chunk.h"
#include "register.h"
#include "table.h"
#include "value.h"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

typedef struct {
  bool verbose;
  FILE* outstream;
} Disassembler;

Disassembler* disassembler;

void initDisassembler(bool verbose, FILE* outstream);
void freeDisassembler();
void disassembleInstruction(uint32_t bitInstruction, Disassembler* disassembler);
void showTableState(Table* table, Disassembler* disassembler);
void showRegisterState(Register* registers, Register* topTempRegister, Register* topGlobRegister, Disassembler* disassembler);
void disassembleChunk(Chunk* chunk, Disassembler* disassembler);
void disassembleBinary(const char* fileContent, Disassembler* disassembler);

#endif
