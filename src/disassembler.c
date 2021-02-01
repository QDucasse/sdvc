#include "chunk.h"
#include "disassembler.h"

typedef struct {
  int code;
  char* name;
} OpCodeNames;

OpCodeNames opNames[] = {
  [OP_ADD]   = {OP_ADD, "OP_ADD"},
  [OP_SUB]   = {OP_SUB, "OP_SUB"},
  [OP_MUL]   = {OP_MUL, "OP_MUL"},
  [OP_DIV]   = {OP_DIV, "OP_DIV"},
  [OP_MOD]   = {OP_MOD, "OP_MOD"},
  [OP_AND]   = {OP_AND, "OP_AND"},
  [OP_OR]    = {OP_OR, "OP_OR"},
  [OP_LT]    = {OP_LT, "OP_LT"},
  [OP_LTEQ]  = {OP_LTEQ, "OP_LTEQ"},
  [OP_GT]    = {OP_GT, "OP_GT"},
  [OP_GTEQ]  = {OP_GTEQ, "OP_GTEQ"},
  [OP_EQ]    = {OP_EQ, "OP_EQ"},
  [OP_NEQ]   = {OP_NEQ, "OP_NEQ"},
  [OP_JMP]   = {OP_JMP, "OP_JMP"},
  [OP_STORE] = {OP_STORE, "OP_STORE"},
  [OP_LOAD]  = {OP_LOAD, "OP_LOAD"}
};

void disassembleInstruction(uint32_t bitInstruction) {
  unsigned int op_code = (bitInstruction & 0xF0000000) >> 28;   // 1111 0000 0000 0000 0000 0000 0000 0000
  /* Test if the instruction is binary or not */
  if (op_code < 13) { // BINARY
    unsigned int rd       = (bitInstruction & 0x3C00000) >> 22; // 0000 0011 1100 0000 0000 0000 0000 0000
    unsigned int cfg_mask = (bitInstruction & 0xC000000) >> 26; // 0000 1100 0000 0000 0000 0000 0000 0000
    unsigned int ra   = (bitInstruction & 0x7800) >> 11;        // 0000 0000 0000 0000 0111 1000 0000 0000
    unsigned int rb   = (bitInstruction & 0xF);                 // 0000 0000 0000 0000 0000 0000 0000 1111
    unsigned int imma = (bitInstruction & 0x3FF800) >> 11;      // 0000 0000 0011 1111 1111 1000 0000 0000
    unsigned int immb = (bitInstruction & 0x7FF);               // 0000 0000 0000 0000 0000 0111 1111 1111
    switch (cfg_mask) {
      case CFG_RR: printf("%s - Config: %u - Rd: %u - Ra: %u - Rb: %u\n", opNames[op_code].name, cfg_mask, rd, ra, rb); break;
      case CFG_RI: printf("%s - Config: %u - Rd: %u - Ra: %u - Immb: %u\n", opNames[op_code].name, cfg_mask, rd, ra, immb); break;
      case CFG_IR: printf("%s - Config: %u - Rd: %u - Imma: %u - Rb: %u\n", opNames[op_code].name, cfg_mask, rd, imma, rb); break;
      case CFG_II: printf("%s - Config: %u - Rd: %u - Imma: %u - Immb: %u\n", opNames[op_code].name, cfg_mask, rd, imma, immb); break;
      default: break; // Unreachable
    }
  } else { // UNARY
    unsigned int rd   = (bitInstruction & 0xF00000 ) >> 24;       // 0000 1111 0000 0000 0000 0000 0000 0000
    unsigned int addr = (bitInstruction & 0xFFFFF ) >> 20;        // 0000 0000 1111 1111 1111 1111 1111 1111
    printf("%s - Rd: %u - Addr: %u \n", opNames[op_code].name, rd, addr);
  }
}

void showTableState(Table* table) {

}

void showRegisterState(Register* registers) {

}
