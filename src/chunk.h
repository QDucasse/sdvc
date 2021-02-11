
#ifndef sdvu_instruction_h
#define sdvu_instruction_h

#include "common.h"
#include "register.h"
#include "value.h"

/* OPCODE constants */
#define OP_ADD   0b0000
#define OP_SUB   0b0001
#define OP_MUL   0b0010
#define OP_DIV   0b0011
#define OP_MOD   0b0100
#define OP_AND   0b0101
#define OP_OR    0b0110
#define OP_LT    0b0111
#define OP_LTEQ  0b1000
#define OP_GT    0b1001
#define OP_GTEQ  0b1010
#define OP_EQ    0b1011
#define OP_NEQ   0b1100
#define OP_JMP   0b1101
#define OP_STORE 0b1110
#define OP_LOAD  0b1111

/* CONFIG bitmasks for binary operations */
#define CFG_RR  0b00
#define CFG_RI  0b01
#define CFG_IR  0b10
#define CFG_II  0b11

/* Config bitmasks for the LOAD operation */
#define LOAD_REG 0b00
#define LOAD_IMM 0b01
#define LOAD_ADR 0b10
#define LOAD_RAA 0b11

/* Config Bitmasks for the STORE operation */
#define STORE_ADR 0b00
#define STORE_RAA 0b01

/* Chunk of instructions definition */
typedef struct {
  int count;              // Number of allocated entries in use
  int capacity;           // Size of the chunk
  uint32_t* instructions; // Actual raw 32-bits instructions
} Chunk;

/* Instruction config */
typedef struct {
  unsigned int op_code: 4;  // Operation code
  unsigned int cfg_mask: 2; // Config ; RR, RI or IR for binary ; REG, IMM or ADR for LOAD
  unsigned int rd: 4;       // Destination register
  unsigned int ra: 4;       // Source register A
  unsigned int rb: 4;       // Source register B
  unsigned int imma: 11;    // Immediate value A
  unsigned int immb: 11;    // Immediate value B
  unsigned int addr: 24;    // Address
  unsigned int type: 2;     // Type of the value to load/store (bool, byte, int or state)
} Instruction;

/* Initialize a given chunk */
Chunk* initChunk();
/* Free the given chunk */
void freeChunk(Chunk* chunk);
/* Write an instruction to the given chunk */
void writeChunk(Chunk* chunk, uint32_t);

/* Initialization struct initialization */
Instruction* initInstruction();
/* Free the given instruction */
void freeInstruction(Instruction* instruction);
/* Convert an instruction to a uint32_t counterpart */
uint32_t instructionToUint32(Instruction* instruction);
/* Binary instruction of form Register Register */
uint32_t binaryInstructionRR(Instruction* instruction, unsigned int op_code, unsigned int rd, unsigned int ra, unsigned int rb);
/* Binary instruction of form Register Immediate */
uint32_t binaryInstructionRI(Instruction* instruction, unsigned int op_code, unsigned int rd, unsigned int ra, unsigned int imm);
/* Binary instruction of form Immediate Register */
uint32_t binaryInstructionIR(Instruction* instruction, unsigned int op_code, unsigned int rd, unsigned int imm, unsigned int rb);
/* Binary instruction of form Immediate Register */
uint32_t binaryInstructionII(Instruction* instruction, unsigned int op_code, unsigned int rd, unsigned int imma, unsigned int immb);
/* Store Instruction */
uint32_t storeInstruction(Instruction* instruction,  unsigned int rd, unsigned int addr, unsigned int type);
/* Jump Instruction */
uint32_t jumpInstruction(Instruction* instruction,  unsigned int rd, unsigned int addr);
/* Emit reset jump */
uint32_t resetJump(Instruction* instruction);
/* Load from a register */
uint32_t loadInstructionReg(Instruction* instruction, unsigned int rd, unsigned int ra);
/* Load from an immediate value */
uint32_t loadInstructionImm(Instruction* instruction, unsigned int rd, unsigned int imma);
/* Load from an address */
uint32_t loadInstructionAddr(Instruction* instruction, unsigned int rd, unsigned int addr, unsigned int type);
/* Write a store instruction from a register */
void writeStoreFromRegister(Register* reg, Chunk* chunk);
/* Write a load instruction from a register */
void writeLoadFromRegister(Register* reg, Chunk* chunk);

#endif
