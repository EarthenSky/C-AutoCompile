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
    const int WINDOW_WIDTH = 8*4+192*3+16+2;
    const int WINDOW_HEIGHT = 464;

    //const char BUTTON_NAME[] = "button";
    //const char EDIT_NAME[] = "edit";

    void BuildProgram(void);
    void InitProgram(void);

#endif  //MAIN
