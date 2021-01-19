## Instruction Set Architecture of SDVU

---

While the SDVU compiler requires few lookahead in its instructions, they still have to handle several cases.

### Operations				

| OP CODE | Usage                                  | Representation |
| :-----: | -------------------------------------- | -------------- |
|   ADD   | D = A + B                              | 0000           |
|   SUB   | D = A - B                              | 0001           |
|   MUL   | D = A * B                              | 0010           |
|   DIV   | D = A / B                              | 0011           |
|   MOD   | D = A % B                              | 0100           |
|   AND   | D = A and B                            | 0101           |
|   OR    | D = A or B                             | 0110           |
|   LT    | D = A < B                              | 0111           |
|  LTEQ   | D = A <= B                             | 1000           |
|   GT    | D = A > B                              | 1001           |
|  GTEQ   | D = A >= B                             | 1010           |
|   EQ    | D = A == B                             | 1011           |
|   NEQ   | D = A != B                             | 1100           |
|   JMP   | JMP to immediate address (over effect) | 1101           |
|  STORE  | Store value into global                | 1110           |
|  LOAD   | Load value from globalG                | 1111           |

### Data Type

With `D` the destination, `A` the left-hand operand and `B` the right-hand operand. They all can represent different types of data such as:

- **Global variable**: Represented by an identifier such as `x`, `P1.state` or `b[4]`. A configuration of ALL global variables is given as an input to the CPU and the modified (or maybe not) version of the configuration should be the output once all process have run.
- **Temporary variables**: Represented by an identifier beginning with `t_` such as `t_253`. Those variables are temporary and used only once in the whole program. Their values are kept in registers.
- **Immediate values**: Represented by numbers to add directly in the instruction.

Global variables are loaded in registers from the top and follow the following rules:
- Global variables are using the registers as a **FIFO** as long as they do not interfere with temporaries
- **Any** global variable in a register is the correct one (rather than the one in BRAM)
- Temporaries **always** have the priority on globals so loading a new global should result in overwriting an older global (and storing it back)

Four configurations exist to perform a binary operation:

| Destination | LHS  | RHS  | Representation |
| :---------: | :--: | :--: | -------------- |
|      R      |  R   |  R   | 00             |
|      R      |  R   | Imm  | 01             |
|      R      | Imm  |  R   | 10             |
|      R      | Imm  | Imm  | 11             |

### Instruction Composition

Each instruction holds 32 bits.

In the case of a binary operation (`ADD` to `NEQ`), it is composed of an `OP_CODE` (4 bits), a configuration bitmask (4 bits) and a representation of the three data types (8 bits each).

For example:

```
x = t_251 + t_252
R     R       R

ADD    Config mask   Destination (index 15)   LHS operand (reg0)     RHS operand (reg1)     
0000      00                1111                 0000000 0001            0000000 0010        

---

x = t_253 + 8
R     R    Imm

ADD    Config mask   Destination (index 15)   LHS operand (reg2)     RHS operand (Imm value = 8)
0000      01                1111                 0000000 0011             00000001000

---

t_1 = 255 + 8
R   Imm  Imm

ADD    Config mask   Destination (reg0)   LHS operand (Imm value = 255)     RHS operand (Imm value = 8)
0000      11                0001                  00011111111                     00000001000
```

In the case of the `JMP` operation:

```
JMP             Address
1101  00011100011100011100011100011
```

And the `STORE` and `LOAD`:

```
STORE  Destination (index 15)               Address
1110           1111                 000111000111000111000111
```
