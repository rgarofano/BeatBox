/* Used to open files for reading or writing and retireve the contents from a file */

#ifndef FILE_H
#define FILE_H

#include <stdio.h>

FILE* File_getFilePointer(const char* FILE_PATH, char* MODE, const char* FILE_NAME);
char* File_extractFileContents(FILE* pFile);

#endif