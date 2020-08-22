#ifndef UTIL_H
#define UTIL_H

/**
 * This Module contains MOST of the utility macros and reserved
 * words for the other modules
*/


/* Consts */
#define MAX_CELLS_SIZE 14

#define MAX_FILE_NAME 100

#define ILLEGAL -1

#define TRUE 1
#define FALSE 0
#define MAX_LINE_LENGTH 80
#define MAX_SYMBOL_SIZE 31

#define MAX_MEMORY_SIZE 256

#define MIN_IMMEDIATE_SIZE -2048
#define MAX_IMMEDIATE_SIZE 2047

#define MIN_NUM_SIZE -8192
#define MAX_NUM_SIZE 8191

#define MAX_LABEL_SIZE 32

#define DATA_TYPE 0
#define INSTRUCTION_TYPE 1

#define DEST 1
#define SRC 0

#define MAX_OPERAND_SIZE 20

/************************/
/*DATA TYPE MACROS*/
#define DATA 0
#define STRING 1
#define ENTRY 2
#define EXTERN 3
#define SYMBOL 4

#define ENTRY_ARRAY_LEN 4
/*END OF DATA TYPE MACROS*/
/************************/

/************************/
/*ADDRESSING METHOD MACROS*/
#define IMMEDIATE 0
#define DIRECT 1
#define JUMP 2
#define REGISTER 3
/*END OF ADDRESSING METHOD MACROS*/
/************************/

/************************/
/*MASK VALUES MACROS*/

#define NONE                0x0000

#define ERA_ABSOLUTE        0x0000
#define ERA_EXTERNAL        0x0001
#define ERA_RELOCATABLE     0x0002

#define IMMEDIATE_HEX_DEST  0x0000
#define DIRECT_HEX_DEST     0x0004
#define JUMP_HEX_DEST       0x0008
#define REGISTER_HEX_DEST   0x000C

#define IMMEDIATE_HEX_SRC   0x0000
#define DIRECT_HEX_SRC      0x0010
#define JUMP_HEX_SRC        0x0020
#define REGISTER_HEX_SRC    0x0030

#define IMMEDIATE_HEX_PARAM1  0x0000
#define DIRECT_HEX_PARAM1     0x1000
#define REGISTER_HEX_PARAM1   0x3000

#define IMMEDIATE_HEX_PARAM2  0x0000
#define DIRECT_HEX_PARAM2     0x0400
#define REGISTER_HEX_PARAM2   0x0C00

/************************/
/*OPCODE MACROS*/
#define MOV 0
#define CMP 1
#define ADD 2
#define SUB 3
#define NOT 4
#define CLR 5
#define LEA 6
#define INC 7
#define DEC 8
#define JMP 9
#define BNE 10
#define RED 11
#define PRN 12
#define JSR 13
#define RTS 14
#define STOP 15

#define TWO_OPERAND 2
#define ONE_OPERAND 1
#define ZERO_OPERAND 0

#define NUM_OF_OPCODES 16

/*END OF OPCODE MACROS*/
/************************/

/************************/
/*REGISTER MACROS*/
#define R0 0
#define R1 1
#define R2 2
#define R3 3
#define R4 4
#define R5 5
#define R6 6
#define R7 7

#define NUM_OF_REGISTERS 8
/*END OF REGISTER MACROS*/
/************************/

/************************/
/*SAVED WORDS MACROS*/
/* according to https://www.cs.virginia.edu/~evans/cs216/guides/x86.html*/

#define SAVED_PUSH "push"
#define SAVED_POP "pop"
#define SAVED_ADD "add"
#define SAVED_IMUL "imul"
#define SAVED_IDIV "idiv"
#define SAVED_AND "and"
#define SAVED_OR "or"
#define SAVED_XOR "xor"
#define SAVED_SHL "shl"
#define SAVED_SHR "shr"
#define SAVED_CALL "call"
#define SAVED_RET "ret"
#define SAVED_WORD_SIZE 12


/*END OF SAVED WORDS MACROS*/
/************************/

char *entries[4];

/*REGISTER FUNCTIONS*/
char *getRegisterName (int reg_num);
int getRegister_6bit (int reg_num);
unsigned int getRegisterValuebyName (char *reg, int operand);

unsigned int getIntegerHexVal(int i);
int getImmediateHexVal(char *i);
int getIntegerVal(char *i);

/*OPCODE FUNCTIONS*/
char *getOpCodeName (int opcode_id);
unsigned int getOpCodeMask (int opcode_id);

char *getSavedWord(int index);


#endif