#ifndef ASM_OBJECT_H
#define ASM_OBJECT_H

#include <stdio.h>

typedef struct asm_object asmObject;

asmObject* construct(FILE *f);

void kill(asmObject *obj);

char* setLine(asmObject *obj);

char* getLine(asmObject *obj);

char* getWord(asmObject *obj);

int getLineSize (asmObject *obj);

char* readWord(asmObject *obj);

int findInstruction(char *word);

int findOpCode(char *word);

int findRegister(char *word);

int getNumOfOparands(int opCode);

int isSavedWord(char* word);

int getOperandType(char *op);

unsigned int maskParam (unsigned int src, unsigned int dest);

unsigned int getMaskParamSrc(unsigned int src);

unsigned int getMaskParamDest(unsigned int dest);

void addOperands(char *src, char *dest, int srcType, int destType, asmObject *obj);

void handleOperandSrc(char *src, int srcType, asmObject *obj);

void handleOperandDest(char *dest, int destType, asmObject *obj);

unsigned int maskOperand (unsigned int src, unsigned int dest);

unsigned int maskOperandSrc(unsigned int src);

unsigned int maskOperandDest(unsigned int dest);

unsigned int maskInstruction(asmObject *obj);

void addInstructionToMemory(asmObject *obj);

void addInstructionNoParenthese(asmObject *obj);

void addInstructionParenthese(asmObject *obj);

int isValidAdreesingSingleOp(int opCode, int firstOpType);

int isValidAdreesingTwoOp(int opCode, int firstOpType, int secondOpType);

void AddSingleOperandInstruction(asmObject *obj);

void AddTwoOperandInstruction(asmObject *obj);

void addOpCode(asmObject *obj);

void handleOperandCount(int opCode, int operandCount, asmObject *obj);

void addNumberToDataMemory(asmObject *obj);

void addStringToDataMemory(asmObject *obj);

void addLabelToDataMemory(asmObject *obj);

int memoryDataCheckLabel(char* label, int wordSize,asmObject *obj);

int memoryDataCheckInstruction(asmObject *obj);

void addFileToMemory(asmObject *obj);

int memoryDataCheckOpCode(asmObject *obj);

void addExternToSymbolTable(asmObject *obj);

void addEntry(asmObject *obj);

int isValidLabel(char *label);





#endif