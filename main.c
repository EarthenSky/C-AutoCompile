// Build Commands:
//cd C:/Github/C-AutoCompile
//windres resc.rc resc.o
//g++ main.c main.h dialog.h dialog.c layout.h layout.c resource.h resc.o -o main.exe -lgdi32
//g++ main.c main.h dialog.h dialog.c layout.h layout.c ptconfig.h ptconfig.c resource.h resc.o -o main.exe -lgdi32

#include "main.h"
//#include "wingdi.h"  // Don't need.

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    switch(msg)
    {
        case WM_CREATE:
        {
            InitProgram();
            InitLayout(hwnd, msg, wParam, lParam, hInstance);  // Adds all the controls (see layout.c)
        }
        break;
        case WM_CTLCOLORSTATIC:  // Make static controls have a TRANSPARENT background.
        {
            if (GetDlgCtrlID((HWND)lParam) == ID_LBL_BUILD ||
                GetDlgCtrlID((HWND)lParam) == ID_LBL_PROJECT) {  // only affect the specific ids' attributes.

                HDC hdcStatic = (HDC) wParam;
                SetTextColor(hdcStatic, RGB(0,0,0));  // Set colour.
                SetBkMode(hdcStatic, TRANSPARENT);
                return (INT_PTR)(HBRUSH)GetStockObject(NULL_BRUSH);
            }
        }
        break;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case ID_FILE_EXIT: {
                    printf("exiting program.\n");
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                }
                break;
                case ID_HELP_ABOUT: {
                    printf("open-about.\n");
                    int ret = DialogBox(hInstance, MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlgProc);  // 'pop-up' the new window.
                    printf("close-about\n");
                    if(ret == -1)
                        MessageBox(hwnd, "Dialog failed!", "Error", MB_OK | MB_ICONINFORMATION);
                }
                break;
                case ID_BTN_BUILD: {
                    BuildProgram();
                }
                break;
                case ID_PROJECT_NEW:
                case ID_BTN_CREATE: {
                    CreateNewProject(hInstance, hwnd);
                }
                break;
                case ID_PROJECT_EDIT:
                case ID_BTN_EDIT: {
                    EditProject((HWND)lParam);  // Edit the project.
                }
                break;
                case ID_PROJECT_DELETE: {
                    DeleteProject(hwnd);  // Edit the project.
                }
                break;
                case ID_LB_PROJECT: {
                    if ( HIWORD(wParam) == LBN_SELCHANGE ) {
                        DWORD selectedId = SendMessage((HWND)lParam, LB_GETCURSEL, 0, 0);  // Get id of selected item.
                        if (selectedId >= 0) {
                            // Get string of selected item.
                            int stringLength = SendMessage((HWND)lParam, LB_GETTEXTLEN, selectedId, 0);
                            char holderString[stringLength];
                            SendMessage((HWND)lParam, LB_GETTEXT, selectedId, (LPARAM)holderString);
                            printf("holderString: %s\n", holderString);
                            SetDlgItemText(hwnd, ID_TEXT_PROJECT, holderString);  // This is how to set text to a control.
                        } else {
                            printf("ERROR:8982 (this is just a random number)\n");
                        }
                    }
                }
                break;
            }
        break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);  // Do basic window things.
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;  // wc is window class template.
    HWND hwnd;  // This is the window.
    MSG Msg;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_className;
    //wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MAINMENU);  // Attach the menu.
    // Attach the 32x32 icon.  (default size)
    wc.hIcon  = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));
    // Attach the small icon.  (same image)
    wc.hIconSm  = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 16, 16, 0);

    if(RegisterClassEx(&wc) == false) {  // Chcek if the window was created properly.
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create the window.
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE, g_className,
        "This is the window title",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL
    );

    if(hwnd == NULL) {  // Check if window was created properly.
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // This is the message loop.
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}

void InitConfig(void) {
    // Init the file with titles and subtitles.
    char formatString[] = "projects;projectCount";
    FormatConfig(formatString);

    // Retrieve file as string.
    char* fileString = NULL;
    if ( getFileString(g_configFileName, fileString) == 1 ) {
        printf("ERROR with file Initialization.\n");
    } else {
        // Create a default project
        char key[] = "projects";
        // Data is stored as (name, path, desc, append to command)
        char value[] = "\002\003Project Name\003C:\\_Programming\\TestProject\003Project Description\003-append this to the command string.";
        fileString = ConfigAddRow(fileString, key, value);  // Add a row to the config string.

        // REMEMBER TO MAKE THIS GO UP EACH TIME A NEW ROW IS ADDED.
        char key2[] = "projectCount";
        char value2[] = "\002\0031";  // init as 1.
        fileString = ConfigAddRow(fileString, key2, value2);  // Add a row to the config string.

        //ItemContents_t results = ReadItem(fileString, key, 1, 2);  // Read item 1 to results
        //printf("Out - Results: %c\n", results.contentIdentifier);  //DEBUG: this.
        //printf("Out - Results: %s\n", results.itemString);  //DEBUG: this.
        //DealocateItemContents(results);  // Dealocate the read item.
        //fileString = WriteRow(fileString, key, 1, value4);
        //printf("Out - File Contents After: %s\n", fileString);  //DEBUG: this.

        // Open Write filsstring to file.
        FILE *fptr = fopen(g_configFileName, "w");
        fprintf(fptr, fileString);
        fclose(fptr);
    }

    free(fileString);  // Dealocate fileString.
}

void InitProgram(void) {
    printf("Initializing Program.\n");

    FILE *fptr;
    fptr = fopen(g_configFileName, "r");

    //Create the data file if it doesn't exist.
    if (fptr == NULL) {
        printf("Creating config file.\n");
        InitConfig();  // Create the config file.
    }

    fclose(fptr);  // Close file.

    //TODO: read the data file and fill the listbox with the existing projects ACTUALLY: just do this in layout.c

    printf("Done Initialization.\n");
}

// This function will build the program.
void BuildProgram(void) {
    printf("Error: Building not yet implemented.\n");
}

// This function will build the program.
void CreateNewProject(HINSTANCE hInstance, HWND hwnd) {
    printf("\topen-create-proj.\n");
    int ret = DialogBox(hInstance, MAKEINTRESOURCE(IDD_CREATE_PROJECT), hwnd, CreateProjectDlgProc);  // 'pop-up' the new window.
    printf("\tclose-create-proj\n");
    if(ret == -1)
        MessageBox(hwnd, "Dialog failed!", "Error", MB_OK | MB_ICONINFORMATION);
}

// hwnd == hWindow;
void EditProject(HWND hwnd) {
    printf("Error: EditProject not yet implemented.\n");
}

// hwnd == hWindow;
void DeleteProject(HWND hwnd) {
    // Get the listbox handle.
    HWND hListBoxProject = GetDlgItem(hwnd, ID_LB_PROJECT);
    DWORD selectedId = SendMessage(hListBoxProject, LB_GETCURSEL, 0, 0);  // Get id of selected item.
    printf("ID: %i\n", selectedId);
    if (selectedId != LB_ERR) {
        printf("NO00000000!\n");
        // Retrieve file as string.
        char* fileString = NULL;
        if ( getFileString(g_configFileName, fileString) == 1 ) {
            printf("ERROR with file Initialization.\n");
        } else {
            char key[] = "projects";  char value[] = "";
            fileString = WriteRow(fileString, key, selectedId, value);  // Write an empty row.

            // Get how many projects there are.
            int projectCount = 0;
            char key2[] = "projectCount";
            ItemContents_t results = ReadItem(fileString, key2, 0, 0);
            projectCount = atoi(results.itemString);  // Get the integer value.
            DealocateItemContents(results);  // Dealocate the read item.
            projectCount--;  // MAKE projectCount GO DOWN 1 EACH TIME A NEW ROW IS REMOVED.
            printf("ALIVE\n");
            int value2Length = strlen("\002\003") + N_DIGITS(projectCount)+1;
            char value2[value2Length];
            printf("ALIVE2\n");
            sprintf(value2, "\002\003%i", projectCount);
            printf("ALIVE3\n");
            fileString = WriteRow(fileString, key2, 0, value2);  // Update projectCount
            printf("ALIVE4\n");
            // Open Write filestring to file.
            FILE *fptr = fopen(g_configFileName, "w");
            fprintf(fptr, fileString);
            fclose(fptr);
        }
        free(fileString);  // Dealocate fileString.

        // Remove the selected project from the project selection list box.
        SendMessage(hListBoxProject, LB_DELETESTRING, selectedId, 0);
    } else {
        printf("ERROR:7364 (this is just a random number)\n");
    }
}
