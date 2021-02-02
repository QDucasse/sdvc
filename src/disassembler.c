#include "chunk.h"
#include "disassembler.h"

typedef struct {
  int code;
  char* name;
} CodeNames;

CodeNames codeNames[] = {
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

CodeNames configNames[] = {
  [CFG_RR]   = {CFG_RR, "CFG_RR"},
  [CFG_RI]   = {CFG_RI, "CFG_RI"},
  [CFG_IR]   = {CFG_IR, "CFG_IR"},
  [CFG_II]   = {CFG_II, "CFG_II"}
};

CodeNames loadConfigs[] = {
  [LOAD_REG] = {LOAD_REG, "LOAD_REG"},
  [LOAD_IMM] = {LOAD_IMM, "LOAD_IMM"},
  [LOAD_ADR] = {LOAD_ADR, "LOAD_ADR"}
};

void disassembleInstruction(uint32_t bitInstruction) {
  unsigned int op_code = (bitInstruction & 0xF0000000) >> 28;   // 1111 0000 0000 0000 0000 0000 0000 0000
  /* Test if the instruction is binary or not */
  // printf("=== Disassembling instruction ===\n");
  if (op_code < 13) { // BINARY
    unsigned int rd       = (bitInstruction & 0x3C00000) >> 22; // 0000 0011 1100 0000 0000 0000 0000 0000
    unsigned int cfg_mask = (bitInstruction & 0xC000000) >> 26; // 0000 1100 0000 0000 0000 0000 0000 0000
    unsigned int ra   = (bitInstruction & 0x7800) >> 11;        // 0000 0000 0000 0000 0111 1000 0000 0000
    unsigned int rb   = (bitInstruction & 0xF);                 // 0000 0000 0000 0000 0000 0000 0000 1111
    unsigned int imma = (bitInstruction & 0x3FF800) >> 11;      // 0000 0000 0011 1111 1111 1000 0000 0000
    unsigned int immb = (bitInstruction & 0x7FF);               // 0000 0000 0000 0000 0000 0111 1111 1111

    switch (cfg_mask) {
      case CFG_RR: printf("%7s - Config: %8s - Rd: %2u -   Ra: %5u -   Rb: %5u\n", codeNames[op_code].name, configNames[cfg_mask].name, rd, ra, rb); break;
      case CFG_RI: printf("%7s - Config: %8s - Rd: %2u -   Ra: %5u - Immb: %5u\n", codeNames[op_code].name, configNames[cfg_mask].name, rd, ra, immb); break;
      case CFG_IR: printf("%7s - Config: %8s - Rd: %2u - Imma: %5u -   Rb: %5u\n", codeNames[op_code].name, configNames[cfg_mask].name, rd, imma, rb); break;
      case CFG_II: printf("%7s - Config: %8s - Rd: %2u - Imma: %5u - Immb: %5u\n", codeNames[op_code].name, configNames[cfg_mask].name, rd, imma, immb); break;
      default: break; // Unreachable
    }
  } else if (op_code == OP_LOAD) {
    unsigned int rd       = (bitInstruction & 0x3C00000) >> 22; // 0000 0011 1100 0000 0000 0000 0000 0000
    unsigned int cfg_mask = (bitInstruction & 0xC000000) >> 26; // 0000 1100 0000 0000 0000 0000 0000 0000
    unsigned int ra   = (bitInstruction & 0x7800) >> 11;        // 0000 0000 0000 0000 0111 1000 0000 0000
    unsigned int imma = (bitInstruction & 0x3FF800) >> 11;      // 0000 0000 0011 1111 1111 1000 0000 0000
    unsigned int addr = (bitInstruction & 0x3FFFFFF);           // 0000 0000 0011 1111 1111 1111 1111 1111

    switch (cfg_mask) {
      case LOAD_REG: printf("OP_LOAD - Config: %s - Rd: %2u -   Ra: %5u\n", loadConfigs[cfg_mask].name, rd, ra); break;
      case LOAD_IMM: printf("OP_LOAD - Config: %s - Rd: %2u - Imma: %5u\n", loadConfigs[cfg_mask].name, rd, imma); break;
      case LOAD_ADR: printf("OP_LOAD - Config: %s - Rd: %2u - Addr: %5u\n", loadConfigs[cfg_mask].name, rd, addr); break;
      default: break; // Unreachable
    }
  } else { // UNARY STORE/JMP
    unsigned int rd   = (bitInstruction & 0xF00000 ) >> 24;       // 0000 1111 0000 0000 0000 0000 0000 0000
    unsigned int addr = (bitInstruction & 0xFFFFF ) >> 20;        // 0000 0000 1111 1111 1111 1111 1111 1111
    printf("%8s - Rd: %2u - Addr: %u \n", codeNames[op_code].name, rd, addr);
  }
  // printf("=== ------------------------- ===\n\n");
}

void showTableState(Table* table) {
  printf("=== Global Variables Hash Table ===\n");
  for (int i = 0 ; i < table->capacity ; i++) {
    if (!IS_NIL(table->entries[i].value)) {
      printf("[%2i] - Variable named %s\n", i, table->entries[i].key->chars);
    }
  }
  printf("=== --------------------------- ===\n");
}

void showRegisterState(Register* registers, Register* topTempRegister, Register* topGlobRegister) {
  printf("=== Register states ===\n");
  for (int i = 0 ; i < REG_NUMBER ; i++) {
    if (!(registers[i].varName == NULL)) {
      printf("[%2i] - Variable named %8s", i, registers[i].varName->chars);
    } else {
      printf("[%2i] - Empty                  ", i);
    }

    if (i == topTempRegister->number) {
      printf(" < TOP Temp\n");
    } else if (i == topGlobRegister->number) {
      printf(" < TOP Glob\n");
    } else {
      printf("\n");
    }
  }
  printf("=== --------------- ===\n");
}

void disassembleChunk(Chunk* chunk) {
  printf("=== Disassembling resulting chunk ===\n");
  for (int i = 0 ; i < chunk->capacity ; i++) {
    disassembleInstruction(chunk->instructions[i]);
  }
  printf("=== ----------------------------- ===\n");
}
