## Instruction Set Architecture of SDVU

---

While the SDVU compiler requires few lookahead in its instructions, they still have to handle several cases.

### Operations				

| OP CODE | Usage                                  |
| :-----: | -------------------------------------- |
|   ADD   | D = A + B                              |
|   SUB   | D = A - B                              |
|   MUL   | D = A * B                              |
|   DIV   | D = A / B                              |
|   MOD   | D = A % B                              |
|   AND   | D = A and B                            |
|   OR    | D = A or B                             |
|   LT    | D = A < B                              |
|  LTEQ   | D = A <= B                             |
|   GT    | D = A > B                              |
|  GTEQ   | D = A >= B                             |
|   EQ    | D = A == B                             |
|   NEQ   | D = A != B                             |
|   JMP   | JMP to immediate address (over effect) |
|  STORE  | Store value into global                |
|  LOAD   | Load value from global                 |

### Data Type

With `D` the destination, `A` the left-hand operand and `B` the right-hand operand. They all can represent different types of data such as:

- **Global variable**: Represented by an identifier such as `x`, `P1.state` or `b[4]`. A configuration of ALL global variables is given as an input to the CPU and the modified (or maybe not) version of the configuration should be the output once all process have run.
- **Temporary variables**: Represented by an identifier beginning with `t_` such as `t_253`. Those variables are temporary and used only once in the whole program. Their values are kept in registers.
- **Immediate values**: Represented by numbers to add directly in the instruction.

Using `G` for *global variables*, `T` for *temporary variables* and `I` for *immediate values*, the different combinations are the following:

| Destination | LHS  | RHS  |
| :---------: | :--: | :--: |
|      T      | Imm  |  T   |
|      T      | Imm  |  G   |
|      T      |  T   |  T   |
|      T      |  T   |  G   |
|      T      |  T   | Imm  |
|      T      |  G   |  T   |
|      T      |  G   |  G   |
|      T      |  G   | Imm  |
|      G      | Imm  |  T   |
|      G      | Imm  |  G   |
|      G      |  T   |  T   |
|      G      |  T   |  G   |
|      G      |  T   | Imm  |
|      G      |  G   |  T   |
|      G      |  G   |  G   |
|      G      |  G   | Imm  |

### Instruction Composition

Each instruction holds 32 bits. In the case of a binary operation (`ADD` to `NEQ`), it is composed of an `OP_CODE` (4 bits), a configuration bitmask (4 bits) and a representation of the three data types (8 bits each). 



