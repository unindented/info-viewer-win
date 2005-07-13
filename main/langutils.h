///////////////////////////////////////////////////////////////////////////////
///
/// iNFO - Utility to view NFO and DIZ files
/// Copyright (C) 2005 Daniel Pérez Álvarez <unindented@gmail.com>
///
///////////////////////////////////////////////////////////////////////////////

#ifndef	_LANGUTILS_H_
#define _LANGUTILS_H_

#define MAX_KEYLENGTH 80
#define MAX_LANGNAME  255

typedef struct
{
    int nCount;
    LANGID langID;
} LANGINFO;

typedef LANGINFO *PLANGINFO;

UINT PopulateLanguagesComboBox(HWND, LANGID, LPCTSTR);
HMODULE LoadSatelliteDLL(LANGID, LPCTSTR, LPCTSTR);
LANGID DetectLanguage();
BOOL IsHongKongVersion();
LANGID GetNTDLLNativeLangID();
BOOL CALLBACK EnumLangProc(HANDLE, LPCTSTR, LPCTSTR, LANGID, LPARAM);

#endif // _LANGUTILS_H_
