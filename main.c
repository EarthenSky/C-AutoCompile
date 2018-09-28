// Build Commands:
//cd Desktop\cDev\learnprog
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
            if (ID_LBL_BUILD == GetDlgCtrlID((HWND)lParam)) {  // only affect the specific id'd dialog.
                HDC hdcStatic = (HDC) wParam;
                SetTextColor(hdcStatic, RGB(0,0,0));  // Set colour.
                SetBkMode(hdcStatic, TRANSPARENT);
                return (INT_PTR)(HBRUSH)GetStockObject(NULL_BRUSH);
            }
        }
        break;
        //case WM_LBUTTONDOWN:
        //{
            //char szFileName[MAX_PATH];
            //HINSTANCE hInstance = GetModuleHandle(NULL);

            //GetModuleFileName(hInstance, szFileName, MAX_PATH);
            //MessageBox(hwnd, szFileName, "This program is located at:", MB_OK | MB_ICONINFORMATION);
        //}
        //break;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case ID_FILE_EXIT:
                    printf("exiting program.\n");
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                break;
                case ID_HELP_ABOUT:
                {
                    printf("open-about.\n");
                    int ret = DialogBox(hInstance, MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlgProc);  // 'pop-up' the new window.
                    printf("close-about\n");
                    if(ret == -1)
                        MessageBox(hwnd, "Dialog failed!", "Error", MB_OK | MB_ICONINFORMATION);
                }
                break;
                case ID_STUFF_GO:
                    printf("just go.\n");
                break;
                case ID_BTN_BUILD:
                    BuildProgram();
                    printf("open-create-proj.\n");
                    int ret = DialogBox(hInstance, MAKEINTRESOURCE(IDD_CREATE_PROJECT), hwnd, CreateProjectDlgProc);  // 'pop-up' the new window.
                    printf("close-create-proj\n");
                    if(ret == -1)
                        MessageBox(hwnd, "Dialog failed!", "Error", MB_OK | MB_ICONINFORMATION);
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

void InitProgram(void) {
    printf("Initializing Program.\n");

    FILE *fptr;
    fptr = fopen(g_configFileName, "r");

    //Create the data file if it doesn't exist.
    if (fptr == NULL) {
        printf("Creating config file.\n");
        fptr = fopen(g_configFileName, "w");
    }

    fclose(fptr);  // Close file.

    // Init the file with titles and subtitles.
    //char formatString[] = "projects";
    char formatString[] = "title.subtitle;title2.subtitle.subtitle2.subtitle3;numbers";
    FormatConfig(formatString);

    // Get file as string.
    char* fileString = NULL;
    if ( getFileString(g_configFileName, fileString) == 1 ) {
        printf("ERROR with file Initialization.\n");
    } else {
        printf("Out - File Contents: %s\n", fileString);  //DEBUG: this.
        char key[] = "title2.subtitle2";
        char value[] = "\003hey1\003hello2";
        ConfigAddRow(fileString, key, value);
        printf("Out - File Contents2: %s\n", fileString);  //DEBUG: this.

        fptr = fopen(g_configFileName, "w");
        fprintf(fptr, fileString);
        fclose(fptr);  // Close file.
    }

    free(fileString);  // dealocate fileString.



    //TODO: read the data file and fill the listbox with the existing projects.

    printf("Done Initialization.\n");
}

// This function will build the program.
void BuildProgram(void) {
    printf("Starting Building Program.\n");
    printf("Error: Building not yet implemented.\n");
}
