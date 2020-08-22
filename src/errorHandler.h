#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "asmObject.h"

#define MAX_ERR_LENGTH 100

typedef struct error Error;

/* print warning to stdout*/
void reportWarning(char *msg);

/* Print to defind stderr*/
void reportToStderr(char *msg, int lineNumber);

/* Print to defined stdout*/
void reportToStdout(char *msg, int lineNumber);

/*add an error to the error list*/
void addError(int line, int *msg);

/*prints the errors found to stdout*/
void printErrors();

/*checks wheter there are errors*/
int  isErrors();

/*frees the error list*/
void freeErrors();

#endif