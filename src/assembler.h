#ifndef ASEMBLER_H
#define ASEMBLER_H

#include "util.h"
#include "asmObject.h"
#include "memory.h"
#include "symbolTable.h"
#include "errorHandler.h"

void parseAsmFile(int argc, char *argv[]);

int runAsmTask(char *argv[], int file_counter);

int checkFileNameElocation(char *name, int codeLine);

int readAsmFile(asmObject *asmObj, char* fileName);

int handleObjFile(asmObject *asmObj, char *file_name_obj);

int handleExternFile(asmObject *asmObj, char *file_name_ext, char *file_name_og);

int handleEntryFile(asmObject *asmObj,  char *file_name_ent);

#endif