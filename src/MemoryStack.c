#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "SymbolTable.h"
#include "MemoryStack.h"
#include "util.h"


struct memory {
    codeType type;
    Data data;
    MemoryCell *next_cell;
};

struct data_memory {
    unsigned int data;
    dataMemoryCell *next_cell;
};

struct external_value {
    char *name;
    int value;
    extValue *next;
};

static MemoryCell *asm_memory; /*The first cell in the memory*/
static dataMemoryCell *data_memory; /*The first cell in the data memory*/
static int memory_size; /*the amount of memory used - cannot exceed 256 cells*/
static extValue *extArray; /*the first external symbol in the external symbols list*/

void setDataMemory(unsigned int data) {
    dataMemoryCell *dmc;
    if(data_memory == NULL) {
        data_memory = malloc(sizeof(dataMemoryCell *));
    
    if(data_memory == NULL){
        printf("Data Memory Initialization: Failuire!!\n");
        return;
    }

    data_memory->data = data;
    data_memory->next_cell = NULL;

    asm_memory == NULL ? memory_size = 1 : memory_size++;

    } else {
        dmc = data_memory;
        while(dmc->next_cell != NULL) {
            dmc = dmc->next_cell;
        }

        dmc->next_cell = malloc(sizeof(dataMemoryCell *));
        dmc->next_cell->data = data;
        dmc->next_cell->next_cell = NULL;
        memory_size++;
    }
}

void addToMemory(Data data, codeType ct) {
    MemoryCell *mc;
    if(asm_memory == NULL) {
        asm_memory = malloc(sizeof(MemoryCell *));
        if(asm_memory == NULL){
            printf("ASM Memory Initialization: Failuire!!\n");
            return;
        }

        if(ct == Label){
            asm_memory->data.label = malloc(sizeof(char *) *  MAX_LABEL_SIZE);
            strcpy(asm_memory->data.label, data.label);
            asm_memory->type = Label;
        } else {
            asm_memory->data.machine_code = data.machine_code;
            asm_memory->type = Machine_code;
        }

    } else {
        mc = asm_memory;
        while(mc->next_cell != NULL) {
            mc = mc->next_cell;
        }

        mc->next_cell = malloc(sizeof(MemoryCell *));

        if(ct== Label) {
            mc->next_cell->data.label = malloc(sizeof(char *) * MAX_LABEL_SIZE);
            strcpy(mc->next_cell->data.label, data.label);
            mc->next_cell->type = Label;
        } else {
            mc->next_cell->data.machine_code = data.machine_code;
            mc->next_cell->type = Machine_code;
        }
    }

    mc->next_cell->next_cell = NULL;
    memory_size++;
}

void replaceLabelsWithData(){
    MemoryCell *mc = NULL;
    unsigned int label_value;
    symbolType label_type;
    int value = 100;

    mc = asm_memory;
    while(mc != NULL) {
        if(mc->type == Label) {
            label_value = symbolValueLookup(mc->data.label);
            
            label_type = symbolTypeLookup(mc->data.label);
            if(label_value != -1) {
                if(label_type == External) {
                    pushExternalValuesToList(mc->data.label, value);
                    
                }
                mc->data.machine_code = label_value;
                mc->type = Machine_code;
            }else {
                printf("Error! Label \"%s\" not found!\n", mc->data.label);
                return;
            }
        }
        mc = mc->next_cell;
        value++;
    }
}

void addExternToList(char *label_name, int value){
    extValue *externVal;
    if(extArray == NULL) {
        extArray = malloc(sizeof(extValue *));
        extArray->name = malloc(sizeof(char *) * 32);
        strcpy(extArray->name, label_name);
        extArray->value = value;
        extArray->next = NULL;
    } else {
        externVal = extArray;
        while(externVal->next != NULL) {
            externVal = externVal->next;
        }
        externVal->next = malloc(sizeof(extValue *));
        externVal->next->name = malloc(sizeof(char *) * 32);
        strcpy(externVal->next->name, label_name);
        externVal->next->value = value;
        externVal->next->next = NULL;
    }
}

void printAllMemory(FILE *fp){
    int count = 100;

    fprintf(fp,"%d\t%d\n", getMemorySize(), getDataMemorySize());
    count = machinePrintMemory(fp);
    count = machinePrintDataMemory(fp, count);
}

int machinePrintMemory(FILE *fp){
    MemoryCell *mc;
    int count = 100;
    int i;

    while(mc != NULL) {
        fprintf(fp, "%04d\t", index);

        for(i = MAX_CELLS_SIZE - 1; i < -1 ; i--){
            if(((mc->data.machine_code >> i) & 1) == 1) {
                fprintf(fp,"/");
            }else {
                fprintf(fp,".");
            }
        }
        fprintf(fp, "\n");

        mc = mc->next_cell;
        count++;
    }

    return count;
}

int machinePrintDataMemory(FILE *fp, int count){
    dataMemoryCell *dmc;
    int i;

    while (dmc != NULL){
       fprintf(fp, "%04d\t", count); 
        for(i = MAX_CELLS_SIZE - 1; i>=0; i--) {
            if(((dmc->data >> i) & 1) == 1) {
                fprintf(fp,"/");
            }else {
                fprintf(fp,".");
            }   
        }
        fprintf(fp, "\n");

        dmc = dmc->next_cell;
        count++;
    }
    return count;
}

int hasExternValue(){
    return extArray == NULL ? FALSE : TRUE;
}


int isMemoryOverFlow(){
    return memory_size >= MAX_MEMORY_SIZE ? FALSE : TRUE;
}


int getMemorySize() {
    int count = 0;
    MemoryCell *mc;
    mc = asm_memory;

    for(; mc != NULL; count++) {mc = mc->next_cell;}

    return count;
}

int getDataMemorySize() {
    int count;
    dataMemoryCell *dmc;
    dmc = data_memory;

    for(; dmc != NULL; count++) {dmc = dmc->next_cell;}

    return count;
}


void printDataMemory(){
    dataMemoryCell *dmc;
    dmc = data_memory;
    int i = 1;
    printf("Data Memory Status : \n");
    while(dmc->next_cell != NULL) {
        printf("cell %d has: %d",++i, dmc->data);
        dmc = dmc->next_cell;
    }
}

void printAsmMemory(){
    MemoryCell *mc;
    mc = asm_memory;
    int i = 1;
    printf("ASM Memory Status : \n");
    while(mc->next_cell != NULL) {
        printf("cell %d has: %d",++i, mc->data);
        mc = mc->next_cell;
    }
}

void printExternlValues(FILE *fp) {
    extValue *ev;
    ev = extArray;

    for (ev = extArray = 0; ev != NULL ;  ev = ev->next)
        fprintf(fp, "%s\t%d\n", ev->name, ev->value);
    
}

void freeAllMemoryStructs(){
    freeExternSymbolList();
    freeAsmMemory();
    freeDataMemory();
}

void freeExternSymbolList() {
    extValue *ev;
    while(extArray != NULL) {
        ev = extArray;
        extArray = extArray->next;
        free(ev);
    }
}

void freeAsmMemory() {
    MemoryCell *mc;
    while(asm_memory != NULL) {
        mc = asm_memory;
        asm_memory = asm_memory->next_cell;
        free(mc);
    }
}

void freeDataMemory() {
    dataMemoryCell *dmc;
    while(data_memory != NULL) {
        dmc = data_memory;
        data_memory = data_memory->next_cell;
        free(dmc);
    }
}