#ifndef DIALOG
#define DIALOG
    #include <windows.h>
    #include <stdio.h>  // for printf()

    #include "resource.h"

    // This forward declaration is so that other files can use this function.
    BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
    BOOL CALLBACK CreateProjectDlgProc(HWND, UINT, WPARAM, LPARAM);

#endif  //DIALOG
