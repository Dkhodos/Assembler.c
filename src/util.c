#include "util.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct opcode {
    char *opcode_name;
    unsigned int opcode_mask;
    
}Opcode;

typedef struct reg {
    char *name;
    unsigned int dest_val;
    unsigned int src_val;
}Register;
        
Opcode opCodeArray[] = { {"mov", 0x0000},
                         {"cmp",   0x0040},
                         {"add",   0x0080},
                         {"sub",   0x00C0},
                         {"not",   0x0100},
                         {"clr",   0x0140},
                         {"lea",   0x0180},
                         {"inc",   0x01C0},
                         {"dec",   0x0200},
                         {"jmp",   0x0240},
                         {"bne",   0x0280},
                         {"red",   0x02C0},
                         {"prn",   0x0300},
                         {"jsr",   0x0340},
                         {"rts",   0x0380},
                         {"stop",  0x03C0}
                        };
                             /*dest |  src  */  
Register regArray[] = {{"r0", 0x0000, 0x0000},
                        {"r1", 0x0004, 0x0100},
                        {"r2", 0x0008, 0x0200},
                        {"r3", 0x000C, 0x0300},
                        {"r4", 0x0010, 0x0400},
                        {"r5", 0x0014, 0x0500},
                        {"r6", 0x0018, 0x0600},
                        {"r7", 0x001C, 0x0700}
};

char* savedWords[] = {SAVED_PUSH,
                      SAVED_POP,
                      SAVED_ADD,
                      SAVED_IMUL,
                      SAVED_IDIV
                      SAVED_AND,
                      SAVED_OR,
                      SAVED_XOR,
                      SAVED_SHL,
                      SAVED_SHR,
                      SAVED_CALL,
                      SAVED_RET
};

char *entryArray[] = {".data",
                    ".string",
                    ".entry",
                    ".extern"};


char *getOpCodeName (int opcode_id) {
    return opCodeArray[opcode_id].opcode_name;
}

/**
 * getOpCodeMask - returns the opcode_id HEX mask value
*/

unsigned int getOpCodeMask (int opcode_id) {
    return opCodeArray[opcode_id].opcode_mask;
}

/**
 * getRegisterName - returns the register full name (r#)
*/

char *getRegisterName (int reg_num) {
    return regArray[reg_num].name;
}

/**
 * getSavedName - returns the saved name
*/
char *getSavedWord(int index){
    return savedWords[index];
}

/**
 * getRegisterValuebyName - returns the HEX mask value of
 * a register.
 * operand: flags if the register is a source operand or a destination operand
*/

unsigned int getRegisterValuebyName (char *reg, int operand) {
    char index;
    int i_index;
    index = reg[strlen(reg)-1];
    i_index = index - '0';
    
    
    if(operand == DEST) {
        
        return regArray[i_index].dest_val;
    }else {
        
        return regArray[i_index].src_val;
    }
    
}

/**
 * getIntegerVal - returns the integer value of expression *i
*/
int getIntegerVal(char *i) {
    i++;
    return strtol(i, NULL, 10);
}

/**
 * getIntegerHexVal - returns the two's compliment form of i 
*/

unsigned int getIntegerHexVal(int i) {
    
    return (i > 0) ? i : ~(-i) + 1;
}

/**
 * getImmediateHexVal - returns the two's compliment form of immediate value *i
*/

int getImmediateHexVal(char *i) {
    int converted;
    
    i++;
    
    converted = (i[0] != '-') ? strtol(i, NULL, 10) : (~(strtol(i+1, NULL, 10)) + 1);
    converted = (converted << 2) & 0xFFFC; /*No need to mask HEX value with ERA bits
                                             - Immediate values are absolute and the 2 LSBs will be 0*/
    return converted;
}

