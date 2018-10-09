#include "dialog.h"

// This is for the about dialog box.
// (nothing in the default case [no defwindowproc], is done automagically.)
BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch(Message)
    {
        case WM_INITDIALOG:
            return TRUE;  // True means message was processed.  (This also sets focus to the new window.)
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDOK:
                    EndDialog(hwnd, IDOK);  // This destroys the current window.
                break;
                //case IDCANCEL:
                //    EndDialog(hwnd, IDCANCEL);  // This destroys the current window.
                //break;
            }
        break;
        case WM_CLOSE:
            EndDialog(hwnd, IDCANCEL);  // Exiting the window has the same affect as pressing cancel.
            return TRUE;
        default:
            return FALSE;  // False means message not processed.
    }
    return TRUE;
}

bool VerifyInput(char* title, char* path);
void ProcessInput(HWND hwnd, char* title, char* path, char* desc, char* appendCommand);
void PushError(void);

static HWND hDialog;

// This is the callback loop for the create project window.
BOOL CALLBACK CreateProjectDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch(Message)
    {
        case WM_INITDIALOG:
            hDialog = hwnd;
            return TRUE;  // True means message was processed.  (This also sets focus to the new window.)
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDOK: {
                    // Get length and value of all edit contol values, and save them. (To later verify and write to config file.)
                    HWND hTitle = GetDlgItem(hwnd, DB_ID_EDIT_NAME);
                    int titleLength = SendMessage(hTitle, EM_LINELENGTH, 0, 0)+1;
                    TCHAR titleStr[titleLength];

                    HWND hPath = GetDlgItem(hwnd, DB_ID_EDIT_PATH);
                    int pathLength = SendMessage(hPath, EM_LINELENGTH, 0, 0)+1;
                    TCHAR pathStr[pathLength];

                    HWND hDesc = GetDlgItem(hwnd, DB_ID_EDIT_COMMENT);
                    int descLength = SendMessage(hDesc, EM_LINELENGTH, 0, 0)+1;
                    TCHAR descStr[descLength];

                    HWND hAppend = GetDlgItem(hwnd, DB_ID_EDIT_APPEND);
                    int appendLength = SendMessage(hAppend, EM_LINELENGTH, 0, 0)+1;
                    TCHAR appendStr[appendLength];

                    GetDlgItemText(hwnd, DB_ID_EDIT_NAME, titleStr, titleLength);
                    GetDlgItemText(hwnd, DB_ID_EDIT_PATH, pathStr, pathLength);
                    GetDlgItemText(hwnd, DB_ID_EDIT_COMMENT, descStr, descLength);
                    GetDlgItemText(hwnd, DB_ID_EDIT_APPEND, appendStr, appendLength);

                    // Make sure that user input is acceptable.
                    if( VerifyInput(titleStr, pathStr) == TRUE ) {
                        ProcessInput(hwnd, titleStr, pathStr, descStr, appendStr);  // Write the project information to the data file. (In plaintext.)
                        EndDialog(hwnd, IDOK);  // This destroys the current window.
                    } else {
                        PushError();  // This stops the user from exiting using the 'OK' button.
                    }
                }
                break;
                case IDCANCEL:
                    EndDialog(hwnd, IDCANCEL);  // This destroys the current window.
                break;
            }
        break;
        case WM_CLOSE: {
            EndDialog(hwnd, IDCANCEL);  // Exiting the window has the same affect as pressing cancel.
            return TRUE;
        }
        case WM_KEYDOWN: {
            if(wParam == VK_RETURN) {
                return TRUE;  // Stop enter from exiting the control.
            }
        }
        default:
            return FALSE;  // False means message not processed.
    }
    return TRUE;
}

bool VerifyInput(char* title, char* path) {
    //printf( "title: %s\n", title=="c\0" );
    //printf( "path: %s\n", path=="c\0" );
    //printf( "NULL: %s\n", NULL );
    //printf("\ttitle: %s, path: %s\n", title, path);

    bool isEmpty = FALSE;
    if(strcmp(title, "") == 0 || strcmp(path, "") == 0) {
        printf("ERR: Project title and path must to be filled in.\n");
        isEmpty = TRUE;
    }

    bool isValidPath = TRUE;  // check if path is valid.

    bool containsValidChars = TRUE;  // make sure that no invalid characters exist.

    return !isEmpty && isValidPath && containsValidChars;  //TODO: do all these checks.
}

// This file writes to the program data file.
void ProcessInput(HWND hwnd, char* title, char* path, char* desc, char* appendCommand) {
    //printf("TODO: input code here to process the input.\n");
    // Retrieve file as string.
    char* fileString = NULL;
    if ( getFileString(g_configFileName, fileString) == 1 ) {
        printf("ERROR with file Initialization.\n");
    } else {
        // Add the new project row.
        char key[] = "projects";
        int valueLength = strlen("\002\003\003\003\003") + strlen(title) + strlen(path) + strlen(desc) + strlen(appendCommand)+1;
        char value[valueLength];  // Data is stored as (name, path, desc, append to command)
        sprintf(value, "\002\003%s\003%s\003%s\003%s", title, path, desc, appendCommand);
        fileString = ConfigAddRow(fileString, key, value);  // Add a row to the config string.

        // Get how many projects there are.
        int projectCount = 0;
        char key2[] = "projectCount";
        ItemContents_t results = ReadItem(fileString, key2, 0, 0);
        projectCount = atoi(results.itemString);  // Get the integer value.
        DealocateItemContents(results);  // Dealocate the read item.

        // MAKE projectCount GO UP 1 EACH TIME A NEW ROW IS ADDED.
        int value2Length = strlen("\002\003") + N_DIGITS(projectCount+1)+1;
        char value2[value2Length];
        sprintf(value2, "\002\003%i", projectCount+1);
        fileString = WriteRow(fileString, key2, 0, value2);  // Update projectCount
        //printf("fileString: %s\n", fileString);

        // Write filestring to file.
        FILE *fptr = fopen(g_configFileName, "w");
        fprintf(fptr, fileString);
        fclose(fptr);

        // Add the new project to the project selection list box.
        HWND hListBoxProject = GetDlgItem(GetParent(hwnd), ID_LB_PROJECT);
        SendMessage(hListBoxProject, LB_ADDSTRING, 0, (LPARAM)title);
    }

    free(fileString);  // Dealocate fileString.
}

void PushError(void) {
    printf("Input invalid.\n");
}
