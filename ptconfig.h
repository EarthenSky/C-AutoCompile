#ifndef PTCONFIG
#define PTCONFIG
    //#include <windows.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    typedef struct ItemContents {
        char contentIdentifier;  // 's' == string, 'n' == number, 'a' == array.
        int num;
        char* itemString;
        char** rowArray;  // This is an array containing the values of each item (as raw strings [includes the type identifier].)
        //char stringArray[arrayLength][maxStringSize+1];
    } ItemContents_t;
    //ItemContents_Default = {false, -1, '\000'};

    const char g_configFileName[] = ".ptconfig";  // The filename of the program's data file.

    int getFileString(const char*, char*&);  // Read file and output string ptr.

    ItemContents_t ReadItem(char*, char*, int, int);
    void DealocateItemContents(ItemContents_t);

    int FormatConfig(char*);
    char* ConfigAddRow(char*, char*, char*);

#endif  //PTCONFIG
