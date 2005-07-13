///////////////////////////////////////////////////////////////////////////////
///
/// iNFO - Utility to view NFO and DIZ files
/// Copyright (C) 2005 Daniel Pérez Álvarez <unindented@gmail.com>
///
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include <shlobj.h>
#include "regutils.h"

/// <summary>
/// Read a binary buffer from the registry.
/// </summary>
/// <param name="hKey">Registry key.</param>
/// <param name="lpszKeyName">Null-terminated string containing the name of the
/// value to be queried.</param>
/// <param name="lpRetData">Buffer that receives the data.</param>
/// <param name="cbLen">Number of bytes the buffer can hold.</param>
BOOL ReadRegistryBin(HKEY hKey, LPCTSTR lpszKeyName, LPVOID lpRetData, ULONG cbLen)
{
    ZeroMemory(lpRetData, cbLen);
    return (ERROR_SUCCESS == RegQueryValueEx(hKey, lpszKeyName, 0, 0, (LPBYTE)lpRetData, &cbLen));
}

/// <summary>
/// Read an integer value from the registry.
/// </summary>
/// <param name="hKey">Registry key.</param>
/// <param name="lpszKeyName">Null-terminated string containing the name of the
/// value to be queried.</param>
/// <param name="lpRetInt">Buffer that receives the integer.</param>
/// <param name="nDefault">Default value.</param>
BOOL ReadRegistryInt(HKEY hKey, LPCTSTR lpszKeyName, LPLONG lpRetInt, LONG nDefault)
{
    ULONG cbLen = sizeof (nDefault);
    *lpRetInt = nDefault;
    return (ERROR_SUCCESS == RegQueryValueEx(hKey, lpszKeyName, 0, 0, (LPBYTE)lpRetInt, &cbLen));
}

/// <summary>
/// Read a string buffer from the registry.
/// </summary>
/// <param name="hKey">Registry key.</param>
/// <param name="lpszKeyName">Null-terminated string containing the name of the
/// value to be queried.</param>
/// <param name="lpszRetStr">Buffer that receives the string.</param>
/// <param name="cchLen">Number of characters the buffer can hold.</param>
/// <param name="lpszDefault">Default value.</param>
BOOL ReadRegistryStr(HKEY hKey, LPCTSTR lpszKeyName, LPTSTR lpszRetStr, ULONG cchLen, LPCTSTR lpszDefault)
{
    ULONG cbLen = cchLen * sizeof (TCHAR);
    lstrcpyn(lpszRetStr, lpszDefault, cchLen);
    return (ERROR_SUCCESS == RegQueryValueEx(hKey, lpszKeyName, 0, 0, (LPBYTE)lpszRetStr, &cbLen));
}

/// <summary>
/// Write a binary buffer from the registry.
/// </summary>
/// <param name="hKey">Registry key.</param>
/// <param name="lpszKeyName">Null-terminated string containing the name of the
/// value to set.</param>
/// <param name="lpData">Data to write.</param>
/// <param name="cbLen">Size in bytes of data.</param>
BOOL WriteRegistryBin(HKEY hKey, LPCTSTR lpszKeyName, LPVOID lpData, ULONG cbLen)
{
    return (ERROR_SUCCESS == RegSetValueEx(hKey, lpszKeyName, 0, REG_BINARY, (LPBYTE)lpData, cbLen));
}

/// <summary>
/// Write an integer value from the registry.
/// </summary>
/// <param name="hKey">Registry key.</param>
/// <param name="lpszKeyName">Null-terminated string containing the name of the
/// value to set.</param>
/// <param name="nInteger">Integer to write.</param>
BOOL WriteRegistryInt(HKEY hKey, LPCTSTR lpszKeyName, LONG nInteger)
{
    return (ERROR_SUCCESS == RegSetValueEx(hKey, lpszKeyName, 0, REG_DWORD, (LPBYTE)&nInteger, sizeof (nInteger)));
}

/// <summary>
/// Write a string buffer from the registry.
/// </summary>
/// <param name="hKey">Registry key.</param>
/// <param name="lpszKeyName">Null-terminated string containing the name of the
/// value to set.</param>
/// <param name="lpszString">String to write.</param>
BOOL WriteRegistryStr(HKEY hKey, LPCTSTR lpszKeyName, LPCTSTR lpszString)
{
    return (ERROR_SUCCESS == RegSetValueEx(hKey, lpszKeyName, 0, REG_SZ, (LPBYTE)lpszString, (lstrlen(lpszString) + 1) * sizeof(TCHAR)));
}

/// <summary>
/// Checks if an extension is associated to an application.
/// </summary>
/// <param name="lpszExt">File name extension, including the period.</param>
/// <param name="lpszProgID">Program identifier for that extension.</param>
BOOL IsAssociatedExtToApp(LPCTSTR lpszExt, LPCTSTR lpszProgID)
{
    HKEY hKey;
    TCHAR szAppKeyValue[MAX_KEYLENGTH];
    ULONG cbKeyLen = MAX_KEYLENGTH;
    BOOL bResult = FALSE;

    // calculate length of key value
    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CLASSES_ROOT, lpszExt, 0, KEY_READ, &hKey))
    {
        // see if the value equals the name of the application
        if ((ERROR_SUCCESS == RegQueryValueEx(hKey, _T(""), 0, 0, (LPBYTE)szAppKeyValue, &cbKeyLen))
            && !lstrcmp(lpszProgID, szAppKeyValue))
        {
            bResult = TRUE;
        }

        RegCloseKey(hKey);
    }

    return bResult;
}

/// <summary>
/// Associates a file name extension to this application.
/// </summary>
/// <param name="lpszExt">File name extension, including the period.</param>
/// <param name="lpszProgID">Program identifier for that extension.</param>
/// <param name="lpszDesc">Description.</param>
/// <param name="lpszPerceivedType">Perceived type. If empty string, it will be deleted. If NULL, it will be left untouched.</param>
/// <param name="lpszContentType">Content type. If empty string, it will be deleted. If NULL, it will be left untouched.</param>
/// <example>This associates the extension ".abc" to our program "MyApp":
/// <code>
/// AssociateExtToApp(".abc", "MyApp", "ABC File", "text", "text/plain");
/// </code>
/// </example>
void AssociateExtToApp(LPCTSTR lpszExt, LPCTSTR lpszProgID, LPCTSTR lpszDesc, LPCTSTR lpszPerceivedType, LPCTSTR lpszContentType)
{
    HKEY hKey;
    HKEY hKeyTmp;
    TCHAR szFileName[MAX_PATH];
    TCHAR szCmd[MAX_PATH + 7];
    TCHAR szOldProgID[MAX_KEYLENGTH];
    BOOL bRefresh = FALSE;

    // if program identifier is not empty string, create root entry for this app
    if (lstrcmp(_T(""), lpszProgID))
    {
        if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CLASSES_ROOT, lpszProgID, 0, 0, 0, KEY_WRITE, 0, &hKey, 0))
        {
            // get filename
            GetModuleFileName(NULL, szFileName, MAX_PATH);

            // set the description for this app
            WriteRegistryStr(hKey, _T(""), lpszDesc);

            // set the default icon for this app
            if (ERROR_SUCCESS == RegCreateKeyEx(hKey, _T("DefaultIcon"), 0, 0, 0, KEY_WRITE, 0, &hKeyTmp, 0))
            {
                WriteRegistryStr(hKeyTmp, _T(""), szFileName);
                RegCloseKey(hKeyTmp);
            }

            // set the command line for this app
            if (ERROR_SUCCESS == RegCreateKeyEx(hKey, _T("shell\\open\\command"), 0, 0, 0, KEY_WRITE, 0, &hKeyTmp, 0))
            {
                wsprintf(szCmd, _T("\"%s\" \"%%1\""), szFileName);
                WriteRegistryStr(hKeyTmp, _T(""), szCmd);
                RegCloseKey(hKeyTmp);
            }

            RegCloseKey(hKey);
        }
    }

    // create root entry for the extension
    if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CLASSES_ROOT, lpszExt, 0, 0, 0, KEY_READ | KEY_WRITE, 0, &hKey, 0))
    {
        // check if the extension is already associated with our application
        if (!IsAssociatedExtToApp(lpszExt, lpszProgID))
        {
            // if program identifier is not an empty string, keep a backup of the previous association
            if (lstrcmp(_T(""), lpszProgID))
            {
                ReadRegistryStr(hKey, _T(""), szOldProgID, MAX_KEYLENGTH, _T(""));
                WriteRegistryStr(hKey, _T("Backup"), szOldProgID);
            }

            // only notify the shell if necessary
            bRefresh = TRUE;
        }

        // if program identifier is empty string, restore the backup
        if (!lstrcmp(_T(""), lpszProgID))
        {
            ReadRegistryStr(hKey, _T("Backup"), szOldProgID, MAX_KEYLENGTH, _T(""));
            WriteRegistryStr(hKey, _T(""), szOldProgID);
            RegDeleteValue(hKey, _T("Backup"));
        }
        // if it isn't, and also isn't NULL, associate extension with program
        else if (NULL != lpszProgID)
        {
            WriteRegistryStr(hKey, _T(""), lpszProgID);
        }

        // if perceived type is empty string, delete the value
        if (!lstrcmp(_T(""), lpszPerceivedType))
        {
            RegDeleteValue(hKey, _T("PerceivedType"));
        }
        // if it isn't, and also isn't NULL, set it to the value specified
        else if (NULL != lpszPerceivedType)
        {
            WriteRegistryStr(hKey, _T("PerceivedType"), lpszPerceivedType);
        }

        // if content type is empty string, delete the value
        if (!lstrcmp(_T(""), lpszContentType))
        {
            RegDeleteValue(hKey, _T("Content Type"));
        }
        // if it isn't, and also isn't NULL, set it to the value specified
        else if (NULL != lpszContentType)
        {
            WriteRegistryStr(hKey, _T("Content Type"), lpszContentType);
        }

        RegCloseKey(hKey);
    }

    // notify of the changes
    if (bRefresh)
    {
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
    }
}
