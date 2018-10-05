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
void ProcessInput(void);
void PushError(void);

// This is the callback loop for the create project window.
BOOL CALLBACK CreateProjectDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch(Message)
    {
        case WM_INITDIALOG:
            return TRUE;  // True means message was processed.  (This also sets focus to the new window.)
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDOK: {
                    TCHAR titleStr[128];  // TODO: FIX THIS. -> STRING[128]; ... I think>?
                    TCHAR pathStr[128];
                    GetDlgItemText(hwnd, DB_ID_EDIT_NAME, titleStr, 128);
                    GetDlgItemText(hwnd, DB_ID_EDIT_NAME, pathStr, 128);

                    if( VerifyInput(titleStr, pathStr) == TRUE ) {
                        ProcessInput();  // Write the project information to the data file. (In plaintext.)
                        EndDialog(hwnd, IDOK);  // This destroys the current window.
                    } else {
                        PushError();  // This says you can't exit via ok.
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

    bool isEmpty = FALSE;
    if (strcmp(title, "") == 0 || strcmp(path, "") == 0) {
        printf("Project title and path must to be filled in.\n");
        printf("title: %s, path: %s\n", title, path);
        isEmpty = TRUE;
    }

    bool isValidPath = TRUE;  // check if path is valid.

    bool containsValidChars = TRUE;  // make sure that no invalid characters exist.

    return !isEmpty && isValidPath && containsValidChars;
}

// This file writes to the program data file.
void ProcessInput(void) {
    printf("TODO: input code here to process the input.\n");
    /* open data file for writing */
    //fptr = fopen(".projectdata", "w");
}

void PushError(void) {
    printf("Input invalid.\n");
}
