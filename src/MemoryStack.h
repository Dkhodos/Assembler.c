#ifndef MEMORY_H
#define MEMORY_H

extern FILE *ext_fp;

typedef enum data_type {Machine_code, Label} codeType;

typedef struct memory MemoryCell;
typedef struct data_memory dataMemoryCell;
typedef struct external_value extValue;

typedef union data {
    unsigned int machine_code;
    char *label;
} Data;

void setDataMemory(unsigned int data);

void addToMemory(Data data, codeType ct);

void replaceLabelsWithData();

void addExternToList(char *label_name, int value);

void printAllMemory(FILE *fp);

int machinePrintMemory(FILE *fp);

int machinePrintDataMemory(FILE *fp, int count);

int hasExternValue();

int isMemoryOverFlow();

int getMemorySize();

int getDataMemorySize();

void printDataMemory();

void printAsmMemory();

void printExternlValues(FILE *fp);

void freeAllMemoryStructs();

void freeExternSymbolList();

void freeAsmMemory();

void freeDataMemory();


#endif
