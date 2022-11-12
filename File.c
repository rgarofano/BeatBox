#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 1024

FILE* File_getFilePointer(const char* FILE_PATH, char* MODE, const char* FILE_NAME)
{
    FILE* pFile = fopen(FILE_PATH, MODE);
    if (pFile == NULL) {
        printf("ERROR: Unable to open %s file.\n", FILE_NAME);
        exit(1);
    }
    return pFile;
}

char* File_extractFileContents(FILE* pFile)
{
    char buffer[MAX_SIZE];
    fgets(buffer, MAX_SIZE, pFile);
    char* readData = malloc((strlen(buffer) + 1) * sizeof(*readData));
    strncpy(readData, buffer, strlen(buffer));
    return readData;
}