#ifndef DIALOG
#define DIALOG
    #include <windows.h>
    #include <stdio.h>  // for printf()
    #include <math.h>

    #include "resource.h"
    #include "ptconfig.h"

    #define N_DIGITS(num) (num==0) ? 1 : floor(log10(abs(num))) + 1;  // This is just a tiny macro function.  (num must be >= 0)

    // This forward declaration is so that other files can use this function.
    BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
    BOOL CALLBACK CreateProjectDlgProc(HWND, UINT, WPARAM, LPARAM);
    void FillDialog(void);

#endif  //DIALOG
