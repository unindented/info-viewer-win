///////////////////////////////////////////////////////////////////////////////
///
/// iNFO - Utility to view NFO and DIZ files
/// Copyright (C) 2005 Daniel Pérez Álvarez <unindented@gmail.com>
///
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include "langutils.h"

static BOOL IsHongKongVersion();
static LANGID GetNTDLLNativeLangID();
static BOOL CALLBACK EnumLangProc(HANDLE, LPCTSTR, LPCTSTR, LANGID, LPARAM);

/// <summary>
/// Populates the list of languages in a combo box according to the
/// subdirectories named with decimal LCID.
/// </summary>
/// <param name="hCombo">Handle to the combo box.</param>
/// <param name="langSelected">Language to be selected.</param>
/// <param name="lpszBaseDir">Directory with all the language subdirectories.</param>
/// <returns>Total number of elements in the combo box.</returns>
UINT PopulateLanguageComboBox(HWND hCombo, LANGID langSelected, LPCTSTR lpszBaseDir)
{
    TCHAR szSearchPath[MAX_PATH];
    HANDLE hDir;
    WIN32_FIND_DATA findFileData;
    LANGID langID;
    TCHAR szLangName[MAX_LANGNAME];
    UINT nCount;
    UINT i;
    INT nIndex;

    // check string lengths
    if (lstrlen(lpszBaseDir) + 5 <= MAX_PATH)
    {
        // loop through all directories and populate the list
        wsprintf(szSearchPath, _T("%s\\*.*"), lpszBaseDir);
        hDir = FindFirstFile(szSearchPath, &findFileData);
        if (INVALID_HANDLE_VALUE != hDir)
        {
            // reset contents
            SendMessage(hCombo, CB_RESETCONTENT, 0, 0);
            do
            {
                if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    langID = _ttoi(findFileData.cFileName);
                    if (0 != langID)
                    {
                        if (GetLocaleInfo(langID, LOCALE_SENGLANGUAGE, szLangName, MAX_LANGNAME))
                        {
                            nIndex = (INT)SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)szLangName);
                            SendMessage(hCombo, CB_SETITEMDATA, nIndex, langID);
                        }
                    }
                }
            }
            while (FindNextFile(hDir, &findFileData));
        }
        FindClose(hDir);
    }

    nCount = (INT)SendMessage(hCombo, CB_GETCOUNT, 0, 0);

    // if there are no languages, disable the combo box
    if (nCount <= 0)
    {
        // if it has the focus, pass it to the next component
        if (GetFocus() == hCombo)
        {
            SendMessage(GetParent(hCombo), WM_NEXTDLGCTL, 0, FALSE);
        }
        EnableWindow(hCombo, FALSE);
    }
    else
    {
        // to set the current selection we have to loop through the combo box
        // because the addition sorted the combo box alphabetically
        for (i = 0; i < nCount; i++)
        {
            langID = (LANGID)SendMessage(hCombo, CB_GETITEMDATA, i, 0);
            if (langID == langSelected)
            {
                SendMessage(hCombo, CB_SETCURSEL, i, 0);
                break;
            }
        }
        // if no language was selected, try the primary language ID
        if (langSelected != langID)
        {
            langSelected = PRIMARYLANGID(langSelected);
            for (i = 0; i < nCount; i++)
            {
                langID = (LANGID)SendMessage(hCombo, CB_GETITEMDATA, i, 0);
                if (langID == langSelected)
                {
                    SendMessage(hCombo, CB_SETCURSEL, i, 0);
                    break;
                }
            }
        }
    }

    return nCount;
}

/// <summary>
/// Loads the satellite DLL for the specified language from subdirectories
/// named with decimal LCID.
/// </summary>
/// <param name="langDesired">Desired language.</param>
/// <param name="lpszBaseDir">Directory with all the language subdirectories.</param>
/// <param name="lpszSatName">Name of the satellite DLL.</param>
/// <returns>Handle to the satellite DLL.</returns>
HMODULE LoadSatelliteDLL(LANGID langDesired, LPCTSTR lpszBaseDir, LPCTSTR lpszSatName)
{
    TCHAR szSatellitePath[MAX_PATH];
    TCHAR szBuffer[12];
    HMODULE hMod;

    // check string lengths
    if (lstrlen(lpszBaseDir) + lstrlen(lpszSatName) + 15 <= MAX_PATH)
    {
        // try to load the library with the fully specified language
        _itot(langDesired, szBuffer, 10);
        wsprintf(szSatellitePath, _T("%s\\%s\\%s"), lpszBaseDir, szBuffer, lpszSatName);
        hMod = LoadLibrary(szSatellitePath);
        if (NULL != hMod)
        {
            return hMod;
        }
        else
        {
            // no luck, try the primary language ID
            langDesired = PRIMARYLANGID(langDesired);
            _itot(langDesired, szBuffer, 10);
            wsprintf(szSatellitePath, _T("%s\\%s\\%s"), lpszBaseDir, szBuffer, lpszSatName);
            hMod = LoadLibrary(szSatellitePath);
            if (NULL != hMod)
            {
                return hMod;
            }
        }
    }

    return NULL;
}

/// <summary>
/// This function detects a correct initial UI language for all
/// platforms (Win9x, ME, NT4, Windows 2000, Windows XP, Windows 2003).
/// </summary>
/// <returns>Language identifier.</returns>
LANGID DetectLanguage()
{
    OSVERSIONINFO versionInfo;
    HMODULE hMod;
    typedef LANGID (WINAPI *GETUSERDEFAULTUILANGUAGE)();
    GETUSERDEFAULTUILANGUAGE pGetUserDefaultUILanguage;
    LANGID langID = 0;
    HKEY hKey;
    DWORD dwType;
    TCHAR szLangKeyValue[MAX_KEYLENGTH];
    ULONG cbKeyLen = MAX_KEYLENGTH;

    versionInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
    if (!GetVersionEx(&versionInfo))
    {
        return 0;
    }

    switch (versionInfo.dwPlatformId)
    {
    case VER_PLATFORM_WIN32_NT:
        // Windows NT, Windows 2000 or higher
        if (versionInfo.dwMajorVersion >= 5)
        {
            // GetUserDefaultUILanguage()
            hMod = LoadLibrary(_T("kernel32.dll"));
            if (NULL != hMod)
            {
                pGetUserDefaultUILanguage = (GETUSERDEFAULTUILANGUAGE)GetProcAddress(hMod, "GetUserDefaultUILanguage");
                if (NULL != pGetUserDefaultUILanguage)
                {
                    langID = pGetUserDefaultUILanguage();
                }
                FreeLibrary(hMod);
            }
        }
        else
        {
            langID = GetNTDLLNativeLangID();
            if (1033 == langID)
            {
                if (IsHongKongVersion())
                {
                    langID = 3076;
                }
            }
        }
        break;

    case VER_PLATFORM_WIN32_WINDOWS:
        // Windows 95, Windows 98 or Windows ME
        if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, _T("Default\\Control Panel\\Desktop\\ResourceLocale"), 0, KEY_QUERY_VALUE, &hKey))
        {
            if ((ERROR_SUCCESS == RegQueryValueEx(hKey, NULL, NULL, &dwType, NULL, NULL))
                && (REG_SZ == dwType))
            {
                if (ERROR_SUCCESS == RegQueryValueEx(hKey, NULL, NULL, &dwType, (LPBYTE)szLangKeyValue, &cbKeyLen))
                {
                    langID = _ttoi(szLangKeyValue);
                }
            }

            RegCloseKey(hKey);
        }
        break;
    }

    if (0 == langID)
    {
        langID = GetUserDefaultLangID();
    }

    return langID;
}

/// <summary>
/// Checks if NT4 system is Hong Kong SAR version.
/// </summary>
/// <returns>True if it's the Hong Kong SAR version. False otherwise.</returns>
static BOOL IsHongKongVersion()
{
    HMODULE hMod;
    typedef BOOL (WINAPI *IMMRELEASECONTEXT)(HWND, HIMC);
    IMMRELEASECONTEXT pImmReleaseContext;
    BOOL bResult = FALSE;

    // ImmReleaseContext()
    hMod = LoadLibrary(_T("imm32.dll"));
    if (NULL != hMod)
    {
        pImmReleaseContext = (IMMRELEASECONTEXT)GetProcAddress(hMod, "ImmReleaseContext");
        if (NULL != pImmReleaseContext)
        {
            bResult = pImmReleaseContext(NULL, NULL);
        }
        FreeLibrary(hMod);
    }

    return bResult;
}

/// <summary>
/// Detects the language of ntdll.dll with some specific processing for the
/// Hong Kong SAR version.
/// </summary>
/// <returns>Language identifier.</returns>
static LANGID GetNTDLLNativeLangID()
{
    LANGINFO langInfo;
    HMODULE hMod;
    BOOL bResult;
    LPCTSTR lpszType = (LPCTSTR)((LPVOID)((WORD)16));
    LPCTSTR lpszName = (LPCTSTR)1;

    ZeroMemory(&langInfo, sizeof (langInfo));

    hMod = GetModuleHandle(_T("ntdll.dll"));
    if (NULL == hMod)
    {
        return 0;
    }

    bResult = EnumResourceLanguages(hMod, lpszType, lpszName,
        (ENUMRESLANGPROC)EnumLangProc, (LONG_PTR) &langInfo);

    if (!bResult || (langInfo.nCount > 2) || (langInfo.nCount < 1))
    {
        return 0;
    }

    return (langInfo.langID);
}

/// <summary>
/// Procedure to enumerate languages.
/// </summary>
static BOOL CALLBACK EnumLangProc(HANDLE hModule, LPCTSTR lpszType, LPCTSTR lpszName, LANGID langID, LPARAM lParam)
{
    PLANGINFO pLangInfo;

    pLangInfo = (PLANGINFO)lParam;
    pLangInfo->nCount++;
    pLangInfo->langID = langID;

    // continue enumeration
    return TRUE;
}
