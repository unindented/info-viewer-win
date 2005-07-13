///////////////////////////////////////////////////////////////////////////////
///
/// iNFO - Utility to view NFO and DIZ files
/// Copyright (C) 2005 Daniel Pérez Álvarez <unindented@gmail.com>
///
///////////////////////////////////////////////////////////////////////////////

#ifndef	_MAIN_H_
#define _MAIN_H_

#ifdef UNICODE
typedef LOGFONTW TLOGFONT;
#else
typedef LOGFONT TLOGFONT;
#endif // UNICODE

#define APP_NAME            _T("iNFO")
#define APP_PATHS_KEY       _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\info.exe")
#define APP_PREFS_KEY       _T("Software\\iNFO")

#define MAX_LOADSTRING      100

#define DEFAULT_WIDTH       680
#define DEFAULT_HEIGHT      420

#define DEFAULT_LANGUI      0
#define DEFAULT_FONTNAME    _T("IBMPC")
#define DEFAULT_FONTSIZE    14
#define DEFAULT_EDITFG      0xFFFFFF
#define DEFAULT_EDITBG      0x000000

extern TCHAR g_szAppPath[MAX_PATH];         // application path
extern TCHAR g_szLangPath[MAX_PATH];        // language satellites path

extern TCHAR g_szAppTitle[MAX_LOADSTRING];  // window title
extern TCHAR g_szNFOName[MAX_LOADSTRING];   // reg key name for NFO files
extern TCHAR g_szNFODesc[MAX_LOADSTRING];   // reg key description for NFO files
extern TCHAR g_szDIZName[MAX_LOADSTRING];   // reg key name for DIZ files
extern TCHAR g_szDIZDesc[MAX_LOADSTRING];   // reg key description for DIZ files
extern TCHAR g_szFilter[MAX_LOADSTRING];    // filter for Open dialog

extern LANGID g_langUI;                     // language
extern TCHAR g_szFontName[LF_FACESIZE];     // edit control font name
extern LONG g_lFontSize;                    // edit control font size
extern COLORREF g_crEditFg;                 // edit control foreground color
extern COLORREF g_crEditBg;                 // edit control background color

void ChangeLanguage(HWND, LANGID);
BOOL CALLBACK PrefDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

#endif // _MAIN_H_
