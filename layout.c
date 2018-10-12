#include "layout.h"

void CreateMenu(HWND);
void AddMainControls(HWND, HINSTANCE);
void InitLBProjects(void);
//void AddProjectControls(HWND, HINSTANCE);

// All window controls are global (but only in this file.)
static HWND hBtnBuild;
static HWND hStaticLabelBuild;
static HWND hBtnSelect;
static HWND hTextProject;
static HWND hBtnCreate;
static HWND hEditBuildComment;
static HWND hListBoxProject;
static HWND hStaticLabelProject;

//TODO: remove.
//static HWND hEditProjectName;
//static HWND hEditPath;
//static HWND hEditProjectDesc;

void InitLayout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance) {
    CreateMenu(hwnd);
    AddMainControls(hwnd, hInstance);
    //AddProjectControls(hwnd, hInstance);
    //SetDlgItemText(hwnd, ID_LB_PROJECT, "This is a string");  // This is how to set text to a control.
    //ShowWindow(hEditPath, SW_HIDE);  // Hide control
    //ShowWindow(hEditPath, SW_SHOW);  // Show control

    InitLBProjects();
}

// This function creates the menu bar.
void CreateMenu(HWND hwnd) {
    HMENU hMenu, hSubMenu;

    hMenu = CreateMenu();  // Create the entire menu.

    hSubMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&File");
    AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, "E&xit");

    hSubMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Project");
    AppendMenu(hSubMenu, MF_STRING, ID_PROJECT_NEW, "Create &New Project");
    AppendMenu(hSubMenu, MF_STRING, ID_PROJECT_DELETE, "&Delete Current Project");
    AppendMenu(hSubMenu, MF_STRING, ID_PROJECT_EDIT, "&Edit Current Project");

    hSubMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Help");
    AppendMenu(hSubMenu, MF_STRING, ID_HELP_ABOUT, "&About");

    SetMenu(hwnd, hMenu);
}

// This creates all the window controls.
void AddMainControls(HWND hwnd, HINSTANCE hInstance) {
    const int BRK = 8;  //break

    // Create the build button.
    hBtnBuild = CreateWindowEx(
        WS_EX_CLIENTEDGE, "BUTTON", "Build Program",
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
        BRK, BRK, 192, 64,
        hwnd, (HMENU) ID_BTN_BUILD, hInstance, NULL
    );

    // Create the label.
    hStaticLabelBuild = CreateWindowEx(
        WS_EX_APPWINDOW, "STATIC", "Build Comment:",
        WS_CHILD | WS_VISIBLE,
        BRK, BRK*2+64, 192, 24,
        hwnd, (HMENU) ID_LBL_BUILD, hInstance, NULL
    );

    // Create the comment Box
    hEditBuildComment = CreateWindowEx(
        WS_EX_CLIENTEDGE, "EDIT", "Insert build comments here",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_TABSTOP,
        BRK, BRK+24+64, 192, 192+BRK/2,
        hwnd, (HMENU) ID_EDIT_BUILD_COMMENT, hInstance, NULL
    );

    // Create the label.
    hStaticLabelProject = CreateWindowEx(
        WS_EX_APPWINDOW, "STATIC", "Selected Project:",
        WS_CHILD | WS_VISIBLE,
        BRK*2+192, BRK, 192, 16,
        hwnd, (HMENU) ID_LBL_PROJECT, hInstance, NULL
    );

    // Create a EDIT element (for text.)
    hTextProject = CreateWindowEx(
        WS_EX_STATICEDGE, "EDIT", "No Project Selected",
        WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY,
        BRK*2+192, BRK+16, 192, 24,
        hwnd, (HMENU) ID_TEXT_PROJECT, hInstance, NULL
    );

    // Create the project select listbox
    hListBoxProject = CreateWindowEx(
        WS_EX_CLIENTEDGE, "LISTBOX", NULL,
        WS_CHILD | WS_VISIBLE /*| WM_VSCROLL*/ | ES_AUTOVSCROLL | WS_TABSTOP | LBS_NOTIFY,
        BRK*2+192, BRK*2+24+16, 192, 192,
        hwnd, (HMENU) ID_LB_PROJECT, hInstance, NULL
    );

    // Create the select button.
    hBtnSelect = CreateWindowEx(
        WS_EX_CLIENTEDGE, "BUTTON", "Edit Current Project",  // the second parameter must be the string "button"
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
        BRK*2+192, BRK*3+24+192, 192, 24,
        hwnd, (HMENU) ID_BTN_EDIT, hInstance, NULL
    );

    // Create the Create Project button.
    hBtnCreate = CreateWindowEx(
        WS_EX_CLIENTEDGE, "BUTTON", "Create New Project",
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
        BRK*2+192, BRK*3.5+24*2+192, 192, 24,
        hwnd, (HMENU) ID_BTN_CREATE, hInstance, NULL
    );
}

void InitLBProjects(void) {
    // Read the file items.
    char* fileString = NULL;
    if ( getFileString(g_configFileName, fileString) == 1 ) {
        printf("ERROR with file Initialization.\n");
    } else {
        // Get how many projects there are.
        int projectCount = 0;
        char key[] = "projectCount";
        printf("fileStringOUT: %s\n\n", fileString);  //THE end character thing is part of this string.
        ItemContents_t results = ReadItem(fileString, key, 0, 0);
        projectCount = atoi(results.itemString);  // Get the integer value.
        DealocateItemContents(results);  // Dealocate the read item.
        printf("PROJECT_COUNT : %i\n", projectCount);

        char key2[] = "projects";
        for (int i=0; i<projectCount; i++) {
            results = ReadItem(fileString, key2, i, 0);  // Read item 1 to results
            SendMessage(hListBoxProject, LB_ADDSTRING, 0, (LPARAM)results.itemString);
            DealocateItemContents(results);  // Dealocate the read item.
        }
    }

    free(fileString);  // Dealocate fileString.

    //init listbox.
    //SendMessage(hListBoxProject, LB_ADDSTRING, 0, (LPARAM)"Project 1");
    //SendMessage(hListBoxProject, LB_ADDSTRING, 0, (LPARAM)"Project 2");
    //SendMessage(hListBoxProject, LB_ADDSTRING, 0, (LPARAM)"Project 3");
}

/*
// This creates all the window controls.
void AddProjectControls(HWND hwnd, HINSTANCE hInstance) {
    int xPos = 8*3+192*2;  // The x pos of this row.

    //Create The path enter box?
    hEditProjectName = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "Insert project name here",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
        xPos, 8*2+96, 192, 24,
        hwnd, (HMENU) ID_EDIT_PROJECT_NAME, hInstance, NULL
    );

    //Create The path enter box?
    hEditPath = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "Insert project path here",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
        xPos, 8*3+96+24, 192, 24,
        hwnd, (HMENU) ID_EDIT_PATH, hInstance, NULL
    );

    // Create a EDIT element for project desc.
    hEditProjectDesc = CreateWindowEx(
        WS_EX_CLIENTEDGE, "EDIT", "Insert project desc here",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
        xPos, 8*4+96+24*2, 192, 128,
        hwnd, (HMENU) ID_EDIT_PROJECT_DESC, hInstance, NULL
    );
}
*/
