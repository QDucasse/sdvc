#include "chunk.h"
#include "disassembler.h"

char* codeNames[] = {
  [OP_ADD]   = "OP_ADD",
  [OP_SUB]   = "OP_SUB",
  [OP_MUL]   = "OP_MUL",
  [OP_DIV]   = "OP_DIV",
  [OP_MOD]   = "OP_MOD",
  [OP_AND]   = "OP_AND",
  [OP_OR]    = "OP_OR",
  [OP_LT]    = "OP_LT",
  [OP_GT]    = "OP_GT",
  [OP_EQ]    = "OP_EQ",
  [OP_NOT]   = "OP_NOT",
  [OP_JMP]   = "OP_JMP",
  [OP_STORE] = "OP_STORE",
  [OP_LOAD]  = "OP_LOAD"
};

char* binConfigs[] = {
  [CFG_RR]   = "CFG_RR",
  [CFG_RI]   = "CFG_RI",
  [CFG_IR]   = "CFG_IR",
  [CFG_II]   = "CFG_II"
};

char* typeConfigs[] = {
  [VAL_BOOL]  = "VAL_BOL",
  [VAL_BYTE]  = "VAL_BYT",
  [VAL_INT]   = "VAL_INT",
  [VAL_STATE] = "VAL_STA"
};

char* loadConfigs[] = {
  [LOAD_REG] = "LOAD_REG",
  [LOAD_IMM] = "LOAD_IMM",
  [LOAD_ADR] = "LOAD_ADR",
  [LOAD_RAA] = "LOAD_RAA"
};

char* storeConfigs[] = {
  [STORE_ADR] = "STORE_ADR",
  [STORE_RAA] = "STORE_RAA"
};

void disassembleInstruction(uint32_t bitInstruction, FILE* outstream) {
  unsigned int op_code = (bitInstruction & 0xF0000000) >> 28;   // 1111 0000 0000 0000 0000 0000 0000 0000
  /* Test if the instruction is binary or not */
  if (op_code < OP_NOT) { // BINARY
      unsigned int rd = (bitInstruction & 0x3C00000) >> 22; // 0000 0011 1100 0000 0000 0000 0000 0000
      unsigned int cfg_mask = (bitInstruction & 0xC000000) >> 26; // 0000 1100 0000 0000 0000 0000 0000 0000
      unsigned int ra = (bitInstruction & 0x7800) >> 11;        // 0000 0000 0000 0000 0111 1000 0000 0000
      unsigned int rb = (bitInstruction & 0xF);                 // 0000 0000 0000 0000 0000 0000 0000 1111
      unsigned int imma = (bitInstruction & 0x3FF800) >> 11;      // 0000 0000 0011 1111 1111 1000 0000 0000
      unsigned int immb = (bitInstruction & 0x7FF);               // 0000 0000 0000 0000 0000 0111 1111 1111

      switch (cfg_mask) {
          case CFG_RR:
              printf("%8s - Config: %9s - Rd: %2u -   Ra: %5u -   Rb: %5u\n", codeNames[op_code], binConfigs[cfg_mask],
                     rd, ra, rb);
              break;
          case CFG_RI:
              printf("%8s - Config: %9s - Rd: %2u -   Ra: %5u - Immb: %5u\n", codeNames[op_code], binConfigs[cfg_mask],
                     rd, ra, immb);
              break;
          case CFG_IR:
              printf("%8s - Config: %9s - Rd: %2u - Imma: %5u -   Rb: %5u\n", codeNames[op_code], binConfigs[cfg_mask],
                     rd, imma, rb);
              break;
          case CFG_II:
              printf("%8s - Config: %9s - Rd: %2u - Imma: %5u - Immb: %5u\n", codeNames[op_code], binConfigs[cfg_mask],
                     rd, imma, immb);
              break;
          default:
              break; // Unreachable
      }
  } else if (op_code == OP_NOT) {
    unsigned int rd   = (bitInstruction & 0x0F00000) >> 20; // 0000 0000 1111 0000 0000 0000 0000 0000
    unsigned int ra   = (bitInstruction & 0x0000F);         // 0000 0000 0000 0000 0000 0000 0000 1111
    printf(WHT "  OP_NOT -                   - Rd: %2u -   Ra: %5u\n" RESET, rd, ra);
  } else if (op_code == OP_LOAD) {
    unsigned int cfg_mask = (bitInstruction & 0xC000000) >> 26; // 0000 1100 0000 0000 0000 0000 0000 0000
    unsigned int type     = (bitInstruction & 0x3000000) >> 24; // 0000 0011 0000 0000 0000 0000 0000 0000
    unsigned int rd       = (bitInstruction & 0x0F00000) >> 20; // 0000 0000 1111 0000 0000 0000 0000 0000
    unsigned int ra   = (bitInstruction & 0x0000F);             // 0000 0000 0000 0000 0000 0000 0000 1111
    unsigned int imma = (bitInstruction & 0x007FF);             // 0000 0000 0000 0000 0000 0111 1111 1111
    unsigned int addr = (bitInstruction & 0xFFFFF);             // 0000 0000 0000 1111 1111 1111 1111 1111

    switch (cfg_mask) {
      case LOAD_REG: printf(GRN " OP_LOAD - Config: %9s - Rd: %2u -   Ra: %5u\n" RESET, loadConfigs[cfg_mask], rd, ra); break;
      case LOAD_IMM: printf(GRN " OP_LOAD - Config: %9s - Rd: %2u - Imma: %5u\n" RESET, loadConfigs[cfg_mask], rd, imma); break;
      case LOAD_ADR: printf(GRN " OP_LOAD - Config: %9s - Rd: %2u - Addr: %5u - Type: %5s\n" RESET, loadConfigs[cfg_mask], rd, addr, typeConfigs[type]); break;
      case LOAD_RAA: printf(CYN " OP_LOAD - Config: %9s - Rd: %2u -   Ra: %5u - Type: %5s\n" RESET, loadConfigs[cfg_mask], rd, ra, typeConfigs[type]); break;
      default: break; // Unreachable
    }
  } else if (op_code == OP_STORE) { // STORE
    unsigned int cfg_mask = (bitInstruction & 0xC000000) >> 26; // 0000 1100 0000 0000 0000 0000 0000 0000
    unsigned int type     = (bitInstruction & 0x3000000) >> 24; // 0000 0011 0000 0000 0000 0000 0000 0000
    unsigned int rd       = (bitInstruction & 0x0F00000) >> 20; // 0000 0000 1111 0000 0000 0000 0000 0000
    unsigned int ra   = (bitInstruction & 0x0000F);             // 0000 0000 0000 0000 0000 0000 0000 1111
    unsigned int addr = (bitInstruction & 0xFFFFF);             // 0000 0000 0000 1111 1111 1111 1111 1111
    switch(cfg_mask) {
      case STORE_ADR: printf(RED "OP_STORE - Config: %9s - Rd: %2u - Addr: %5u - Type: %5s\n" RESET, storeConfigs[cfg_mask], rd, addr, typeConfigs[type]); break;
      case STORE_RAA: printf(MAG "OP_STORE - Config: %9s - Rd: %2u -   Ra: %5u - Type: %5s\n" RESET, storeConfigs[cfg_mask], rd, ra, typeConfigs[type]); break;
      default: break;
    }
  } else if (op_code == OP_JMP) { // JMP
    unsigned int rd   = (bitInstruction & 0xF000000 ) >> 24;       // 0000 1111 0000 0000 0000 0000 0000 0000
    unsigned int addr = (bitInstruction & 0x0FFFFFF );             // 0000 0000 1111 1111 1111 1111 1111 1111
    printf(YEL "%8s -                   - Rd: %2u - Addr: %5u\n" RESET, codeNames[op_code], rd, addr);
  }
}

void showTableState(Table* table) {
  printf("=== Global Variables Hash Table ===\n");
  for (int i = 0 ; i < table->capacity ; i++) {
    if (!IS_NIL(table->entries[i].value)) {
      printf("[%2i] - Variable named %8s with value '", i, table->entries[i].key->chars);
      printValue(table->entries[i].value);
      printf("' at address %u\n", table->entries[i].address);
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

    if (i == topTempRegister->number && i == topGlobRegister->number) {
      printf(" << TOP Temp | TOP Glob\n");
    } else if (i == topTempRegister->number) {
      printf(" < TOP Temp\n");
    } else if (i == topGlobRegister->number) {
      printf(" < TOP Glob\n");
    } else {
      printf("\n");
    }
  }
  printf("=== --------------- ===\n");
}

void disassembleChunk(Chunk* chunk, FILE* outstream) {
  printf("=== Disassembling resulting chunk ===\n");
  for (int i = 0 ; i < chunk->capacity ; i++) {
    disassembleInstruction(chunk->instructions[i], outstream);
  }
  printf("=== ----------------------------- ===\n");
}
