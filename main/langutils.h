///////////////////////////////////////////////////////////////////////////////
///
/// iNFO - Utility to view NFO and DIZ files
/// Copyright (C) 2005 Daniel Pérez Álvarez <unindented@gmail.com>
///
///////////////////////////////////////////////////////////////////////////////

#ifndef	_LANGUTILS_H_
#define _LANGUTILS_H_

#ifndef MAX_KEYLENGTH
#define MAX_KEYLENGTH 80
#endif // MAX_KEYLENGTH

#ifndef MAX_LANGNAME
#define MAX_LANGNAME 255
#endif // MAX_LANGNAME

typedef struct
{
    INT nCount;
    LANGID langID;
} LANGINFO;
typedef LANGINFO *PLANGINFO;

UINT PopulateLanguageComboBox(HWND, LANGID, LPCTSTR);
HMODULE LoadSatelliteDLL(LANGID, LPCTSTR, LPCTSTR);
LANGID DetectLanguage();

#endif // _LANGUTILS_H_
