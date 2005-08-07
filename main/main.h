///////////////////////////////////////////////////////////////////////////////
///
/// iNFO - Utility to view NFO and DIZ files
/// Copyright (C) 2005 Daniel Pérez Álvarez <unindented@gmail.com>
///
///////////////////////////////////////////////////////////////////////////////

#ifndef	_MAIN_H_
#define _MAIN_H_

#ifndef MAX_LOADSTRING
#define MAX_LOADSTRING 100
#endif // MAX_LOADSTRING

#define APP_NAME            _T("iNFO")
#define APP_PREFS_KEY       _T("Software\\iNFO")

#define DEFAULT_WIDTH       680
#define DEFAULT_HEIGHT      420

#define DEFAULT_LANGUI      0
#define DEFAULT_FONTNAME    _T("IBMPC")
#define DEFAULT_FONTSIZE    11
#define DEFAULT_EDITFG      RGB(255, 255, 255)
#define DEFAULT_EDITBG      RGB(0, 0, 0)

extern TCHAR g_szAppPath[MAX_PATH];        // application path
extern TCHAR g_szLangPath[MAX_PATH];       // language satellites path
extern TCHAR g_szOpenFileName[MAX_PATH];   // last opened file

extern TCHAR g_szAppTitle[MAX_LOADSTRING]; // application title
extern TCHAR g_szNFOName[MAX_LOADSTRING];  // reg key name for NFO files
extern TCHAR g_szNFODesc[MAX_LOADSTRING];  // reg key description for NFO files
extern TCHAR g_szDIZName[MAX_LOADSTRING];  // reg key name for DIZ files
extern TCHAR g_szDIZDesc[MAX_LOADSTRING];  // reg key description for DIZ files

extern LANGID g_langUI;                    // current language
extern TCHAR g_szFontName[LF_FACESIZE];    // edit control font name
extern LONG g_nFontSize;                   // edit control font size
extern COLORREF g_rgbEditFg;               // edit control foreground color
extern COLORREF g_rgbEditBg;               // edit control background color
extern BOOL g_bMaximized;                  // window maximized
extern LONG g_nWidth;                      // window width
extern LONG g_nHeight;                     // window height

extern HMODULE g_hSatDLL;                  // satellite DLL with localized resources

VOID ChangeLanguage(HWND, LANGID);
VOID ChangeFont(HWND, LPCTSTR, LONG);
VOID ChangeColor(HWND, COLORREF, COLORREF);
BOOL CALLBACK PrefDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

#endif // _MAIN_H_
