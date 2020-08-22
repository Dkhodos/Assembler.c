#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

typedef struct symbol Symbol;
typedef enum symbol_type {Relocatable, External, Entry, None} symbolType;

void addToSymbolTable(char *label, int type, int value, symbolType s_type, int asmCodeLine);

void printSymbolList();

void updateDataSymbols(int offset);

symbolType getSymbolType(char *label);

unsigned int findSymbolValue(char *label);

unsigned int getSymbolValue(Symbol *symbol);

void updateTableEntries(char *arr[], int size, int asmLine);

int hasEntryValue();

void printTableEntries(FILE *file);

void freeSymbolList();

#endif

