#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "assembler.h"

static const char *FILE_EXT = ".as";
static const char *OBJ_EXT = ".ob";
static const char *EXT_EXT = ".ext";
static const char *ENT_EXT = ".ent";

void parseAsmFile(int argc, char *argv[]){
    int status;
	FILE *obj;
	FILE *ext;
	FILE *ent;
	int file_counter;
	/*file names:*/
	char *file_name_og;
	char *file_name_asem;
	char *file_name_obj;
	char *file_name_ent;
	char *file_name_ext;

    printf("Number of tasks: %d\n", argc-1);

    for(file_counter=1; file_counter<argc; file_counter++) {
        status = runAsmTask(argv, file_counter);
        if(status){
            printf("read for file number %d succseful! \n");
        }
    }
}

int runAsmTask(char *argv[], int file_counter){
    asmObject *asmObj;
    FILE *asem;
    char *file_name_og = NULL;
	char *file_name_asem  = NULL;
	char *file_name_obj  = NULL;
	char *file_name_ent  = NULL;
	char *file_name_ext  = NULL;

    int status;

    file_name_og = malloc(sizeof(char *) * MAX_FILE_NAME);
    if(!checkFileNameElocation(file_name_og, 43)){
        return FALSE;
    }
	file_name_asem = malloc(sizeof(char *) * MAX_FILE_NAME);
    if(!checkFileNameElocation(file_name_og, 47)){
        return FALSE;
    }
	file_name_obj = malloc(sizeof(char *) * MAX_FILE_NAME);
    if(!checkFileNameElocation(file_name_og, 51)){
        return FALSE;
    }
	file_name_ent = malloc(sizeof(char *) * MAX_FILE_NAME);
    if(!checkFileNameElocation(file_name_og, 55)){
        return FALSE;
    }
	file_name_ext = malloc(sizeof(char *) * MAX_FILE_NAME);
    if(!checkFileNameElocation(file_name_og, 59)){
        return FALSE;
    }

    strcpy(file_name_og, argv[file_counter]);
    strcpy(file_name_asem, file_name_og);
    strcpy(file_name_obj, file_name_og);
    strcpy(file_name_ent, file_name_og);
    strcpy(file_name_ext, file_name_og);

    file_name_asem = strcat(file_name_asem, FILE_EXT);
	printf("File Name: %s\n", file_name_asem);

    asem = fopen(file_name_asem, "r");
    if(asem != NULL){
        asmObj = construct(asem);
        status = readAsmFile(asmObj, file_name_asem);
        if(!status){
            return FALSE;
        }

        status = handleObjFile(asmObj, file_name_obj);
        if(!status){
            return FALSE;
        }

        status = handleExternFile(asmObj, file_name_ext, file_name_og);
        if(!status){
            return FALSE;
        }

        status = handleEntryFile(asmObj, file_name_ent);
        if(!status){
            return FALSE;
        }

        kill(asmObj);
        free(file_name_og);
		free(file_name_obj);
		free(file_name_ext);
		free(file_name_ent);

        return TRUE;
    }
    printf("%s doesn't exists! Proceeding to next file...\n", file_name_asem);
    return FALSE;
}

int checkFileNameElocation(char *name, int codeLine){
    if(name == NULL){
        reportToStderr("Crtical Error!! system memory alocation error in asembler.c",codeLine);
        return FALSE;
    }
    return TRUE;
}

int readAsmFile(asmObject *asmObj, char* fileName){
    printf("%s Exists. Begin parsing:\n", fileName);

    void addFileToMemory(asmObject *asmObj);
    if(isErrors()){
        printErrors();
        return FALSE;
    }

    return TRUE;
}

int handleObjFile(asmObject *asmObj, char *file_name_obj){
    FILE *obj;

    obj = fopen(strcat(file_name_obj, OBJ_EXT), "w");
    if(obj == NULL) {
        printf("Error! Couldn't open .obj file!\n");
        return FALSE;
    }else {
        printf("Creating .ob file!\n");
        printAllMemory(obj);
        fclose(obj);
    }

    return TRUE;
}

int handleExternFile(asmObject *asmObj, char *file_name_ext, char *file_name_og){
    FILE *ext;
    if(hasExternValue()) {
        printf("File Name_og: %s\n", file_name_og);
        ext = fopen(strcat(file_name_ext, EXT_EXT), "w");
        if(ext == NULL){
            printf("Error! Failed to open .ext file!\n");
            return FALSE;
        }

        printf("External symbols file created!\n");
        printExternlValues(ext);
        fclose(ext);
    }
    return TRUE;
}

int handleEntryFile(asmObject *asmObj,  char *file_name_ent){
    FILE *ent;
    if(hasEntryValue()){
        ent = fopen(strcat(file_name_ent, ENT_EXT), "w");
        if(ent == NULL) {
            printf("Error! Couldn't open .ent file!\n");
            return FALSE;
        }

        printf("Entry file created!\n");
        printTableEntries(ent);
        fclose(ent);
    }
    return TRUE;
}