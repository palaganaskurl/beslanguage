#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "..\headers\PL_ENVIRONMENT.h"
#include "..\headers\LINKEDLIST.H"
#include "..\headers\LEXER.H"
#include "..\headers\PARSER.H"
#include "..\headers\CODE_GENERATION.h"

const char g_szClassName[] = "Bes Language Editor";
char openedFileName[MAX_PATH];
int isNewFile = 0;

char fileName[100];

int hasError = 0;

HWND hwnd, hEdit, hNotifs;
HMENU hMenu, hSubMenu;
WNDPROC lpEditWndProc;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        case WM_KEYDOWN:{
        	if(GetKeyState(VK_CONTROL) < 0 && (wParam == 'N' || wParam == 'n')){
		      	newFile();
		    } else if(GetKeyState(VK_CONTROL) < 0 && (wParam == 'O' || wParam == 'o')){
		      	isNewFile = 0;
				fileOpen();
			} else if(GetKeyState(VK_CONTROL) < 0 && wParam == VK_F9){
				compile();
			} else if(GetKeyState(VK_CONTROL) < 0 && wParam == VK_F12){
				run();
			}
			break;
		}
        case WM_CTLCOLOREDIT:{
			HDC hdcStatic = (HDC) wParam;
			SetTextColor(hdcStatic, RGB(0, 0, 0));
			SetBkColor(hdcStatic, RGB(192, 192, 192));
			return (INT_PTR) CreateSolidBrush(RGB(192, 192, 192));
		}
        case WM_SIZE:{
        	switch(LOWORD(wParam)){
        		case SIZE_RESTORED:{
        			ShowWindow(hwnd, SW_MAXIMIZE);
    				UpdateWindow(hwnd);		
					break;
				}
			}
			break;
		}
        case WM_COMMAND:{
        	switch(LOWORD(wParam)){
        		case ID_FILE_EXIT:{
        			DestroyWindow(hwnd);
        			break;
				}
				case ID_FILE_NEW:{
					newFile();
					break;
				}
				case ID_FILE_OPEN:{
					isNewFile = 0;
					fileOpen();
					break;
				}
				case ID_FILE_SAVE:{
					fileSave();
					break;
				}
				case ID_FILE_CLOSE:{
					if(hEdit != NULL){
						clearScreen();
						ModifyMenu(hMenu, ID_FILE_SAVE, MF_BYCOMMAND | MF_DISABLED, ID_FILE_SAVE, "&Save\tCtrl + S");
						ModifyMenu(hMenu, ID_FILE_CLOSE, MF_BYCOMMAND | MF_DISABLED, ID_FILE_CLOSE, "&Close\tCtrl + C");
						DestroyWindow(hEdit);
						DestroyWindow(hNotifs);
					}
					break;
				}
				case ID_STUFF_COMPILE:{
					compile();
					break;
				}
				case ID_STUFF_RUN:{
					run();
					break;
				}
			}
			break;
		}
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK MyEditCallBackProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    if((uMsg == WM_KEYDOWN) && GetKeyState(VK_CONTROL) < 0 && (wParam == 'N' || wParam == 'n')){
    	newFile();
	} else if((uMsg == WM_KEYDOWN) && GetKeyState(VK_CONTROL) < 0 && (wParam == 'O' || wParam == 'o')){
    	isNewFile = 0;
		fileOpen();
	} else if((uMsg == WM_KEYDOWN) && GetKeyState(VK_CONTROL) < 0 && (wParam == 'S' || wParam == 's')){
    	fileSave();
	} else if((uMsg == WM_KEYDOWN) && GetKeyState(VK_CONTROL) < 0 && wParam == VK_F9){
		compile();
	} else if((uMsg == WM_KEYDOWN) && GetKeyState(VK_CONTROL) < 0 && wParam == VK_F12){
		run();
	}
    return CallWindowProc(lpEditWndProc, hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    MSG Msg;

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc)){
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    DWORD dwExStyle, dwStyle;
    
    dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;  
    dwStyle=(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        dwExStyle,
        g_szClassName,
        "Bes Language Editor",
        dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 600,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL){
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HICON hIcon, hIconSm;

    hMenu = CreateMenu();

    hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_BYCOMMAND | MF_ENABLED | MF_STRING, ID_FILE_NEW, "&New\tCtrl + N");
    AppendMenu(hSubMenu, MF_BYCOMMAND | MF_ENABLED | MF_STRING, ID_FILE_OPEN, "&Open\tCtrl + O");
    AppendMenu(hSubMenu, MF_BYCOMMAND | MF_DISABLED | MF_STRING, ID_FILE_SAVE, "&Save\tCtrl + S");
    AppendMenu(hSubMenu, MF_SEPARATOR, ID_FILE_SEPARATOR, "");
    AppendMenu(hSubMenu, MF_BYCOMMAND | MF_DISABLED | MF_STRING, ID_FILE_CLOSE, "&Close\tCtrl + C");
    AppendMenu(hSubMenu, MF_SEPARATOR, ID_FILE_SEPARATOR, "");
    AppendMenu(hSubMenu, MF_BYCOMMAND | MF_ENABLED | MF_STRING, ID_FILE_EXIT, "E&xit\tCtrl + X");
    AppendMenu(hMenu, MF_BYCOMMAND | MF_ENABLED | MF_STRING | MF_POPUP, (UINT)hSubMenu, "&File");

    hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_BYCOMMAND | MF_ENABLED | MF_STRING, ID_STUFF_COMPILE, "&Compile\tCtrl + F9");
    AppendMenu(hSubMenu, MF_BYCOMMAND | MF_ENABLED | MF_STRING, ID_STUFF_RUN, "&Run\tCtrl + F12");
    AppendMenu(hMenu, MF_BYCOMMAND | MF_ENABLED | MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Execute");

    SetMenu(hwnd, hMenu);
    
    hIcon = LoadImage(NULL, "BesLanguage.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
    if(hIcon)
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    else
        MessageBox(hwnd, "Could not load large icon!", "Error", MB_OK | MB_ICONERROR);

    hIconSm = LoadImage(NULL, "BesLanguage.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
    if(hIconSm)
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);
    else
        MessageBox(hwnd, "Could not load small icon!", "Error", MB_OK | MB_ICONERROR);
	
	SetFocus(hwnd);
    ShowWindow(hwnd, SW_MAXIMIZE);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}

void createTextArea(){		
	if(hEdit != NULL){
		clearScreen();
		DestroyWindow(hEdit);
	}
	hEdit = CreateWindowEx(
			WS_EX_DLGMODALFRAME, "EDIT",   // predefined class 
			NULL,         // no window title 
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | 
			ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL, 
			150, 50, 800, 500,   // set size in WM_SIZE message 
			hwnd,         // parent window 
			(HMENU) IDC_MAIN_EDIT,   // edit control ID 
			(HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE), 
			NULL);        // pointer not needed 
	
	HFONT hFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, 
								OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
								DEFAULT_PITCH | FF_SCRIPT, TEXT("Tahoma"));
	
	SendMessage(hEdit,
				WM_SETFONT,
				(WPARAM) hFont,
				MAKELPARAM(FALSE,0));
}

void createNotifs(){
	if(hNotifs != NULL){
		clearScreen();
		DestroyWindow(hNotifs);
	}
	
	RECT rect;
	int width;
	if(GetWindowRect(hwnd, &rect)){
		width = rect.right - rect.left;
	}
	
	hNotifs = CreateWindowEx(
	            WS_EX_DLGMODALFRAME, "EDIT",   // predefined class 
	            NULL,         // no window title 
	            WS_CHILD | WS_VISIBLE | WS_VSCROLL | 
	            ES_LEFT | ES_READONLY | ES_MULTILINE | ES_AUTOVSCROLL, 
	            0, 570, width, 120,   // set size in WM_SIZE message 
	            hwnd,         // parent window 
	            (HMENU) IDC_MAIN_NOTIFS,   // edit control ID 
	            (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE), 
	            NULL);        // pointer not needed 
	
	lpEditWndProc = (WNDPROC) SetWindowLongPtr(hEdit, GWL_WNDPROC, (LONG_PTR) &MyEditCallBackProcedure);
	
	HFONT hFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, 
					OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
					DEFAULT_PITCH | FF_SCRIPT, TEXT("Tahoma"));
	
	SendMessage(hNotifs,
				WM_SETFONT,
				(WPARAM) hFont,
				MAKELPARAM(FALSE,0));
}

void clearScreen(){
	HDC hdc = GetWindowDC(hwnd);
	RECT rect = {155, 65, 300, 92};
	HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
	FillRect(hdc, &rect, brush);				
	DeleteObject(brush);
}

int openFile(){
	OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";
    FILE * fp;
    char * buffer = 0;
    long length;

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Bes Files (*.bes)\0*.bes\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "bes";

	if(isNewFile){
		GetSaveFileName(&ofn);
		strcpy(openedFileName, ofn.lpstrFile);
		isNewFile = 0;
		saveFile();
		return 1;
	}

    if(GetOpenFileName(&ofn)){        
        HANDLE hFile;
	    int bSuccess = 0;
	
		createTextArea();
		
		strcpy(openedFileName, szFileName);
	
	    hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
	        OPEN_EXISTING, 0, NULL);
	        
	    if(hFile != INVALID_HANDLE_VALUE){
	        DWORD dwFileSize;
	        dwFileSize = GetFileSize(hFile, NULL);
	        if(dwFileSize != 0xFFFFFFFF){
	            LPSTR pszFileText;
	            pszFileText = GlobalAlloc(GPTR, dwFileSize + 1);
	            if(pszFileText != NULL){
	                DWORD dwRead;
	                if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL)){
	                    pszFileText[dwFileSize] = 0; // Add null terminator
	                    if(SetWindowText(hEdit, pszFileText))
	                        bSuccess = 1; // It worked!
	                }
	                GlobalFree(pszFileText);
	            }
	        }
	        CloseHandle(hFile);
	    }
	    return bSuccess;
    } else {
    	return 0;
	}
    
}

int saveFile(){
	HANDLE hFile;
    int bSuccess = 0;
	char pszFileName[MAX_PATH] = "";
	
	strcpy(pszFileName, openedFileName);
	
    hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile != INVALID_HANDLE_VALUE){
        DWORD dwTextLength;

        dwTextLength = GetWindowTextLength(hEdit);
        // No need to bother if there's no text.
        if(dwTextLength >= 0){
            LPSTR pszText;
            DWORD dwBufferSize = dwTextLength + 1;

            pszText = GlobalAlloc(GPTR, dwBufferSize);
            if(pszText != NULL){
                if(GetWindowText(hEdit, pszText, dwBufferSize)){
                    DWORD dwWritten;

                    if(WriteFile(hFile, pszText, dwTextLength, &dwWritten, NULL))
                        bSuccess = 1;
                }
                GlobalFree(pszText);
            }
        }
        CloseHandle(hFile);
    }
    return bSuccess;
}

void newFile(){
	createTextArea();
	createNotifs();
	isNewFile = 1;
	if(hEdit != NULL){
		ModifyMenu(hMenu, ID_FILE_SAVE, MF_BYCOMMAND | MF_ENABLED, ID_FILE_SAVE, "&Save\tCtrl + S");
		ModifyMenu(hMenu, ID_FILE_CLOSE, MF_BYCOMMAND | MF_ENABLED, ID_FILE_CLOSE, "&Close\tCtrl + C");
		HDC hdc;
		hdc = GetWindowDC(hwnd);
		TextOut(hdc, 160, 70, "untitled.bes", strlen("untitled.bes"));
		ReleaseDC(hwnd, hdc);
	}
}

void fileOpen(){
	if(!openFile()){
		MessageBox(hwnd, "Opening file error!", "Error", MB_OK | MB_ICONERROR);
		return;
	}
	createNotifs();
	if(hEdit != NULL){
		ModifyMenu(hMenu, ID_FILE_SAVE, MF_BYCOMMAND | MF_ENABLED, ID_FILE_SAVE, "&Save\tCtrl + S");
		ModifyMenu(hMenu, ID_FILE_CLOSE, MF_BYCOMMAND | MF_ENABLED, ID_FILE_CLOSE, "&Close\tCtrl + C");
		HDC hdc;
		hdc = GetWindowDC(hwnd);
		int i, counter = 0;
		for(i = strlen(openedFileName) - 1; i > 0; i--){
			if(openedFileName[i] == '\\'){
				fileName[counter] = '\0';
				break;
			}
			fileName[counter] = openedFileName[i];
			counter++;
		}
		TextOut(hdc, 160, 70, strrev(fileName), strlen(fileName));
		ReleaseDC(hwnd, hdc);
	}
}

void fileSave(){
	if(isNewFile){
		openFile();
	} else {
		if(!saveFile()){
			MessageBox(hwnd, "Saving file error!", "Error", MB_OK | MB_ICONERROR);
		}
	}
	clearScreen();
	HDC hdc;
	hdc = GetWindowDC(hwnd);
	char fileName[100];
	int i, counter = 0;
	for(i = strlen(openedFileName) - 1; i > 0; i--){
		if(openedFileName[i] == '\\'){
			fileName[counter] = '\0';
			break;
		}
		fileName[counter] = openedFileName[i];
		counter++;
	}
	TextOut(hdc, 160, 70, strrev(fileName), strlen(fileName));
	ReleaseDC(hwnd, hdc);
}

void compile(){
	fileSave();
	SendMessage(hNotifs, WM_SETTEXT, 0, (LPARAM) "");	
	FILE *fp; // file input
	FILE *fo; // file output
	TokenList * tokenList;

	fp = fopen(openedFileName, "r");
	fo = fopen("SYMBOL TABLE.txt", "w");
	tokenList = createTokenList();
	initLexer(fp, fo);
	
	initToken(tokenList);
	readToken();
	generateCode(tokenList);
	fclose(fp);
	fclose(fo);
	
	if(strcmp(errorList[0], "") == 0){
		SendMessage(hNotifs, WM_SETTEXT, 0, (LPARAM) "No error");
		hasError = 0;	
	} else {
		hasError = 1;
		int i;
		for(i = 0; i < sizeof(errorList) / sizeof(errorList[0]); i++){
			int index = GetWindowTextLength(hNotifs);
			SetFocus(hNotifs);
			SendMessageA(hNotifs, EM_SETSEL, (WPARAM)index, (LPARAM)index); // set selection - end of text
			SendMessageA(hNotifs, EM_REPLACESEL, 0, (LPARAM) errorList[i]); // append!
		}	
	}
	memset(errorList, 0, sizeof(errorList) / sizeof(errorList[0]));	
}

void run(){
	if(!hasError){
		char compileString[1000] = "gcc -m32 gencode.c -o ";
		char * tempFile = strtok(fileName, ".");
		strcat(compileString, tempFile);
		system(compileString);
		system(tempFile);	
	}
}

