#include<stdlib.h>
#include<string.h>
#include "errorHandler.h"
#include "util.h"

struct error {
    char *msg;
    int asmLineNumber;
    Error *nextError;
};

static Error *Errors;

void reportToStderr(char *msg, int lineNumber){
    printf(stderr, "%s in line %d", msg, lineNumber);
}

void reportToStdout(char *msg, int lineNumber){
    printf("Error: in line %d: %s\n", lineNumber, msg);
}

void reportWarning(char *msg){
    printf("%s",msg);
}


void addError(int line, int *msg){
    Error *error;
    
    if(error == NULL){
        error = malloc(sizeof(Error *));
        if(error == NULL){
            reportToStderr("Crtical Error!! system memory alocation error in errorHaldler.c",22);
            return;
        }
        error->msg = malloc(sizeof(char *) * MAX_ERR_LENGTH);
        if(error->msg == NULL){
            reportToStderr("Crtical Error!! system memory alocation error in errorHaldler.c",26);
            return;
        }
        Errors->asmLineNumber = line;
        strcpy(Errors->msg, msg);
        Errors->nextError = NULL;
    } else {
        error = Errors;
        for(error = Errors; error->nextError != NULL; error = error->nextError);

        error->nextError = malloc(sizeof(Error *));
        if(error->msg == NULL){
            reportToStderr("Crtical Error!! system memory alocation error in errorHaldler.c",37);
            return;
        }
        error->nextError->msg = malloc(sizeof(char *) * MAX_ERR_LENGTH);
        if(error->msg == NULL){
            reportToStderr("Crtical Error!! system memory alocation error in errorHaldler.c",42);
            return;
        }

        error->nextError->asmLineNumber = line;
        strcpy(error->nextError->msg, msg);
        error->nextError->nextError = NULL;
    }
}

void printErrors(){
    Error *error = Errors;
    char *buffer = malloc(sizeof(char *) * MAX_ERR_LENGTH);
    for(error = Errors; error != NULL; error = error->nextError){
        reportToStdout(error->msg, error->asmLineNumber);
    }
}

int isErrors(){
    return Errors == NULL ? FALSE : TRUE;
}

void freeErrors(){
    Error *error = Errors;
    while (error != NULL){
        Errors = Errors->nextError;
        free(error->msg);
        free(error);
        error = Errors; 
    }
}
