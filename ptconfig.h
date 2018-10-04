#ifndef PTCONFIG
#define PTCONFIG
    //#include <windows.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    typedef struct ItemContents {
        char contentIdentifier;  // 's' == string, 'a' == array.
        char* itemString;
        char** rowArray;  // This is an array containing the values of each item.
        //char stringArray[arrayLength][maxStringSize+1];
    } ItemContents_t;

    const char g_configFileName[] = ".ptconfig";  // The filename of the program's data file.

    int getFileString(const char*, char*&);  // Read file and output string ptr.

    ItemContents_t ReadItem(char*, char*, int, int);
    void DealocateItemContents(ItemContents_t);

    char* WriteRow(char* fileStringIn, char* keyString, int rowIndex, char* rowString);  // Write to file.

    int FormatConfig(char*);
    char* ConfigAddRow(char*, char*, char*);

#endif  //PTCONFIG
