#include<string.h>
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<errno.h>

#include "util.h"
#include "symbolTable.h"
#include "memoryStack.h"
#include "asmObject.h"
#include "errorHandler.h"

struct asm_object {
    FILE *asmFile; /*The current asm file */
    char* line; /* the current asm line*/
    char* word; /* the current word*/

    int lineSize;
    int instructionSize;

    int newIC;
    int currentIC;
    int newDC;
    int currentDC;

    int isLabel;
    char *labelName;
    char *labelType;

    int opCode;

    unsigned int ERAMask; /*Holds the appropriate ERA bits HEX masking value*/
    unsigned int operandMask; /*Same, for operands*/
    unsigned int opcodeMask; /*Same, for opcode*/
    unsigned int paramMask;  /*Same, for parameters (for jmp-like instructions)*/

    Data instructionData; /*An union that can hold the machine code/label string to be passed to the memory*/
    codeType instructionType;  /*A flag to signal if instruction_data holds a machine_code value or a label value*/

    int isValidCode;

    char *labels[MAX_MEMORY_SIZE];
    int labelsSize;
};

asmObject* construct(FILE *f){
    asmObject* newObject;

    newObject = malloc(sizeof(asmObject));
    if(newObject == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asmObject.c",49);
        return NULL;
    }
    newObject->asmFile = f;
    newObject->line = malloc(sizeof(char *) * MAX_LINE_LENGTH);
    if(newObject->line == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asmObject.c line",55);
        return NULL;
    }
    newObject->word = malloc(sizeof(char *) * MAX_LINE_LENGTH);
    if(newObject->word == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asmObject.c line",60);
        return NULL;
    }

    newObject->lineSize = 0;
    newObject->newIC = 100;
    newObject->currentIC = 0;
    newObject->newDC = 0;
    newObject->currentDC = 0;
    newObject->instructionSize = 0;

    newObject->isLabel = FALSE;
    newObject->labelName = malloc(sizeof(char *) * MAX_LABEL_SIZE);
    if(newObject->labelName == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asmObject.c line",74);
        return NULL;
    }

    newObject->opCode = 0;
    newObject->instructionData.label = malloc(sizeof(char *) * MAX_LABEL_SIZE);
    if(newObject->instructionData.label == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asmObject.c line",81);
        return NULL;
    }
    newObject->labelsSize = 0;

    return newObject;
}


void kill(asmObject *obj){
    int i;
    char* label,*temp;

    free(obj->line);
    free(obj->word);
    free(obj->labelName);
    free(obj->instructionData.label);

    label = obj->labels;
    for (i =0; i < obj->labelsSize ; i++){
        free(obj->labels + i);
    }
    
    free(obj);

    // freeSymbolList();
    // freeExternalSymbolList();
    freeAllMemoryStructs();
}

char* setLine(asmObject *obj){
    char *state = fgets(obj->line, MAX_LINE_LENGTH, obj->asmFile);

    obj->labelsSize++;
    obj->instructionSize = 1;
    obj->isLabel = FALSE;
    obj->opCode = ILLEGAL;
    obj->isValidCode = FALSE;

    return state;
}

char* getLine(asmObject *obj) {
     return obj->line;
}

char* getWord(asmObject *obj) {
    return obj->word;
}

int getLineSize (asmObject *obj) {
    return obj->lineSize;
}

char* readWord(asmObject *obj){
    char* buffer;
    int i = 0;

    buffer = malloc(sizeof(char) * MAX_LINE_LENGTH);
    if(buffer == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asmObject.c line",141);
        return NULL;
    }

    while (obj->line[0] != EOF){
        if(isspace(obj->line[0])){
            obj->line = obj->line+1;
            continue;
        }

        while (isspace(obj->line[0]) && obj->line[0] != EOF){
            buffer[i] = obj->line[0];
            i++;
            obj->line = obj->line+1;
        }
        buffer[i] = EOF;
        strcpy(obj->word, buffer);
        
        return buffer;
    }
    return NULL;
}

int findInstruction(char *word){
    int i;

    for (i = 0; i < ENTRY_ARRAY_LEN; i++){
        if(strcmp(word, entries[i]) == 0){
            return i;
        }
    }
    return ILLEGAL;
}

int findOpCode(char *word){
    int i;

    for (i = 0; i < NUM_OF_OPCODES; i++){
        if(strcmp(word, getOpCodeName(i)) == 0){
            return i;
        }
    }
    return ILLEGAL;
}

int findRegister(char *word){
    int i;

    for (i = 0; i < NUM_OF_REGISTERS; i++){
        if(strcmp(word, getRegisterName(i)) == 0){
            return i;
        }
    }
    return ILLEGAL;
}

int getNumOfOparands(int opCode){
    if((opCode >= MOV && opCode <= SUB) || opCode == LEA){
        return TWO_OPERAND;
    }

    if(opCode == NOT || opCode == CLR || (opCode >= INC && opCode <= JSR)){
        return ONE_OPERAND;
    }

    return ZERO_OPERAND;
}

int isSavedWord(char* word){
    int i;
    for(i=0; i < SAVED_WORD_SIZE; i++){
        if(strcmp(word, getSavedWord(i)) == 0){
            return TRUE;
        }
    }
    return FALSE;
}


int getOperandType(char *op){
    if(op[0] == '#'){
        op ++;
    
        if( op[0] == '-'){
            op++;
    
            while (op[0] != EOF){
                if(!isdigit(op[0])){
                    return ILLEGAL;
                }
                op++;
            }
        }
        return IMMEDIATE;
    } else {
        if(findRegister(op) != ILLEGAL){
            return REGISTER;
        }

        if(isValidLabel(op)){
            return DIRECT;
        }

        return ILLEGAL;
    }
}

unsigned int maskParam (unsigned int src, unsigned int dest){
    return (getMaskParamSrc(src) | getMaskParamDest(dest));
}

unsigned int getMaskParamSrc(unsigned int src){
    switch(src) {
        case IMMEDIATE:
            return IMMEDIATE_HEX_PARAM1;
        case DIRECT:
            return DIRECT_HEX_PARAM1;
        case REGISTER:
            return REGISTER_HEX_PARAM1;
        default:
            return 0x0000;
    }
}

unsigned int getMaskParamDest(unsigned int dest){
    switch(dest) {
        case IMMEDIATE:
            return IMMEDIATE_HEX_PARAM2;
        case DIRECT:
            return DIRECT_HEX_PARAM2;
        case REGISTER:
            return REGISTER_HEX_PARAM2;
        default:
            return 0x0000;
    }
}

void addOperands(char *src, char *dest, int srcType, int destType, asmObject *obj){
    if(srcType == REGISTER && destType == REGISTER){
        obj-> instructionData.machine_code = findRegisterByName(src, SRC) | findRegisterByName(dest, DEST);
        obj->instructionType = Machine_code;
        addToMemory(obj->instructionData, obj->instructionType);
        obj->newIC++;
    } else {
        if(src){
            obj->newIC++;
            handleOperandSrc(src, srcType, obj);
        }
        if(dest){
            obj->newIC++;
            handleOperandDest(dest, destType, obj);
        }
    }
}

void handleOperandSrc(char *src, int srcType, asmObject *obj){
    int buffer;
    switch (srcType){
        case IMMEDIATE: {
            buffer = getAsmNumber(src);
            obj->instructionData.machine_code = getImmediateHex(src);
            obj->instructionType = Machine_code;
            if(buffer > MAX_IMMEDIATE_SIZE || buffer < MIN_IMMEDIATE_SIZE){
                addError("Error! Value of immediate operand exceeds allowed size!", obj->lineSize);
                return;
            }
            break;
        }
        case DIRECT: {
            strcpy(obj->instructionData.label, src);
            obj->instructionType = Label;
            break;
        }
        case REGISTER: {
            obj->instructionData.machine_code =  findRegisterByName(src, SRC);
            obj->instructionType = Machine_code;
            break;
        }
    }
    addToMemory(obj->instructionData, obj->instructionType);
}

void handleOperandDest(char *dest, int destType, asmObject *obj){
    int buffer;
    switch(destType){
        case IMMEDIATE: {
            buffer = getAsmNumber(dest);
            obj->instructionData.machine_code = getImmediateHex(dest);
            obj->instructionType = Machine_code;

            if(buffer > MAX_NUM_SIZE || buffer < MIN_NUM_SIZE) {
                addError("Error! Value of immediate operand exceeds allowed size!", obj->lineSize);
                return;
            }
        }
        case DIRECT: {
            obj->instructionData.label = malloc(sizeof(char) * MAX_LABEL_SIZE);
            if(obj->instructionData.label == NULL){
                reportToStderr("Crtical Error!! system memory alocation error in asmObject.c line",342);
                return;
            }
            strcpy(obj->instructionData.label, dest);
            obj->instructionType = Label;
            break;
        }
        case REGISTER: {
            obj->instructionData.machine_code = findRegisterByName(dest, DEST);
            obj->instructionType = Machine_code;
            break;
        }
    }
    addToMemory(obj->instructionData, obj->instructionType);
}


unsigned int maskOperand (unsigned int src, unsigned int dest) {
    return maskOperandSrc(src) | maskOperandDest(dest);
}

unsigned int maskOperandSrc(unsigned int src){
    switch(src){
        case DIRECT:
            return DIRECT_HEX_SRC;
        case REGISTER:
            return REGISTER_HEX_SRC;
        case IMMEDIATE:
        default:
            return 0x0000;
    }
}

unsigned int maskOperandDest(unsigned int dest){
    switch(dest){
        case DIRECT:
            return DIRECT_HEX_DEST;
        case JUMP:
            return JUMP_HEX_DEST;
        case REGISTER:
            return REGISTER_HEX_DEST;
        case IMMEDIATE:
        default:
            return 0x0000;
    }
}

unsigned int maskInstruction(asmObject *obj) {
    return (obj->ERAMask | obj->operandMask | obj->opcodeMask  | obj->paramMask);
}

void addInstructionToMemory(asmObject *obj){
    char *firstOcuurence;
    if(obj->line[0] == EOF){
        addError("Error! jmp, jsr, and bne require one operand!", obj->lineSize);
        return;
    }

    while (isspace(obj-> line[0]) != 0){
        obj->line++;
    }

    firstOcuurence = strchr(obj->line, '(');
    if(firstOcuurence == NULL){
        addInstructionNoParenthese(obj);
    } else {
        addInstructionParenthese(obj);
    }
}

void addInstructionNoParenthese(asmObject *obj){
    char* label;

    readWord(obj);
    if(!isValidLabel(obj->word)){
        addError("Error! jmp, jsr and bne instructions require a legit label!", obj->lineSize);
        return;
    }
    label = malloc(sizeof(char) * MAX_LABEL_SIZE);
    if(label == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asmObject.c line",419);
        return;
    }
    strcpy(label, obj-> word);
    if(readWord(obj) != NULL){
        addError("Error! jmp, jsr and bne instructions require ONLY one operand", obj->lineSize);
    }

    obj->isValidCode = TRUE;
    obj-> ERAMask = ERA_ABSOLUTE;
    obj->operandMask = DIRECT_HEX_DEST;
    obj->paramMask = maskOperand(0, 0);

    obj->instructionData.machine_code = maskInstruction(obj);
    obj->instructionType = Machine_code;
    addToMemory(obj->instructionData, obj->instructionType);

    obj->instructionData.label = malloc(sizeof(char *) * MAX_LABEL_SIZE);
    if(obj->instructionData.label  == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asmObject.c", 438);
        return;
    }
    strcpy(obj->instructionData.label, label);
    obj->instructionType = Label;
    addToMemory(obj->instructionData, obj->instructionType);

    obj->newIC+=2;
    free(label);
}

void addInstructionParenthese(asmObject *obj){
    char* label = malloc(sizeof(char *) * MAX_LABEL_SIZE);
    char *firstOp;
    int firstOpType;
    char *secondOp;
    int secontOpType;
    char *delimiter;

    int i =0;
    if(label == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asmObject.c",452);
        return;
    }

    while(obj->line[0] != "," && obj->line[0] != EOF){
        label[++i] = obj->line[0];
        obj->line++;
    }

    if(label == NULL){
        addError("Error! jmp, jsr and bne require a legit LABEL operand!", obj->lineSize);
        return;
    }

    if(!isValidLabel(label)){
        addError("Error! jmp, jsr and bne require a legit LABEL operand!", obj->lineSize);
        return;
    }

    delimiter = strchr(obj->line, ',');
    if(delimiter == NULL){
        addError("Error! jmp, jsr and bne require the following operand structure: LABEL(operand1, operand2)", obj->lineSize);
        return;
    }

    obj->line++;
    firstOp = malloc(sizeof(char *) * MAX_LINE_LENGTH);
    i=0;
    if(firstOp == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asmObject.c",74);
        return;
    }

    while(obj->line[0] != ')' && obj->line[0] != EOF) {
        firstOp[++i] = obj->line[0];
        obj->line++;
    }

    if(firstOp == NULL){
        addError("Error! jmp, jsr and bne require the following operand structure: LABEL(operand1, operand2)", obj->lineSize);
        return;
    }

    firstOpType = getOperandType(firstOp);
    if(firstOpType == ILLEGAL){
        addError("Error! First parameter of operand is ilegal!", obj->lineSize);
        return;
    }
    

    secondOp = malloc(sizeof(char *) * MAX_LINE_LENGTH);
    i=0;
    if(secondOp == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asmObject.c", 511);
        return;
    }

    while(obj->line[0] != ')' && obj->line[0] != EOF) {
        secondOp[++i] = obj->line[0];
        obj->line++;
    }

    if(secondOp == NULL){
        addError("Error! jmp, jsr and bne require the following operand structure: LABEL(operand1, operand2)", obj->lineSize);
        return;
    }

    secontOpType = getOperandType(secondOp);
    if(secontOpType == ILLEGAL){
        addError("Error! Second parameter of operand is ilegal!", obj->lineSize);
        return;
    }


    obj->line++;
    readWord(obj);
    if(readWord(obj) == NULL){
        obj-> isValidCode = TRUE;
        obj->ERAMask = ERA_ABSOLUTE;
        obj-> operandMask = JUMP_HEX_DEST;
        obj->paramMask = maskParam(firstOpType, secontOpType);

        obj->instructionData.machine_code = maskInstruction(obj);
        obj->instructionType = Machine_code;
        addToMemory(obj->instructionData, obj->instructionType);

        obj->instructionData.label = malloc(sizeof(char *) * MAX_LABEL_SIZE);
        if(obj->instructionData.label == NULL){
            reportToStderr("Crtical Error!! system memory alocation error in asmObject.c line",547);
            return;
        }
        strcpy(obj->instructionData.label, label);
        obj-> instructionType = Label;
        addToMemory(obj->instructionData, obj->instructionType);

        obj->newIC+=2;

        addOperands(firstOp, firstOpType, secondOp, secontOpType, obj);

        free(firstOp);
        free(secondOp);
        free(label);
    } else{
        addError("Error!, jmp, jsr and bne instructions require ONLY 1 operand!", obj->lineSize);
    }
}

int isValidAdreesingSingleOp(int opCode, int firstOpType){
    if(firstOpType == ILLEGAL){
        return FALSE;
    }

    if(opCode == PRN){
        return firstOpType == IMMEDIATE ? FALSE : TRUE;
    }

    return TRUE;
}

int isValidAdreesingTwoOp(int opCode, int firstOpType, int secondOpType){
    if(firstOpType == ILLEGAL || secondOpType == ILLEGAL){
        return FALSE;
    }

    if(opCode == LEA){
        if(firstOpType != DIRECT){
            return FALSE;
        }
        return secondOpType != DIRECT && secondOpType != REGISTER ? FALSE : TRUE;
    }

    if(opCode == CMP){
        return TRUE;
    }

    return secondOpType == IMMEDIATE ? TRUE : FALSE;
}

void AddSingleOperandInstruction(asmObject *obj){
    char *op = malloc(sizeof(char *) * MAX_OPERAND_SIZE);
    int type;

    if(op == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asmObject.c",600);
        return;
    }

    strcpy(op, readWord(obj));
    if(op == NULL || readWord(obj) != NULL || strchr(op,',') != NULL){
        addError("Error!, jmp, jsr and bne instructions require ONLY 1 operand!", obj->lineSize);
        return;
    }

    type = getOperandType(op);
    if(!isValidAdreesingSingleOp(obj->opCode, type)){
        addError("Error! Operand type is Illegal for instruction", obj->lineSize);
        return;
    }

    obj->isValidCode = TRUE;
    obj->ERAMask = ERA_ABSOLUTE;
    obj->opcodeMask = maskOperand(NONE, type);
    obj-> paramMask = NONE;

    obj->instructionData.machine_code = maskInstruction(obj);
    obj->instructionType = Machine_code;
    addToMemory(obj->instructionData, obj->instructionType);

    obj->instructionData.label = malloc(sizeof(char *) * MAX_LABEL_SIZE);
    if(obj->instructionData.label == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asmObject.c",641);
        return;
    }

    obj->newIC++;
    addOperands(NULL, op, -1, type, obj);
}

void AddTwoOperandInstruction(asmObject *obj){
    char *deleimiter;
    char *firstOp;
    int firstOpType;
    char *secondOp;
    int secondOpType;
    int i = 0;

    deleimiter = strchr(obj->line, ',');
    if(deleimiter == NULL){
        addError("Error! instruction requires TWO operands!", obj->lineSize);
        return;
    }

    firstOp = malloc(sizeof(char *) * MAX_LINE_LENGTH);
    if(firstOp == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asmObject.c",663);
        return;
    }
    secondOp = malloc(sizeof(char *) * MAX_LINE_LENGTH);
    if(secondOp == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asmObject.c line",668);
        return;
    }

    while(isspace(obj-> line[0])){
        obj->line++;
    }

    while (obj->line[0] != EOF && obj->line[0] != ','){
        firstOp[++i] = obj->line[0];
        obj->line++;
    }
    firstOp[i] = EOF;

    while(isspace(obj-> line[0])){
        obj->line++;
    }

    if(obj->line[0] != ','){
        addError("Error! Illegal first operand!", obj->lineSize);
        return;
    }

    obj->line++;
    while(isspace(obj-> line[0])){
        obj->line++;
    }

    while (obj->line[0] != EOF && obj->line[0] != ','){
        secondOp[++i] = obj->line[0];
        obj->line++;
    }
    secondOp[i] = EOF;

    if(readWord(obj) != NULL){
        addError("Error! instruction expects ONLY two operands!", obj->lineSize);
        return;
    }

    if(firstOp == NULL || secondOp == NULL){
        addError("Error! instruction requires TWO operands!", obj->lineSize);
        return;
    }

    firstOpType = getOperandType(firstOp);
    secondOpType = getOperandType(secondOp);
    if(!isValidAdreesingTwoOp(obj->opCode, firstOpType, secondOpType)){
        addError("Error! Ilegal operand types!", obj->lineSize);
        return;
    }

    obj->isValidCode = TRUE;
    obj-> ERAMask = ERA_ABSOLUTE;
    obj-> opcodeMask = maskOperand(firstOpType, secondOpType);
    obj->paramMask = NONE;

    obj->instructionData.machine_code = maskInstruction(obj);
    obj->instructionType = Machine_code;
    addToMemory(obj-> instructionData, obj->instructionType);

    obj-> instructionData.label = malloc(sizeof(char *) * MAX_LABEL_SIZE);
    if(obj-> instructionData.label == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asmObject.c",732);
        return;
    }
    addOperands(firstOp, secondOp, firstOpType, secondOpType, obj);
    obj->newIC++;
}

void addOpCode(asmObject *obj){
    int opCode = findOpCode(obj->word);
    int operandCount;

    if(opCode == ILLEGAL){
        addError("Error! Illegal Opcode!", obj->lineSize);
        return;
    }

    obj->opCode = opCode;
    obj->opcodeMask = getOpCodeMask(obj->opCode);
    obj->labelType = INSTRUCTION_TYPE;
    operandCount = getNumOfOparands(opCode);

    handleOperandCount(opCode, operandCount, obj);
    
}

void handleOperandCount(int opCode, int operandCount, asmObject *obj){
    switch(operandCount){
        case ZERO_OPERAND: {
            if(readWord(obj) != NULL){
                addError("Error! Opcode expects no operands!", obj->lineSize);
                return;
            }

            obj->isValidCode = TRUE;
            obj->instructionData.machine_code = obj-> opcodeMask;
            obj->instructionType = Machine_code;

            addToMemory(obj->instructionData, obj->instructionType);
            obj->newIC++;
            break;
        }
        case ONE_OPERAND: {
            if(opCode == JMP || opCode == JSR || opCode == BNE){
                addInstructionToMemory(obj);
            } else {
                AddSingleOperandInstruction(obj);
            }
            break;
        }
        case TWO_OPERAND:{
            AddTwoOperandInstruction(obj);
            break;
        }
    }
}


void addNumberToDataMemory(asmObject *obj){
    long number[MAX_LINE_LENGTH];
    int dataSize = 0;
    int i = 0;
    int isNegative = FALSE;

    char *rawToken = NULL;
    char *token;
    errno = 0;

    rawToken = strtok(obj->line, ",");

    while (rawToken != NULL){
        i = 0;
        token = malloc(sizeof(char *) * MAX_LINE_LENGTH);
        if(token == NULL){
            reportToStderr("Crtical Error!! system memory alocation error in asmObject.c line",805);
            return;
        }

        while(isspace(rawToken[0]) != 0){
            rawToken++;
        }

        while (rawToken[0] != EOF){
              if(rawToken[0] == '-' || rawToken[0] == '+') {
                  if(isNegative){
                      addError("Error! .data must receive legal numerical expressions!", obj->lineSize);
                      return;
                  } else {
                    token[++i] = rawToken[0];
                    rawToken++;
                    isNegative = TRUE;
                  }
              } else {
                  isNegative = TRUE;

                  if(isdigit(rawToken[0]) == 0){
                      while(rawToken[0] != EOF){
                          if(isspace(rawToken) == 0){
                              addError("Error! .data must receive legal numerical expressions!", obj->lineSize);
                              return;
                          }
                          rawToken++;
                      }
                  } else {
                      token[++i] = rawToken[0];
                      rawToken++;
                  }
              }      
        
        }

        number[dataSize] = strtol(token, NULL, 10);

        if(number[dataSize] < MIN_NUM_SIZE || number > MAX_NUM_SIZE || errno == ERANGE ){
            addError("Error! .data accepts numbers between 8191 and -8191", obj->lineSize);
            return;
        }

        dataSize++;
        rawToken = strtok(NULL, ',');
        isNegative = FALSE;
        errno = 0;
    }

    obj->isValidCode = TRUE;
    for(i=0; i < dataSize; i++){
        setDataMemory(getIntegerHexVal(number[i]));
        obj->newDC++;
    }
}

void addStringToDataMemory(asmObject *obj){
    int i = 0;
    char *buffer = malloc(sizeof(char *) * strlen(obj->line));;

    char *stringStart;
    char *stringEnd;
    char *stringEndCpy; /* a copy of lastValid */

    if(buffer == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asmObject.c",864);
        return;
    }

    while(isspace(obj->line[0]) != 0){
        obj->line++;
    }

    if(obj->line[0] == '"'){
        addError("Error! .string parameter must be encapsulated by \"!", obj->lineSize);
        return;
    }

    stringStart = strchr(obj->line, '"');
    stringEnd = strrchr(obj->line, '"');
    stringEndCpy = stringEnd;
    if(stringStart == stringEnd){
        addError("Error! .string parameter must be encapsulated by \"!", obj->lineSize);
        return;
    }

    stringEndCpy++;
    while(stringEndCpy != EOF){
        if(isspace(stringEndCpy[0]) == 0){
            addError("Error! .string parameter must be encapsulated by \"!", obj->lineSize);
            return;
        }
        stringEndCpy++;
    }

    while (stringStart != stringEnd + 1){
        stringStart++;
        buffer[++i] = stringStart[0];
    }

    obj->isValidCode = TRUE;
    while(buffer[0] != EOF){
        setDataMemory(buffer[0]);
        buffer++;
        obj->newDC++;
    }
    setDataMemory(0); /* designate end of input */
    obj->newDC++;
}

void addLabelToDataMemory(asmObject *obj){
    char *label;
    char *delimiter;
    
    int wordSize;
    symbolType symbol;

    delimiter = strchr(obj->line, ':');

    if(delimiter == NULL){
        readWord(obj);
    } else {
        label = malloc(sizeof(char) * MAX_LABEL_SIZE);
        if(label == NULL){
            reportToStderr("Crtical Error!! system memory alocation error in asmObject.c",938);
            return;
        }
        sscanf(strtok(obj->line, ":"), "%s", label);
        obj->line = ++ delimiter;
        wordSize = strlen(label);
    }

    /* if label exist check it*/
    if(label != NULL && !memoryDataCheckLabel(label, wordSize, obj)){
       return;
    }

    /*check memory instruction*/
    if(obj->word[0] == '.' && !memoryDataCheckInstruction(obj)){
        return;
    }

    /*check opcode*/
    memoryDataCheckOpCode(obj);

    obj->currentIC = obj->newIC;
    obj->currentDC = obj->newDC;
}

int memoryDataCheckLabel(char* label, int wordSize,asmObject *obj){
    label[wordSize] = EOF;
    if(!isValidLabel(label)){
        //"Error! Illegal Label!"
        addError("Error! Illegal Label!", obj->lineSize);
        return FALSE;
    }
    obj->isLabel = TRUE;
    strcpy(obj->labelName, label);
    readWord(obj);
    return TRUE;
}

int memoryDataCheckInstruction(asmObject *obj){
    int instruction = findInstruction(obj->word);
    if(instruction != ILLEGAL){
        obj->labelType = DATA_TYPE;
    }
    switch(instruction){
        case EXTERN: {
            if(obj->isLabel){
                //todo: add warning handle
                printf("Warning: Label is meaningless before \".extern\" and \".entry\" instructions.\n");
            }
            addExternToSymbolTable(obj);
            return TRUE;
        }
        case ENTRY:{
            if(obj->isLabel){
                //todo: add warning handle
                printf("Warning: Label is meaningless before \".extern\" and \".entry\" instructions.\n");
            }
            addEntry(obj);
            return TRUE;
        }
        case DATA:{
            addNumberToDataMemory(obj);
            return TRUE;
        }
        case STRING:{
            addStringToDataMemory(obj);
            return TRUE;
        }
        default: {
            addError("Error! Ilegal Memory instruction!", obj->lineSize);
            return FALSE;
        }
    }
}

void addFileToMemory(asmObject *obj){
    while (setLine(obj) != NULL){
        if(obj->line == NULL){
            continue;
        }

        while(isspace(obj->line[0]) != 0){
            obj->line++;
        }

        if(obj->line[0] != ';' && obj->line[0] != EOF){
            addLabelToDataMemory(obj);
        }
    }
    //updateDataSymbols(pr->newIC);
    //updateEntries(pr->entArray, pr->entry_count); 
}

int memoryDataCheckOpCode(asmObject *obj){
    obj->labelType = INSTRUCTION_TYPE;
    addOpCode(obj);
}

void addExternToSymbolTable(asmObject *obj){
    char *token = strtok(obj->line, ',');;
    symbolType symbol = EXTERN;

    while (token != NULL){
        sscanf(token, "%s", obj->word);

        if(!isValidLabel(obj->word)){
            addError("Error! Illegal Label!!", obj->lineSize);
            return;
        }

        //addToSymbolTable(pr->word->string, -1, 0, s_type);
        token = strtok(NULL, ",");
    }
    
}

void addEntry(asmObject *obj){
    char *token =  strtok(obj->line, ',');
    while (token != NULL){
        sscanf(token, "%s", obj->word);
        if(!isValidLabel(obj->word)){
            addError("Error! Illegal Label!!", obj->lineSize);
            return;
        }

        obj->labels[obj->labelsSize] = malloc(sizeof(char *) * MAX_LABEL_SIZE);
        if(obj->labels[obj->labelsSize] == NULL){
            reportToStderr("Crtical Error!! system memory alocation error in asmObject.c",1054);
            return;
        }
        strcpy(obj->labels[obj->labelsSize], obj->word);
        obj->labelsSize++;
        token = strtok(NULL, ",");
    }
}

int isValidLabel(char *label){
    int i;
    int labelSize = strlen(label);

    if(label[labelSize -1] == ':'){
        label[labelSize-1] = EOF;
        labelSize--;
    }

    if(labelSize == 0 || isalpha(label[0]) == 0){
        return FALSE;
    }

    for(i =0; i <labelSize; i++){
        if(!isalpha(label[i]) && !isdigit(label[i])){
            return FALSE;
        }
    }

    if(findRegister(label) != ILLEGAL || findInstruction(label) != ILLEGAL || isSavedWord(label)){
        return FALSE;
    }

    return TRUE;
}