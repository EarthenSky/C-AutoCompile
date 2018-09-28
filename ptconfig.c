#include "ptconfig.h"
/// Instructions:
/// Titles cannot contain '\001', '\002', '\003', '\004', '.', or '; in .ptconfig files.
/// Row items cannot contain '\001', '\002', '\003', or '\004' in .ptconfig files.

// This is a string function to cutr from front.
#include <assert.h>
void chopN(char *str, size_t n) {
    assert(n != 0 && str != 0);
    size_t len = strlen(str);
    if (n > len)
        return;  // Or: n = len;
    memmove(str, str+n, len - n + 1);
}

// This function reads the entire contents of the file String
// and returns ptr to string.
int getFileString(const char* fileName, char* &fileStringOut) {
    FILE *fptr = fopen(fileName, "r");  // open file for reading.

    if (fptr == NULL) { return 1; }

    // Seek to end of file and find length of file.
    int fileStringSize = 0;
    fseek(fptr, 0L, SEEK_END);
    fileStringSize = ftell(fptr)+1;  // the +1 is for the \0 at EOS.
    rewind(fptr);  // Go back to beginning.

    // Fill string with file contents.
    char fileContents[fileStringSize];
    fgets(fileContents, fileStringSize, fptr);

    fclose(fptr);  // Close file.

    printf("In - File Contents: %s\n", fileContents);

    fileStringOut = (char*)malloc(fileStringSize*sizeof(char));  // Allocate memory for the thing.
    if (fileStringOut == NULL) { return 1; }

    strcpy(fileStringOut, fileContents);
    return 0;  // All good!
}

char *multi_tok(char *input, const char *delimiter) {
    static char *string;
    if (input != NULL)
        string = input;

    if (string == NULL)
        return string;

    char *end = strstr(string, delimiter);
    if (end == NULL) {
        char *temp = string;
        string = NULL;
        return temp;
    }

    char *temp = string;

    *end = '\0';
    string = end + strlen(delimiter);
    return temp;
}
/// When accessing sub-titles, include the dot operator in the string.
/// ex. nameArray = "title.subTitle"
//ItemContents_t ReadItem(char* nameString, int rowIndex, int itemIndex) {
    //ItemContents_t results = { .isNumber = NULL, .num = NULL, .itemString = NULL };
    //return results;
//}

// -------------------------------------------------------------------------- //

/// Returns 1 if failed, 0 if pass.
/// This function creates a new file with filestring's contents and overwrites
/// the old file if there is no error.
int WriteConfig(char* fileString) {
    return 0;
}

/// Returns 1 if failed, 0 if pass.
/// This function formats the config file and adds all needed titles and sub-titles.
/// When inputting a name, all subtitles should be separated by a dot, with a semicolon separating new titles.
/// ex. formatString -> "title.subTitle1.subTitle2;newTitle.subTitle1;coolNumbers;words;importantInfo"
int FormatConfig(char* formatString) {
    printf("Start Formatting Config\n");

    int semicolonCount = 0;  // How many semicolons.
    int maxStringSize = 0;  // Largest sub-string.

    int leftLength = 0;
    int rightLength = 0;

    char* tmpStr = formatString;
    while(tmpStr = strstr(tmpStr, ";")) {  // Find how many sub strings there will be.
        semicolonCount++; tmpStr++;

        //printf( "\t\tproper num : %i\n",  (strlen(formatString) - strlen(tmpStr)) - leftLength - 1 );

        int currentStringSize = (strlen(formatString) - strlen(tmpStr)) - leftLength - 1;
        if( currentStringSize > maxStringSize ) {
            maxStringSize = currentStringSize;
        }

        leftLength = strlen(formatString) - strlen(tmpStr);
        rightLength = strlen(tmpStr);
    }

    // Remember to check the last string.
    if( rightLength > maxStringSize ) {
        maxStringSize = rightLength;
    }

    // DEBUG: this.
    //printf("\tsemicolon count: %i\n", semicolonCount);
    //printf("\tmax string size: %i\n", maxStringSize);

    // Init the array to hold all the strings. ( maxStringSize+1 to hold the '\0'? )
    int arrayLength = semicolonCount+1;
    char stringArray[arrayLength][maxStringSize+1];

    // Loop through all string elements and them to the array.
    int index = 0;
    char* ptrStr;
    ptrStr = strtok(formatString, ";");
    while (ptrStr != NULL) {
        //printf("\t;-Writing: %s\n", ptrStr);
        strcpy(stringArray[index], ptrStr);  // assign string value.
        ptrStr = strtok (NULL, ";");  // Remove item from the pointer... somehow.
        index++;
    }

    // Open file to write to.
    FILE *fptr;
    fptr = fopen(g_configFileName, "w");

    for(int i=0; i < arrayLength; i++) {
        // Loop through all string elements.
        ptrStr = strtok(stringArray[i], ".");
        while (ptrStr != NULL) {
            //printf("\t.-Writing%i: %s\n", i, ptrStr);
            fprintf( fptr, "\001%s", ptrStr );
            ptrStr = strtok (NULL, ".");  // Remove item from the pointer... somehow.
        }
        //printf("\t\texit%i\n", i);
        fprintf( fptr, "\001" );
    }

    fclose(fptr);  // Close file.

    // ---------------------------------------
    /*
    // Make a copy of the in string to modify.
    char formatStringCopy[strlen(formatString)];
    strcpy (formatStringCopy, formatString);

    char* semiColonPtr;
    char* dotPtr;
    semiColonPtr = strtok(formatString, ";");

    //loop through all title groups
    while (semiColonPtr != NULL) {
        printf("Writing: %s\n", semiColonPtr);

        // Copy the string from semiColonPtr to titleGroup.
        //char* titleGroup = semiColonPtr;
        char titleGroup[128];
        strcpy (titleGroup, semiColonPtr);

        char* ptrTitleGroup = titleGroup;

        //loop through all title and subtitle elements.
        dotPtr = strtok(titleGroup, ".");
        while (dotPtr != NULL) {
            printf("\tWriting: %s\n", dotPtr);
            dotPtr = strtok (NULL, ".");  // Remove item from the pointer... somehow.
        }
        printf("\tWriting: <-");

        ptrTitleGroup = strtok (NULL, ";");  // Remove item from the pointer... somehow.
        semiColonPtr = ptrTitleGroup;

        //TODO: this.
        char subbuff[5];
        memcpy( subbuff, &buff[10], 4 );
        subbuff[4] = '\0';
    }

        //while ( strlen(titleSection) > 0 ) {
        //int nextTitle = ptr - formatString;
        //}

    //}
    */
    return 0;  // This means good.
}

/// Returns 1 if failed, 0 if pass, -1 if title not found.
/// fileString is the string to modify.  Title is the header to add a row to.  ("title.subtitle") can only ever be 1 '.'
/// rowInitString is the string that is written in the row (remember to include \003 and \004 for item separation).
int ConfigAddRow(char* fileString, char* keyString, char* rowInitString) {
    printf("\t#### In - %s : %s\n", keyString, rowInitString);

    char tmpFileString[strlen(fileString)+1];  // Create filestring copy.

    char* title = NULL;  // This is the key.
    char* subtitle = NULL;  // This is also the key. \000 is null?

    char* ptrStr;
    ptrStr = strtok(keyString, ".");

    int index = 0;
    while (ptrStr != NULL) {
        if(index == 0) {
            title = (char*)malloc((strlen(ptrStr)+1)*sizeof(char));
            strcpy(title, ptrStr);  // assign string value.
        } else if (index == 1) {
            subtitle = (char*)malloc((strlen(ptrStr)+1)*sizeof(char));
            strcpy(subtitle, ptrStr);  // assign string value.
        } else {
            printf("!!! - TitleError\n");
            return 1;  // error with title.
        }
        ptrStr = strtok (NULL, ".");  // Go to next item.
        index++;
    }
    printf("\t\ttitle: %s subtitle: %s \n", title, subtitle);

    // Look for title string.
    const char* delim1 = "\001\002";
    const char* delim2 = "\001";
    while (true) {
        strcpy(tmpFileString, fileString);
        ptrStr = strtok(tmpFileString, delim1);
        //ptrStr = strtok (NULL, delim1);  // Remove leading \001.
        printf("\tReading: |%s| v.s |%s|\n", ptrStr, title);
        //printf("\t output: %i", strcmp(ptrStr, title));
        // Read title name.
        //ptrStr = strtok (NULL, delim1);  // Go to next item.

        if (strcmp(ptrStr, title) == 0) {  // Break loop if title name is correct.
            printf("Correct title\n");
            chopN(fileString, 1);
            goto DoneLoop;  // exit while loop.
        }

        //printf("\t\tfileString is: %s\n", fileString);
        // Remove first char from file string.
        chopN(fileString, 1);

        printf("\t\tfileString is: %s\n", fileString);

        strcpy(tmpFileString, fileString);
        //if (fileString == "") {  //Check if this works.
        //    return -1;  // Could not find title.
        //}

        // Search for double \001 and count amount to cutoff when found.
        int frontCutoffLength = 0;  // Length to cutoff.
        ptrStr = multi_tok(tmpFileString, "\001\001");
        while(strlen(ptrStr) != 0) {
            //printf("\t\t\tLengthAdd: %i, LengthTotal: %i, ptrString: %s\n", strlen(ptrStr) + 1, frontCutoffLength, ptrStr);
            //frontCutoffLength += strlen(ptrStr) + 1;

            printf("\tThe day: %s\n", ptrStr);
            ptrStr = multi_tok(NULL, "\001\001");


            if (ptrStr == NULL) {
                return -1;
            }
        }

        // Remove first char from file string.
        chopN(fileString, frontCutoffLength);
        printf("\t\tLast: fileString is: %s\n", fileString);
    }

    DoneLoop:
    printf("Title is: %s\n", title);

    // Look for subtitle.
    //if (subtitle != NULL) {

    //}

    //TODO: modify fileString.

    // dealocate pointers.
    free(title);
    free(subtitle);
    return 0;
}
