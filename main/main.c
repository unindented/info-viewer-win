///////////////////////////////////////////////////////////////////////////////
///
/// iNFO - Utility to view NFO and DIZ files
/// Copyright (C) 2005 Daniel Pérez Álvarez <unindented@gmail.com>
///
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include "main.h"
#include "langutils.h"
#include "regutils.h"
#include "miscutils.h"
#include "resource.h"

// paths
TCHAR g_szAppPath[MAX_PATH];        // application path
TCHAR g_szLangPath[MAX_PATH];       // language satellites path
TCHAR g_szOpenFileName[MAX_PATH];   // last opened file

// obtained from the resource file
TCHAR g_szAppTitle[MAX_LOADSTRING]; // application title
TCHAR g_szNFOName[MAX_LOADSTRING];  // reg key name for NFO files
TCHAR g_szNFODesc[MAX_LOADSTRING];  // reg key description for NFO files
TCHAR g_szDIZName[MAX_LOADSTRING];  // reg key name for DIZ files
TCHAR g_szDIZDesc[MAX_LOADSTRING];  // reg key description for DIZ files
TCHAR g_szFilter[MAX_LOADSTRING];   // filter for Open dialog

// obtained from the registry
LANGID g_langUI;                    // current language
TCHAR g_szFontName[LF_FACESIZE];    // edit control font size
LONG g_nFontSize;                   // edit control font name
COLORREF g_rgbEditFg;               // edit control foreground color
COLORREF g_rgbEditBg;               // edit control background color

// other stuff
HMODULE g_hSatDLL;                  // satellite DLL with localized resources
HBRUSH g_hBrush;                    // brush to paint the edit control

void LoadRegistrySettings();
void SaveRegistrySettings();
void UpdateTitle(HWND);
void DoFileOpen(HWND);
BOOL LoadTextFileToEdit(HWND, LPCTSTR);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/// <summary>
/// Entry point.
/// </summary>
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int nCmdShow)
{
    HWND hWnd;
    HACCEL hAccel;
    MSG msg;
    WNDCLASSEX wc;
    LPTSTR lpszFilePart;

    // get the name of the executable
    GetModuleFileName(NULL, g_szAppPath, MAX_PATH);
    // cut the file part, so we get the application directory
    lpszFilePart = _tcsrchr(g_szAppPath, _T('\\'));
    if (NULL != lpszFilePart)
    {
        *lpszFilePart = _T('\0');
    }
    // check string lengths
    if (lstrlen(g_szAppPath) + 6 <= MAX_PATH)
    {
        // obtain the language satellites directory
        wsprintf(g_szLangPath, _T("%s\\lang"), g_szAppPath);
    }

    ChangeLanguage(NULL, 0);

    wc.cbSize        = sizeof (WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(g_hSatDLL, (LPCTSTR)IDI_BIG);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName  = 0;
    wc.lpszClassName = APP_NAME;
    wc.hIconSm       = LoadIcon(g_hSatDLL, (LPCTSTR)IDI_SMALL);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, _T("Could not register window."), _T("Error"),
            MB_OK | MB_ICONERROR);
        return FALSE;
    }

    hWnd = CreateWindowEx(WS_EX_ACCEPTFILES, APP_NAME, g_szAppTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_WIDTH, DEFAULT_HEIGHT,
        NULL, NULL, hInstance, NULL);

    if (NULL == hWnd)
    {
        MessageBox(NULL, _T("Could not create window."), _T("Error"),
            MB_OK | MB_ICONERROR);
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // load the accelerators
    hAccel = LoadAccelerators(g_hSatDLL, (LPCTSTR)IDC_MAIN);

    // message loop
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        // don't forget to process accelerator keys
        if (!TranslateAccelerator(hWnd, hAccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

/// <summary>
/// Main window procedure.
/// </summary>
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        {
            HWND hEdit;
            TLOGFONT lf;
            HFONT hFont;
            LPCTSTR lpszCmdLine;
            TCHAR szFileName[MAX_PATH];

            // load settings from the registry
            LoadRegistrySettings();

            // if there was no value for language, autodetect
            if (0 == g_langUI)
            {
                g_langUI = DetectLanguage();
            }
            ChangeLanguage(hWnd, g_langUI);

            // create the edit control
            hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T(""),
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_NOHIDESEL | ES_READONLY,
                0, 0, 0, 0,
                hWnd, (HMENU)IDC_EDIT_MAIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // get a handle to the OEM fixed font, and copy its information to
            // a LOGFONT structure
            GetObject(GetStockObject(OEM_FIXED_FONT), sizeof (TLOGFONT), &lf);
            // change some font attributes
            lf.lfHeight = g_nFontSize;
            lf.lfWidth = 0;
            lstrcpy(lf.lfFaceName, g_szFontName);
            // create the font
            hFont = CreateFontIndirect(&lf);
            // set the font
            SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

            // create a brush which will be used to paint the background
            g_hBrush = CreateSolidBrush(g_rgbEditBg);

            // if a command line was passed, we try to load the first argument
            lpszCmdLine = GetCommandLine();
            GetCommandLineArgument(szFileName, lpszCmdLine, 1);
            LoadTextFileToEdit(hWnd, szFileName);
        }
        break;

    case WM_CTLCOLORSTATIC:
        // if it's the edit control, set its foreground and background colors
        // and return the brush created previously
        if (IDC_EDIT_MAIN == GetDlgCtrlID((HWND)lParam))
        {
            SetTextColor((HDC)wParam, g_rgbEditFg);
            SetBkColor((HDC)wParam, g_rgbEditBg);
            return (LRESULT)g_hBrush;
        }
        break;

    case WM_SETFOCUS:
        {
            // give the focus to the edit control so that the user can scroll
            SetFocus(GetDlgItem(hWnd, IDC_EDIT_MAIN));
        }
        break;

    case WM_SIZE:
        {
            RECT rcClient;

            // resize the edit control so that it fits the window
            GetClientRect(hWnd, &rcClient);
            SetWindowPos(GetDlgItem(hWnd, IDC_EDIT_MAIN), NULL,
                0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
        }
        break;

    case WM_DROPFILES:
        {
            TCHAR szFileName[MAX_PATH];

            // load the dropped file
            DragQueryFile((HDROP)wParam, 0, szFileName, MAX_PATH);
            if (!LoadTextFileToEdit(hWnd, szFileName))
            {
                MessageBox(hWnd, _T("Could not open file."), _T("Error"),
                    MB_OK | MB_ICONEXCLAMATION);
            }
            DragFinish((HDROP)wParam);
            // bring the window to foreground after drag & drop
            SetForegroundWindow(hWnd);
        }
        break;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case ID_FILE_OPEN:
            DoFileOpen(hWnd);
            break;

        case ID_FILE_EXIT:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;

        case ID_EDIT_PREF:
            DialogBox(g_hSatDLL, (LPCTSTR)IDD_PREF, hWnd,
                (DLGPROC)PrefDlgProc);
            break;

        case ID_HELP_ABOUT:
            DialogBox(g_hSatDLL, (LPCTSTR)IDD_ABOUT, hWnd,
                (DLGPROC)AboutDlgProc);
            break;
        }
        break;

    case WM_CLOSE:
        // save settings to the registry
        SaveRegistrySettings();

        // clean up
        DeleteObject(g_hBrush);
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}

/// <summary>
/// Load settings from the registry.
/// </summary>
void LoadRegistrySettings()
{
    HKEY hKey;

    // read settings for the current user
    if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, APP_PREFS_KEY, 0, 0, 0, KEY_READ, 0, &hKey, 0))
    {
        ReadRegistryInt(hKey, _T("Language"), (LPLONG)&g_langUI, DEFAULT_LANGUI);
        ReadRegistryStr(hKey, _T("FontName"), g_szFontName, LF_FACESIZE, DEFAULT_FONTNAME);
        ReadRegistryInt(hKey, _T("FontSize"), &g_nFontSize, DEFAULT_FONTSIZE);
        ReadRegistryInt(hKey, _T("EditForeground"), &g_rgbEditFg, DEFAULT_EDITFG);
        ReadRegistryInt(hKey, _T("EditBackground"), &g_rgbEditBg, DEFAULT_EDITBG);
        RegCloseKey(hKey);
    }
}

/// <summary>
/// Load settings to the registry.
/// </summary>
void SaveRegistrySettings()
{
    HKEY hKey;

    // write settings for the current user
    if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, APP_PREFS_KEY, 0, 0, 0, KEY_WRITE, 0, &hKey, 0))
    {
        WriteRegistryInt(hKey, _T("Language"), g_langUI);
        WriteRegistryStr(hKey, _T("FontName"), g_szFontName);
        WriteRegistryInt(hKey, _T("FontSize"), g_nFontSize);
        WriteRegistryInt(hKey, _T("EditForeground"), g_rgbEditFg);
        WriteRegistryInt(hKey, _T("EditBackground"), g_rgbEditBg);
        RegCloseKey(hKey);
    }
}

/// <summary>
/// Changes the language of the window.
/// </summary>
void ChangeLanguage(HWND hWnd, LANGID langID)
{
    g_langUI = langID;
    g_hSatDLL = LoadSatelliteDLL(g_langUI, g_szLangPath, _T("res.dll"));
    g_hSatDLL = (NULL != g_hSatDLL) ? g_hSatDLL : GetModuleHandle(NULL);

    // load strings
    LoadString(g_hSatDLL, IDS_APP_TITLE, g_szAppTitle, MAX_LOADSTRING);
    LoadString(g_hSatDLL, IDS_NFO_NAME, g_szNFOName, MAX_LOADSTRING);
    LoadString(g_hSatDLL, IDS_NFO_DESC, g_szNFODesc, MAX_LOADSTRING);
    LoadString(g_hSatDLL, IDS_DIZ_NAME, g_szDIZName, MAX_LOADSTRING);
    LoadString(g_hSatDLL, IDS_DIZ_DESC, g_szDIZDesc, MAX_LOADSTRING);
    LoadString(g_hSatDLL, IDS_FILTER, g_szFilter, MAX_LOADSTRING);
    // replace the '|' separators in the filter string with '\0'
    {
        LPTSTR lpszTemp = g_szFilter;
        while (*lpszTemp != '\0')
        {
            if (*lpszTemp == '|')
            {
                *lpszTemp = '\0';
            }
            lpszTemp++;
        }
    }

    if (hWnd)
    {
        HICON hIcon;
        HMENU hMenu;

        // redraw the client area of the main window
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);

        // update the icon
        hIcon = LoadIcon(g_hSatDLL, (LPCTSTR)IDI_BIG);
        SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        hIcon = LoadIcon(g_hSatDLL, (LPCTSTR)IDI_SMALL);
        SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

        // update the title of the main window
        LoadString(g_hSatDLL, IDS_APP_TITLE, g_szAppTitle, MAX_LOADSTRING);
        UpdateTitle(hWnd);

        // load the localized menu
        DestroyMenu(GetMenu(hWnd));
        hMenu = LoadMenu(g_hSatDLL, (LPCTSTR)IDC_MAIN);
        SetMenu(hWnd, hMenu);
        DrawMenuBar(hWnd);
    }
}

/// <summary>
/// Updates the title of the window.
/// </summary>
void UpdateTitle(HWND hWnd)
{
    HANDLE hHeap = GetProcessHeap();
    LPTSTR szShortFileName;
    LPTSTR lpszWindowTitle;

    // check if there is a file open
    if (0 == lstrlen(g_szOpenFileName))
    {
        // if there isn't, just show the name of the app
        SetWindowText(hWnd, g_szAppTitle);
    }
    else
    {
        // obtain the short filename
        szShortFileName = _tcsrchr(g_szOpenFileName, _T('\\'));
        szShortFileName = (NULL != szShortFileName) ? (szShortFileName + 1) : g_szOpenFileName;

        // show the filename in the title of the window
        lpszWindowTitle = HeapAlloc(hHeap, HEAP_ZERO_MEMORY,
            (lstrlen(szShortFileName) + lstrlen(g_szAppTitle) + 4) * sizeof (TCHAR));
        if (NULL != lpszWindowTitle)
        {
            wsprintf(lpszWindowTitle, _T("%s - %s"), szShortFileName, g_szAppTitle);
            SetWindowText(hWnd, lpszWindowTitle);

            HeapFree(hHeap, 0, lpszWindowTitle);
        }
    }
}

/// <summary>
/// Shows an Open common dialog box that lets the user select the file to load.
/// </summary>
/// <param name="hWnd">Handle to the parent window.</param>
void DoFileOpen(HWND hWnd)
{
    OPENFILENAME ofn;
    TCHAR szFileName[MAX_PATH] = _T("");

    ZeroMemory(&ofn, sizeof (ofn));
    ofn.lStructSize = sizeof (OPENFILENAME);
    ofn.hwndOwner   = hWnd;
    ofn.lpstrFilter = g_szFilter;
    ofn.lpstrFile   = szFileName;
    ofn.nMaxFile    = MAX_PATH;
    ofn.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    // show open common dialog
    if (GetOpenFileName(&ofn))
    {
        // try to load the file, and show a message in case of error
        if (!LoadTextFileToEdit(hWnd, szFileName))
        {
            MessageBox(hWnd, _T("Could not open file."), _T("Error"),
                MB_OK | MB_ICONEXCLAMATION);
        }
    }
}

/// <summary>
/// Loads a text file into the edit control.
/// </summary>
/// <param name="hWnd">Handle to the parent window.</param>
/// <param name="lpszFileName">File name.</param>
/// <returns>If the file was loaded correctly, the return value is nonzero. Else,
/// the return value is zero.</returns>
BOOL LoadTextFileToEdit(HWND hWnd, LPCTSTR lpszFileName)
{
    HWND hEdit;
    HANDLE hFile;
    BOOL bSuccess = FALSE;

    hFile = CreateFile(lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE != hFile)
    {
        DWORD dwFileSize;

        dwFileSize = GetFileSize(hFile, NULL);
        if (0xFFFFFFFF != dwFileSize)
        {
            HANDLE hHeap = GetProcessHeap();
            LPTSTR lpszFileContents;
            LPTSTR lpszSrcBuffer;
            LPTSTR lpszDstBuffer;

            // we still don't know if the file is Unicode or not, so we
            // allocate space for an Unicode null-terminator
            lpszFileContents = HeapAlloc(hHeap, HEAP_ZERO_MEMORY,
                dwFileSize + sizeof (wchar_t));
            if (NULL != lpszFileContents)
            {
                DWORD dwRead;

                if (ReadFile(hFile, lpszFileContents, dwFileSize, &dwRead, NULL))
                {
                    int cch;

#ifdef UNICODE

                    // file is already Unicode
                    if (IsTextUnicode(lpszFileContents, dwFileSize, NULL))
                    {
                        // calculate the number of characters needed to hold
                        // the string
                        cch = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)lpszFileContents,
                            -1, NULL, 0, NULL, NULL);

                        lpszSrcBuffer = lpszFileContents;
                    }
                    // file is ANSI, convert to Unicode
                    else
                    {
                        // calculate the number of characters needed to hold
                        // the string
                        cch = MultiByteToWideChar(CP_ACP, 0, (LPSTR)lpszFileContents,
                            -1, NULL, 0);

                        // allocate a buffer big enough to store the ANSI
                        // string in Unicode format
                        lpszSrcBuffer = HeapAlloc(hHeap, HEAP_ZERO_MEMORY,
                            cch * sizeof (wchar_t));
                        if (NULL != lpszSrcBuffer)
                        {
                            // convert ANSI string to Unicode
                            MultiByteToWideChar(CP_ACP, 0, (LPSTR)lpszFileContents,
                                -1, lpszSrcBuffer, cch);
                        }
                    }

#else

                    // file is Unicode, convert to ANSI
                    if (IsTextUnicode(lpszFileContents, dwFileSize, NULL))
                    {
                        // calculate the number of characters needed to hold
                        // the string
                        cch = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)lpszFileContents,
                            -1, NULL, 0, NULL, NULL);

                        // allocate a buffer big enough to store the Unicode
                        // string in ANSI format
                        lpszSrcBuffer = HeapAlloc(hHeap, HEAP_ZERO_MEMORY,
                            cch * sizeof (char));
                        if (NULL != lpszSrcBuffer)
                        {
                            // convert Unicode string to ANSI
                            WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)lpszFileContents,
                                -1, lpszSrcBuffer, cch, NULL, NULL);
                        }
                    }
                    // file is already ANSI
                    else
                    {
                        // calculate the number of characters needed to hold
                        // the string
                        cch = MultiByteToWideChar(CP_ACP, 0, (LPSTR)lpszFileContents,
                            -1, NULL, 0);

                        lpszSrcBuffer = lpszFileContents;
                    }

#endif /* UNICODE */

                    // in the worst case, all characters are linebreaks,
                    // so we have to allocate twice the space
                    lpszDstBuffer = HeapAlloc(hHeap, HEAP_ZERO_MEMORY,
                        2 * cch * sizeof (TCHAR));
                    if (NULL != lpszDstBuffer)
                    {
                        AdjustLineBreaks(lpszDstBuffer, lpszSrcBuffer);

                        // put the text in the edit control
                        hEdit = GetDlgItem(hWnd, IDC_EDIT_MAIN);
                        if (SetWindowText(hEdit, lpszDstBuffer))
                        {
                            // update the title of the window
                            lstrcpy(g_szOpenFileName, lpszFileName);
                            UpdateTitle(hWnd);

                            bSuccess = TRUE;
                        }

                        if (lpszSrcBuffer != lpszFileContents)
                        {
                            HeapFree(hHeap, 0, lpszSrcBuffer);
                        }
                        HeapFree(hHeap, 0, lpszDstBuffer);
                    }
                }

                HeapFree(hHeap, 0, lpszFileContents);
            }
        }

        CloseHandle(hFile);
    }

    return bSuccess;
}
