#ifndef MAIN
#define MAIN
    #include <windows.h>
    #include <stdio.h>

    //#include "unistd.h"

    #include "resource.h"
    #include "dialog.h"
    #include "layout.h"
    #include "ptconfig.h"  // This contains functions for interacting with the config file.

    const char g_className[] = "testWindowClass";
    const int WINDOW_WIDTH = 8*4+192*2+2;
    const int WINDOW_HEIGHT = 8*3+192+24*6-6;

    #define N_DIGITS(num) (num==0) ? 1 : floor(log10(abs(num))) + 1;  // This is just a tiny macro function.  (num must be >= 0)

    void BuildProgram(void);
    void InitProgram(void);
    void CreateNewProject(HINSTANCE, HWND);
    void EditProject(HINSTANCE, HWND);
    void DeleteProject(HWND);

#endif  //MAIN
