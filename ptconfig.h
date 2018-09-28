#ifndef PTCONFIG
#define PTCONFIG
    //#include <windows.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    /*
    // This is for linked lists.
    typedef struct node {
        char* nodeString;
        struct node * next;
    } node_t;
    */

    typedef struct ItemContents {
        bool isNumber;
        int num;
        char itemString[];
    } ItemContents_t;

    const char g_configFileName[] = ".ptconfig";  // The filename of the program's data file.

    int getFileString(const char*, char*&);  // Read file and output string ptr.

    //ItemContents ReadItem(char*, int, int);

    int FormatConfig(char*);
    char* ConfigAddRow(char*, char*, char*);

#endif  //PTCONFIG
