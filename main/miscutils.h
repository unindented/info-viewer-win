///////////////////////////////////////////////////////////////////////////////
///
/// iNFO - Utility to view NFO and DIZ files
/// Copyright (C) 2005 Daniel Pérez Álvarez <unindented@gmail.com>
///
///////////////////////////////////////////////////////////////////////////////

#ifndef	_MISCUTILS_H_
#define _MISCUTILS_H_

#ifdef UNICODE
typedef LOGFONTW TLOGFONT;
#else
typedef LOGFONT TLOGFONT;
#endif // UNICODE

#ifndef ODS_NOFOCUSRECT
#define ODS_NOFOCUSRECT 0x0200
#endif // ODS_NOFOCUSRECT

#ifndef MAX_LOADSTRING
#define MAX_LOADSTRING 100
#endif // MAX_LOADSTRING

#define REALIZE_SYSCOLOR(color) (COLORREF)((color & 0x80000000) ? GetSysColor((int)(color & ~0x80000000)) : color)

VOID InitCustomColorNames(HMODULE, UINT);
UINT PopulateFontNameComboBox(HWND, LPCTSTR);
UINT PopulateFontSizeComboBox(HWND, LPCTSTR, LONG);
UINT PopulateColorComboBox(HWND, COLORREF);
VOID SetComboItemHeight(HWND, INT);
INT LogicalToPoints(INT);
INT PointsToLogical(INT);
BOOL FileChooser(HWND, LPCTSTR, LPTSTR);
BOOL ColorChooser(HWND, COLORREF *);
VOID DisplayError(HWND, HMODULE, UINT, UINT);
VOID GetCommandLineArgument(LPCTSTR, UINT, LPTSTR);
VOID AdjustLineBreaks(LPTSTR, LPCTSTR);

#endif // _MISCUTILS_H_
