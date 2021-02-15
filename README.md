# Simple DiVinE Compiler



SDVC is a compiler that transforms `sdve` files into a binary in a custom ISA. This binary is then used with SDVU (github.com/QDucasse/sdvu) to be processed on board.



## Installation

To install SDVC, clone the project to the desired location and build your `Makefile` with CMake:

```bash
$ git clone git@github.com:QDucasse/sdvc	
$ cd sdvc/
$ cmake CMakeList.txt
$ make
```

The resulting binary `sdvc` can be used to compile a new binary or disassemble a given binary:

```bash
$ ./sdvc -c <testfile>.sdve <binary>
$ ./sdvc -d <binary>
```

## Simple DiVinE

Simple DiVinE (SDVE) is a transformation of the DiVinE language to an SSA form. A Java "pre-compiler" based on ANTLR4 can be found here (https://github.com/plug-obp/dve-language) and was used to produce the SDVE BEEM benchmark (https://github.com/QDucasse/sdve-beem-benchmark). 

SDVE syntax:

```
// Global declarations
byte next = 0;
state {NCS(0), p1(1), p2(2), p3(3), CS(4)} P_0.state = 0;
byte P_0.my_place = 0;

// Processes -> To be distributed to each one of the cores
  process P_0
// Guardblock: Creation of the condition to validate in the guardcondition
	guardBlock
      temp bool t_0 = P_0.state == 0;

// Guardcondition: Condition to check before executing the effect
    guardCondition t_0;

// Effect: Apply if the condition is valid
	effect
      P_0.state = 1,
      P_0.my_place = next,
      temp int t_1 = next + 1,
      next = t_1;
```



## ISA

### Instructions 

The ISA is composed of 14 instructions:

| OP CODE | Usage                                                    | Representation |
| :-----: | -------------------------------------------------------- | -------------- |
|   ADD   | D = A + B                                                | 0000           |
|   SUB   | D = A - B                                                | 0001           |
|   MUL   | D = A * B                                                | 0010           |
|   DIV   | D = A / B                                                | 0011           |
|   MOD   | D = A % B                                                | 0100           |
|   AND   | D = A and B                                              | 0101           |
|   OR    | D = A or B                                               | 0110           |
|   LT    | D = A < B                                                | 0111           |
|   GT    | D = A > B                                                | 1000           |
|   EQ    | D = A == B                                               | 1001           |
|   NOT   | D = not A                                                | 1010           |
|   JMP   | Jump to immediate address (over effect)                  | 1011           |
|  STORE  | Store a given value back to memory                       | 1100           |
|  LOAD   | Load value from memory or another register in a register | 1101           |

Four configurations exist to perform a **binary operation** (**ADD** to **EQ**):

| Destination |       LHS       |       RHS       | Representation |
| :---------: | :-------------: | :-------------: | -------------- |
|  Register   |    Register     |    Register     | 00             |
|  Register   |    Register     | Immediate Value | 01             |
|  Register   | Immediate Value |    Register     | 10             |
|  Register   | Immediate Value | Immediate Value | 11             |

Four configurations are used to perform a **LOAD** operation:

| Destination |       Source        | Representation |
| :---------: | :-----------------: | -------------- |
|  Register   |      Register       | 00             |
|  Register   |   Immediate Value   | 01             |
|  Register   |       Address       | 10             |
|  Register   | Register as address | 11             |

Two configurations are used to perform a **STORE** operation:

|     Destination     |  Source  | Representation |
| :-----------------: | :------: | -------------- |
|       Address       | Register | 00             |
| Register as address | Register | 01             |

### Data Type

With `D` the destination, `A` the left-hand operand and `B` the right-hand operand. They all can represent different types of data such as:

- **Global variable**: Represented by an identifier such as `x`, `P1.state`. A configuration of ALL global variables is given as an input to the CPU and the modified (or maybe not) version of the configuration should be the output once all process have run.
- **Array access:** Arrays can be accessed and set using for example `array[0]` , `array[globalName]` or `array[t_245]`.
- **Temporary variables**: Represe products at full price through BoYata Direct at Amazon.com
  To qualify, participants agree to send us their experiencnted by an identifier beginning with `t_` such as `t_253`. Those variables are temporary and used only once in the whole program. Their values are kept in registers.
- **Immediate values**: Represented by numbers to add directly in the instruction.

Global variables are loaded in registers from the top and follow the following rules:
- **Any** global variable in a register is the correct one (rather than the one in BRAM)
- Temporaries **always** have the priority on globals so loading a new global should result in overwriting an older global (and storing it back)

### Register rules

The registers are used as a two-headed stack. Temporary variables are stacked from the bottom to the top while global variables are loaded from the top. If a temporary variable has to be loaded on a register with a loaded global variable, the global variable is stored back before accepting the temporary variable.

Array accesses use a special behavior. In the case of an array element assignment (`a[0] = ...` for example), the address is stored in a special register (called the address register). Otherwise, it is simply set as a temporary register.