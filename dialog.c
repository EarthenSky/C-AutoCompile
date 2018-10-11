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

// Call this function before starting the dialogcallback loop to prefill
// dialog with text of active LB item.
static bool fillDialog = false;
void FillDialog(void) {
    fillDialog = true;
}

// This is the callback loop for the create project window.
BOOL CALLBACK CreateProjectDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch(Message)
    {
        case WM_INITDIALOG: {
            if(fillDialog == true) {
                // Get id of current selected listbox item.
                HWND hListBoxProject = GetDlgItem(GetParent(hwnd), ID_LB_PROJECT);  // Get the listbox handle.
                DWORD selectedId = SendMessage(hListBoxProject, LB_GETCURSEL, 0, 0);

                if (selectedId != LB_ERR) {
                    // Retrieve file as string.
                    char* fileString = NULL;
                    if ( getFileString(g_configFileName, fileString) == 1 ) {
                        printf("ERROR with file Initialization.\n");
                    } else {
                        char key[] = "projects";
                        ItemContents_t results = ReadItem(fileString, key, selectedId, -1);  // Read item at selectedId to results.

                        // This is setting text to the controls.
                        SetDlgItemText(hwnd, DB_ID_EDIT_NAME, results.rowArray[0]);
                        SetDlgItemText(hwnd, DB_ID_EDIT_PATH, results.rowArray[1]);
                        SetDlgItemText(hwnd, DB_ID_EDIT_COMMENT, results.rowArray[2]);
                        SetDlgItemText(hwnd, DB_ID_EDIT_APPEND, results.rowArray[3]);

                        //printf("results.rowArray[3]: %s\n", results.rowArray[3]);

                        DealocateItemContents(results);  // Dealocate the read item.
                    }

                    free(fileString);  // Remember to dealocate fileString.
                }
            }

            return TRUE;  // True means message was processed.  (This also sets focus to the new window.)
        }
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
                    int descLength = 1;
                    int lineCount = SendMessage(hDesc, EM_GETLINECOUNT, 0, 0);
                    for (int i=0; i<lineCount; i++) {
                        descLength += SendMessage(hDesc, EM_LINELENGTH, i, 0);
                    }
                    TCHAR descStr[descLength];

                    HWND hAppend = GetDlgItem(hwnd, DB_ID_EDIT_APPEND);
                    int appendLength = SendMessage(hAppend, EM_LINELENGTH, 0, 0)+1;
                    TCHAR appendStr[appendLength];

                    GetDlgItemText(hwnd, DB_ID_EDIT_NAME, titleStr, titleLength);
                    GetDlgItemText(hwnd, DB_ID_EDIT_PATH, pathStr, pathLength);
                    GetDlgItemText(hwnd, DB_ID_EDIT_COMMENT, descStr, descLength);
                    GetDlgItemText(hwnd, DB_ID_EDIT_APPEND, appendStr, appendLength);

                    if (descLength == 1) { strcpy(descStr, " "); }  // case: nothing is in comments, fill with space.
                    if (appendLength == 1) { strcpy(appendStr, " "); }  // case: nothing is in comments, fill with space.
                    
                    // Make sure that user input is acceptable.
                    if( VerifyInput(titleStr, pathStr) == TRUE ) {
                        ProcessInput(hwnd, titleStr, pathStr, descStr, appendStr);  // Write the project information to the data file. (In plaintext.)
                        EndDialog(hwnd, IDOK);  // This destroys the current window.
                    } else {
                        PushError();  // This stops the user from exiting using the 'OK' button.
                    }
                }
                break;
                case IDCANCEL: {
                    EndDialog(hwnd, IDCANCEL);  // This destroys the current window.
                    fillDialog = false;
                }
                break;
            }
        break;
        case WM_CLOSE: {
            EndDialog(hwnd, IDCANCEL);  // Exiting the window has the same affect as pressing cancel.
            fillDialog = false;
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

// TODO: make project only edit not create new.

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

        if (fillDialog == true) {  // Case: write over old row.
            HWND hListBoxProject = GetDlgItem(GetParent(hwnd), ID_LB_PROJECT);
            DWORD selectedId = SendMessage(hListBoxProject, LB_GETCURSEL, 0, 0);  // Get id of selected item.
            //printf("edit: process: ID: %i\n", selectedId);

            fileString = WriteRow(fileString, key, selectedId, value);  // Overwrite the selected row in the config file.

            // change the title of the listbox.
            SendMessage(hListBoxProject, LB_DELETESTRING, selectedId, 0);
            SendMessage(hListBoxProject, LB_INSERTSTRING, selectedId, (LPARAM)title);

            SetDlgItemText(GetParent(hwnd), ID_TEXT_PROJECT, title);  // This is how to set text to a control.
        }
        else {  // Case: write a new row and add 1 to project count.
            fileString = ConfigAddRow(fileString, key, value);  // Add a row to the config string.

            // Get how many projects there are.
            int projectCount = 0;
            char key2[] = "projectCount";
            ItemContents_t results = ReadItem(fileString, key2, 0, 0);
            projectCount = atoi(results.itemString);  // Get the integer value.
            DealocateItemContents(results);  // Dealocate the read item.

            // MAKE projectCount GO UP 1 EACH TIME A NEW ROW IS ADDED.
            projectCount++;

            int value2Length = strlen("\002\003") + N_DIGITS(projectCount)+1;
            char value2[value2Length];
            sprintf(value2, "\002\003%i", projectCount);
            fileString = WriteRow(fileString, key2, 0, value2);  // Update projectCount
        }

        // Write filestring to file.
        FILE *fptr = fopen(g_configFileName, "w");
        fprintf(fptr, fileString);
        fclose(fptr);

        if (fillDialog == false) {
            // Add the new project to the project selection list box.
            HWND hListBoxProject = GetDlgItem(GetParent(hwnd), ID_LB_PROJECT);
            SendMessage(hListBoxProject, LB_ADDSTRING, 0, (LPARAM)title);
        }
    }
    free(fileString);  // Dealocate fileString.
}

void PushError(void) {
    printf("Input invalid.\n");
}
