#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "util.h"
#include "symbolTable.h"
#include "errorHandler.h"

struct symbol {
    int value;
    int hex;
    int type;
    symbolType s_type; /*External/Relocatable/Entry/None*/
    char *name;

    Symbol *next_symbol;
};

static Symbol *symbolList; /*The first symbol in the list*/

void addToSymbolTable(char *label, int type, int value, symbolType s_type, int asmCodeLine){
    Symbol *symbol;

    if(symbolList == NULL){
        symbolList = malloc(sizeof(Symbol));
        if(symbolList == NULL){
            reportToStderr("Crtical Error!! system memory alocation error in symbolTable.c", 27);
            return;
        }
        symbolList->value = value;
        symbolList->type = type;
        symbolList->name = malloc(sizeof(char *) * 32);
        if(symbolList->name == NULL){
            reportToStderr("Crtical Error!! system memory alocation error in symbolTable.c", 33);
            return;
        }
        strcpy(symbolList->name, label);
        symbolList->s_type = s_type;
        symbolList->next_symbol = NULL;
    } else {
        symbol = symbolList;

        while(symbol->next_symbol != NULL){
            if(strcmp(label, symbol->name) == 0 && s_type != External){
                addError("Error! Label already exists\n", asmCodeLine);
                return;
            }
            symbol = symbol->next_symbol;
        }
    }

    symbol->next_symbol = malloc(sizeof(Symbol));
    if(symbol->next_symbol == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in symbolTable.c", 51);
        return;
    }

    symbol->next_symbol->name = malloc(sizeof(char *) * MAX_LABEL_SIZE);
    if(symbol->next_symbol->name == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in symbolTable.c", 57);
        return;
    }

    symbol->next_symbol->value = value;
    symbol->next_symbol->type = type;
    symbol->next_symbol->s_type = s_type;
    strcpy(symbol->next_symbol->name, label);
    symbol->next_symbol->next_symbol = NULL;
}

void printSymbolList() {
    Symbol *symbol;
    for(symbol = symbolList; symbol != NULL; symbol = symbol->next_symbol){
        //
    }
}

void updateDataSymbols(int offset){
    Symbol *symbol;
    for(symbol = symbolList; symbol != NULL; symbol = symbol->next_symbol){
        if(symbol->type == DATA_TYPE){
            symbol->value = symbol->value + offset;
        }
    }
}

symbolType getSymbolType(char *label){
    Symbol *symbol;
    for(symbol = symbolList; symbol != NULL; symbol = symbol->next_symbol){
        if(strcmp(symbol->name, label) == 0) {
            return symbol->s_type;
        }
    }
    return NONE;
}

unsigned int findSymbolValue(char *label) {
    Symbol *symbol;
    for(symbol = symbolList; symbol != NULL; symbol = symbol->next_symbol){
        if(strcmp(symbol->name, label) == 0){
            symbol->hex = (symbol->value) << 2;
            symbol->hex = (symbol->hex) & 0xFFFC;
            symbol-> hex = getSymbolValue(symbol);
        }
        return symbol->hex;
    }
    return ILLEGAL;
}

unsigned int getSymbolValue(Symbol *symbol) {
    switch(symbol->s_type) {
        case Relocatable:
            return symbol->hex | ERA_RELOCATABLE;
        case Entry:
           return symbol->hex | ERA_RELOCATABLE;
        case External:
            return symbol->hex | ERA_EXTERNAL;
        default:
            return symbol->hex | ERA_ABSOLUTE;
    }
}

void updateTableEntries(char *arr[], int size, int asmLine){
    Symbol *symbol;
    int i;
    for(symbol = symbolList; symbol != NULL; symbol = symbol->next_symbol){
        for(i = 0; i < size; i++){
            if(strcmp(arr[i],symbol->name) == 0 && symbol -> s_type == External){
                addError("Error! External labels cannot be declared as entries!", asmLine);
                return;
            }
            symbol->s_type = Entry;
        }
    }
}

int hasEntryValue(){
    Symbol *symbol;
    for(symbol = symbolList; symbol != NULL; symbol = symbol->next_symbol){
        if(symbol->s_type == Entry){
            return TRUE;
        }
    }
    return FALSE;
}

void printTableEntries(FILE *file){
    Symbol *symbol;
    for(symbol = symbolList; symbol != NULL; symbol = symbol->next_symbol){
        if(symbol->s_type == Entry){
            fprintf(file, "%s\t%d\n", symbol->name, symbol->value);
        }
    }
}

void freeSymbolList() {
    Symbol *symbol;
    while (symbolList != NULL){
        symbol = symbolList;
        symbolList = symbolList->next_symbol;
        free(symbol);
    }
}

