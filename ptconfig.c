#include "ptconfig.h"
/// Instructions:
/// Titles cannot contain '\001', '\002', '\003', '\004', '.', or '; in .ptconfig files.
/// Row items cannot contain '\001', '\002', '\003', or '\004' in .ptconfig files.

// WARNING: Almost no error check is in this code.
// EDIT: I'm no longer doing the \004 and \003 thing, they are both just \003 now, types can do what they want.
// NOTE: To any future programmers, don't copy this, redo the entire thing. 100%.

// This is a string function to cutr from front.
#include <assert.h>
void chopN(char *str, size_t n) {
    assert(n != 0 && str != 0);  // if false, exit program.
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

    fileStringOut = (char*)malloc(fileStringSize*sizeof(char));  // Allocate memory for the thing.
    if (fileStringOut == NULL) { return 1; }

    strcpy(fileStringOut, fileContents);
    return 0;  // All good!
}

// Thanks internet.
char* multi_tok(char* input, const char* delimiter) {
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

// This function inserts to into from and puts them.
// The string must be freed after use. "free(str)"
char* insertString(char* to, char* from, int position) {
    char* outString = (char*)malloc( (strlen(to)+strlen(from)+2)*sizeof(char) );

    strncpy(outString, to, position);
    outString[position] = '\0';
    strcat(outString, from);
    strcat(outString, to + position);

    return outString;
}

// Remove given section from string. Negative len means remove
// everything up to the end.
int str_cut(char *str, int begin, int len) {
    int l = strlen(str);

    if (len < 0) len = l - begin;
    if (begin + len > l) len = l - begin;
    memmove(str + begin, str + begin + len, l - len + 1);

    return len;
}

/// When accessing sub-titles, include the dot operator in the string.
/// ex. keyString = "title.subtitle" or "title".
/// This function returns the value of an array at a specific table.
/// If itemIndex = -1 then ItemContents (return value) == array of entire row (string data (doesn't the string header value.)  Type is lost.)
ItemContents_t ReadItem(char* fileStringIn, char* keyString, int rowIndex, int itemIndex) {
    ItemContents_t results;

    // Create filestring copy, so that fileStringIn isn't modifed.
    char fileString[strlen(fileStringIn)+1];
    strcpy(fileString, fileStringIn);

    char tmpFileString[strlen(fileString)+1];  // Create filestring copy.

    // A copy of keystring, so keystring isn't modified.
    char keyStringCopy[strlen(keyString)+1];
    strcpy(keyStringCopy, keyString);

    char* title = NULL;  // This is the key.
    char* subtitle = NULL;  // This is also the key.

    char* ptrStr;
    ptrStr = strtok(keyStringCopy, ".");

    // Find title and subtitle.
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
            return results;  // error with title.
        }
        ptrStr = strtok (NULL, ".");  // Go to next item.
        index++;
    }

    // Move to start of selected table:
    // Look for the title string.
    while (true) {
        strcpy(tmpFileString, fileString);
        ptrStr = strtok(tmpFileString, "\001\002");

        // Remove first char from file string.
        chopN(fileString, 1); //insertPosition += 1;

        if (strcmp(ptrStr, title) == 0) {  // Case: found correct title name.
            chopN(fileString, strlen(title)); //insertPosition += strlen(title);
            // WARNING: this GOTO statement jumps to just after the while loop.
            goto ExitTitleWhile;  // Exit while loop.
        }

        strcpy(tmpFileString, fileString);

        // Search for "\001\001" and count amount to cutoff from fileString when found.
        ptrStr = multi_tok(tmpFileString, "\001\001");
        if (ptrStr == NULL) { return results; }  // Exit Condition.

        int frontCutoffLength = strlen(ptrStr) + 1;  // Length to cutoff.

        chopN(fileString, frontCutoffLength); //insertPosition += frontCutoffLength;
    }
    ExitTitleWhile:

    // Look for subtitle string.
    if (subtitle != NULL) {
        while (true) {
            strcpy(tmpFileString, fileString);
            ptrStr = strtok(tmpFileString, "\001\002");

            // Remove first char from file string.
            chopN(fileString, 1); //insertPosition += 1;

            if (strcmp(ptrStr, subtitle) == 0) {  // Break loop if title name is correct.
                chopN(fileString, strlen(subtitle)); //insertPosition += strlen(subtitle);
                // WARNING: this GOTO statement jumps to just after the while loop.
                goto ExitSubtitleWhile;  // Exit while loop.
            }

            strcpy(tmpFileString, fileString);

            // Search for "\001" and count amount to cutoff from fileString when found.
            ptrStr = multi_tok(tmpFileString, "\001");
            if (ptrStr == NULL) { return results; }  // Exit Condition.

            int frontCutoffLength = strlen(ptrStr);  // Length to cutoff.

            chopN(fileString, frontCutoffLength); //insertPosition += frontCutoffLength;
        }
    }
    ExitSubtitleWhile:

    // Find entire row contents:
    strcpy(tmpFileString, fileString);
    ptrStr = strtok(tmpFileString, "\002");

    index = 0;
    while (ptrStr != NULL) {
        if (index == rowIndex) {
            //printf("alive\n");printf("dead\n");
            // WARNING: this GOTO statement jumps to just after the while loop.
            goto ExitRowWhile;
        }
        index++;
        ptrStr = strtok (NULL, "\002\001");  // Go to next item.
    }
    ExitRowWhile:

    char rowContents[strlen(ptrStr)+1];
    strcpy(rowContents, ptrStr);
    //printf("rowContents: %s\n", ptrStr);

    // Add needed information to type.
    if(itemIndex == -1) {
        results.contentIdentifier = 'a';

        // Array bounds.
        int numItems = 0;
        int itemSize = 0;

        // Find size of rowContents.
        for (int i=0; i <= strlen(rowContents); i++) {
            if (rowContents[i] == '\003' || rowContents[i] == '\004') {
                numItems++;
            }
        }

        // Calculate the largest string size.
        int maxStringSize = 0;  // Largest sub-string.
        {
            int leftLength = 0;
            int rightLength = 0;
            char* tmpStr = ptrStr;
            while(tmpStr = strstr(tmpStr, "\003")) {  // Find how many sub strings there will be.
                tmpStr++;

                int currentStringSize = (strlen(ptrStr) - strlen(tmpStr)) - leftLength - 1;
                if( currentStringSize > maxStringSize ) {
                    maxStringSize = currentStringSize;
                }

                leftLength = strlen(ptrStr) - strlen(tmpStr);
                rightLength = strlen(tmpStr);
            }

            // Remember to check the last string.
            if( rightLength > maxStringSize ) {
                maxStringSize = rightLength;
            }
        }
        itemSize = maxStringSize;  // The size of the arrayitems.

        // Alocate a multidemensional array and assign values to it.
        results.rowArray = (char**)malloc(numItems*sizeof(char*));
        for(int i=0; i<numItems; i++) {
            results.rowArray[i] = (char*)malloc(itemSize*sizeof(char));
        }

        //printf("\t\tArray Size: %i\n", numItems);
        strcpy(ptrStr, rowContents);  //TODO: remove this line.
        index = 0;

        // Holds the string to modify.
        char rowContentsCpy[strlen(rowContents)+1];
        strcpy(rowContentsCpy, rowContents);
        while(true) {  // Assign values to the array.
            char rowContentsTmp[strlen(rowContents)+1];
            strcpy(rowContentsTmp, rowContentsCpy);

            char* tmpStr = strtok(rowContentsTmp, "\003");
            if (tmpStr == NULL) {
                return results;
            }

            chopN(rowContentsCpy, strlen(tmpStr)+1);

            //printf("\t\ttmpStr: %s, %i\n", tmpStr, index);
            strcpy(results.rowArray[index], tmpStr);

            //printf("\t\tresults.rowArray[index]: %s\n", results.rowArray[index]);
            //if (index > 0) {
                //printf("\t\tresults.rowArray[index] - v2: %s\n", results.rowArray[index-1]);
            //}

            index++;
        }
    }
    else {
        results.contentIdentifier = 's';

        // Create a copy of rowContents so it is not modified.  ... for some reason.
        char rowContentsTmp[strlen(rowContents)+1];
        strcpy(rowContentsTmp, rowContents);

        // Find the needed item in rowContents and find type.
        index = 0;
        char* tmpStr = strtok(rowContentsTmp, "\003");
        if(index == itemIndex) {
            results.itemString = (char*)malloc((strlen(tmpStr)*sizeof(char))+1);
            strcpy(results.itemString, tmpStr);
            return results;
        }
        index++;
        while(tmpStr = strtok(NULL, "\003")) {
            //printf("\tstring: %s\n", tmpStr);
            if(index == itemIndex) {
                results.itemString = (char*)malloc((strlen(tmpStr)*sizeof(char))+1);
                strcpy(results.itemString, tmpStr);
                return results;
            }
            index++;
        }
    }
    printf("Oh no, things are not looking good for you. \nExecution should not reach here, unless there is a weird error.\n");
    return results;  //
}

/// This function removes the selected row and inserts rowString in it's place.
/// rowString is a string that contains the entire row.
char* WriteRow(char* fileStringIn, char* keyString, int rowIndex, char* rowString) {
    // Create filestring copy, so that fileStringIn isn't modifed.
    char fileString[strlen(fileStringIn)+1];
    strcpy(fileString, fileStringIn);

    int insertPosition = 0;  // Where to insert the row init string.
    int removeLength = 0;  // How much string to cut off.

    char cleanFileString[strlen(fileString)+1];  // Create filestring "clean" copy. An unchanged version of the input file string.
    strcpy(cleanFileString, fileString);

    char tmpFileString[strlen(fileString)+1];  // Create filestring copy.

    char keyStringCopy[strlen(keyString)+1];  // A copy of keystring, so keystring isn't modified.
    strcpy(keyStringCopy, keyString);

    char* title = NULL;  // This is the key.
    char* subtitle = NULL;  // This is also the key. \000 is null?

    char* ptrStr;
    ptrStr = strtok(keyStringCopy, ".");

    // Find title and subtitle.
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
            return NULL;  // error with title.
        }
        ptrStr = strtok (NULL, ".");  // Go to next item.
        index++;
    }

    // Look for the title string.
    while (true) {
        strcpy(tmpFileString, fileString);
        ptrStr = strtok(tmpFileString, "\001\002");

        // Remove first char from file string.
        chopN(fileString, 1); insertPosition += 1;

        if (strcmp(ptrStr, title) == 0) {  // Case: found correct title name.
            chopN(fileString, strlen(title)); insertPosition += strlen(title);
            // WARNING: this GOTO statement jumps to just after the while loop.
            goto ExitTitleWhile;  // Exit while loop.
        }

        strcpy(tmpFileString, fileString);

        // Search for "\001\001" and count amount to cutoff from fileString when found.
        ptrStr = multi_tok(tmpFileString, "\001\001");
        if (ptrStr == NULL) { return NULL; }  // Exit Condition.

        int frontCutoffLength = strlen(ptrStr) + 1;  // Length to cutoff.

        chopN(fileString, frontCutoffLength); insertPosition += frontCutoffLength;
    }
    ExitTitleWhile:

    // Look for subtitle.
    if (subtitle != NULL) {
        while (true) {
            strcpy(tmpFileString, fileString);
            ptrStr = strtok(tmpFileString, "\001\002");

            // Remove first char from file string.
            chopN(fileString, 1); insertPosition += 1;

            if (strcmp(ptrStr, subtitle) == 0) {  // Break loop if title name is correct.
                chopN(fileString, strlen(subtitle)); insertPosition += strlen(subtitle);
                // WARNING: this GOTO statement jumps to just after the while loop.
                goto ExitSubtitleWhile;  // Exit while loop.
            }

            strcpy(tmpFileString, fileString);

            // Search for "\001" and count amount to cutoff from fileString when found.
            ptrStr = multi_tok(tmpFileString, "\001");
            if (ptrStr == NULL) { return NULL; }  // Exit Condition.

            int frontCutoffLength = strlen(ptrStr);  // Length to cutoff.

            chopN(fileString, frontCutoffLength); insertPosition += frontCutoffLength;
        }

        //printf("test\n");
    }
    ExitSubtitleWhile:

    //printf("FilsString is %s\n", fileString);
    // Filestring has cutoff all but here -> \002\003rowstuff ...

    // Find entire row contents:
    strcpy(tmpFileString, fileString);
    ptrStr = strtok(tmpFileString, "\002");

    index = 0;
    while (ptrStr != NULL) {
        if (index == rowIndex) {
            // WARNING: this GOTO statement jumps to just after the while loop.
            goto ExitRowWhile;
        }

        insertPosition += strlen(ptrStr)+1;
        index++;
        ptrStr = strtok (NULL, "\002\001");  // Go to next item.
    }
    ExitRowWhile:

    char rowContents[strlen(ptrStr)+1];
    strcpy(rowContents, ptrStr);
    //printf("rowContents!!!: %s\n", ptrStr);

    // Remove row.  Modify the clean string (only neccisary changes for output.)
    str_cut(cleanFileString, insertPosition, strlen(rowContents)-1);

    //printf("\tRemoved : %s\n", cleanFileString);

    // Insert row.
    char* outFileStringPtr;
    outFileStringPtr = insertString(cleanFileString, rowString, insertPosition);

    //printf("\toutFileStringPtr : %s\n", outFileStringPtr);

    // Dealocate pointers.
    free(title);
    free(subtitle);

    return outFileStringPtr;
}

// Use this function to dealocate items that have been read.  (only dealocates the array and string.)
void DealocateItemContents(ItemContents_t itemContents) {
    if (itemContents.contentIdentifier == 'a') {
        int numItems = sizeof(itemContents.rowArray) / sizeof(itemContents.rowArray[0]);
        //int itemSize = sizeof(itemContents.rowArray[0]) - 1;
        //printf("\titemSize: %i\n", itemSize);
        //printf("\tnumItems: %i\n", numItems);
        for(int i=0; i<numItems; i++) {
            free(itemContents.rowArray[i]);
        }
        free(itemContents.rowArray);
    } else if (itemContents.contentIdentifier == 's') {
        // Deallocate string
        free(itemContents.itemString);
    } else {
        printf("God speed, exceution should not get here.\n");
    }
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

    printf("Done Formatting Config File.\n");
    return 0;  // This means good.
}

/// Returns char or NULL if failed.
/// fileString is the string to modify.  Title is the header to add a row to.  ("title.subtitle") can only ever be 1 '.'
/// rowInitString is the string that is written in the row (remember to include \003 and \004 for item separation).
char* ConfigAddRow(char* fileStringIn, char* keyString, char* rowInitString) {
    // Create filestring copy, so that fileStringIn isn't modifed.
    char fileString[strlen(fileStringIn)+1];
    strcpy(fileString, fileStringIn);

    int insertPosition = 0;  // Where to insert the row init string.

    char cleanFileString[strlen(fileString)+1];  // Create filestring "clean" copy. An unchanged version of the input file string.
    strcpy(cleanFileString, fileString);

    char tmpFileString[strlen(fileString)+1];  // Create filestring copy.

    char keyStringCopy[strlen(keyString)+1];  // A copy of keystring, so keystring isn't modified.
    strcpy(keyStringCopy, keyString);

    char* title = NULL;  // This is the key.
    char* subtitle = NULL;  // This is also the key. \000 is null?

    char* ptrStr;
    ptrStr = strtok(keyStringCopy, ".");

    // Find title and subtitle.
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
            return NULL;  // error with title.
        }
        ptrStr = strtok (NULL, ".");  // Go to next item.
        index++;
    }

    // Look for the title string.
    while (true) {
        strcpy(tmpFileString, fileString);
        ptrStr = strtok(tmpFileString, "\001\002");

        // Remove first char from file string.
        chopN(fileString, 1); insertPosition += 1;

        if (strcmp(ptrStr, title) == 0) {  // Case: found correct title name.
            chopN(fileString, strlen(title)); insertPosition += strlen(title);
            // WARNING: this GOTO statement jumps to just after the while loop.
            goto ExitTitleWhile;  // Exit while loop.
        }

        strcpy(tmpFileString, fileString);

        // Search for "\001\001" and count amount to cutoff from fileString when found.
        ptrStr = multi_tok(tmpFileString, "\001\001");
        if (ptrStr == NULL) { return NULL; }  // Exit Condition.

        int frontCutoffLength = strlen(ptrStr) + 1;  // Length to cutoff.

        chopN(fileString, frontCutoffLength); insertPosition += frontCutoffLength;
    }
    ExitTitleWhile:

    // Look for subtitle.
    if (subtitle != NULL) {
        while (true) {
            strcpy(tmpFileString, fileString);
            ptrStr = strtok(tmpFileString, "\001\002");

            // Remove first char from file string.
            chopN(fileString, 1); insertPosition += 1;

            if (strcmp(ptrStr, subtitle) == 0) {  // Break loop if title name is correct.
                chopN(fileString, strlen(subtitle)); insertPosition += strlen(subtitle);
                // WARNING: this GOTO statement jumps to just after the while loop.
                goto ExitSubtitleWhile;  // Exit while loop.
            }

            strcpy(tmpFileString, fileString);

            // Search for "\001" and count amount to cutoff from fileString when found.
            ptrStr = multi_tok(tmpFileString, "\001");
            if (ptrStr == NULL) { return NULL; }  // Exit Condition.

            int frontCutoffLength = strlen(ptrStr);  // Length to cutoff.

            chopN(fileString, frontCutoffLength); insertPosition += frontCutoffLength;
        }

        //printf("test\n");
    }
    ExitSubtitleWhile:

    // Find the \001 after the current title and move the insert position.
    // (to right infront of the closing \001)
    strcpy(tmpFileString, fileString);
    ptrStr = multi_tok(tmpFileString, "\001");
    insertPosition += strlen(ptrStr);

    // Write Message to fileString.
    char* outFileStringPtr;
    outFileStringPtr = insertString(cleanFileString, rowInitString, insertPosition);

    // Dealocate pointers.
    free(title);
    free(subtitle);

    return outFileStringPtr;
}

// 650 lines? (*-_-)
