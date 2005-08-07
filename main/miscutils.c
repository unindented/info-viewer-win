///////////////////////////////////////////////////////////////////////////////
///
/// iNFO - Utility to view NFO and DIZ files
/// Copyright (C) 2005 Daniel Pérez Álvarez <unindented@gmail.com>
///
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include "miscutils.h"

// custom colors for the combo box
static COLORREF g_rgbCustomColors[] =
{
    RGB(0, 0, 0),       // black
    RGB(255, 255, 255), // white
    RGB(128, 0, 0),     // maroon
    RGB(0, 128, 0),     // dark green
    RGB(128, 128, 0),   // olive
    RGB(0, 0, 128),     // dark blue
    RGB(128, 0, 128),   // purple
    RGB(0, 128, 128),   // aquamarine
    RGB(196, 196, 196), // light gray
    RGB(128, 128, 128), // dark gray
    RGB(255, 0, 0),     // red
    RGB(0, 255, 0),     // green
    RGB(255, 255, 0),   // yellow
    RGB(0, 0, 255),     // blue
    RGB(255, 0, 255),   // magenta
    RGB(0, 255, 255)    // cyan
};
static TCHAR g_lpszCustomColorNames[(sizeof (g_rgbCustomColors) / sizeof (g_rgbCustomColors[0])) + 1][MAX_LOADSTRING];

static INT CALLBACK EnumFontNames(ENUMLOGFONTEX *, NEWTEXTMETRICEX *, DWORD, LPARAM);
static INT CALLBACK EnumFontSizes(ENUMLOGFONTEX *, NEWTEXTMETRICEX *, DWORD, LPARAM);

/// <summary>
/// Loads the names of the custom colors for the combo box.
/// </summary>
/// <param name="hSatDLL">Handle to the DLL with the resources.</param>
/// <param name="uColorNameID">First color name identifier. The method relies
/// on all color name identifiers being consecutive!</param>
VOID InitCustomColorNames(HMODULE hSatDLL, UINT uColorNameID)
{
    UINT nCount = (sizeof (g_rgbCustomColors) / sizeof (g_rgbCustomColors[0])) + 1;
    UINT i;

    for (i = 0; i < nCount; i++)
    {
        // load color string
        LoadString(hSatDLL, uColorNameID, g_lpszCustomColorNames[i], MAX_LOADSTRING);
        uColorNameID++;
    }
}

/// <summary>
/// Populates the list of font names in a combo box.
/// </summary>
/// <param name="hCombo">Handle to the combo box.</param>
/// <param name="lpszNameSelected">Font name to be selected.</param>
/// <returns>Total number of elements in the combo box.</returns>
UINT PopulateFontNameComboBox(HWND hCombo, LPCTSTR lpszNameSelected)
{
    TLOGFONT lf;
    HDC hDC = GetDC(hCombo);
    UINT nCount;
    INT nIndex;

    // all fonts with all character sets
    lf.lfCharSet = DEFAULT_CHARSET;
    lstrcpy(lf.lfFaceName, _T(""));
    lf.lfPitchAndFamily = 0;

    // reset contents
    SendMessage(hCombo, CB_RESETCONTENT, 0, 0);
    // enumerate font names
    EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)EnumFontNames, (LPARAM)hCombo, 0);

    ReleaseDC(hCombo, hDC);

    nCount = (INT)SendMessage(hCombo, CB_GETCOUNT, 0, 0);

    // set the current selection
    nIndex = (INT)SendMessage(hCombo, CB_FINDSTRING, 0, (LPARAM)lpszNameSelected);
    SendMessage(hCombo, CB_SETCURSEL, nIndex, 0);
    // set the current text
    SetWindowText(hCombo, lpszNameSelected);

    return nCount;
}

/// <summary>
/// Populates the list of font sizes in a combo box.
/// </summary>
/// <param name="hCombo">Handle to the combo box.</param>
/// <param name="lpszNameSelected">Font name for which to list the sizes.</param>
/// <param name="lSizeSelected">Font size to be selected.</param>
/// <returns>Total number of elements in the combo box.</returns>
UINT PopulateFontSizeComboBox(HWND hCombo, LPCTSTR lpszNameSelected, LONG nSizeSelected)
{
    TLOGFONT lf;
    HDC hDC = GetDC(hCombo);
    TCHAR szBuffer[12];
    UINT nCount;
    UINT i;
    UINT nNearest = 0;

    // only selected font
    lf.lfCharSet = DEFAULT_CHARSET;
    lstrcpy(lf.lfFaceName, lpszNameSelected);
    lf.lfPitchAndFamily = 0;

    // reset contents
    SendMessage(hCombo, CB_RESETCONTENT, 0, 0);
    // enumerate font sizes
    EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)EnumFontSizes, (LPARAM)hCombo, 0);

    ReleaseDC(hCombo, hDC);

    nCount = (INT)SendMessage(hCombo, CB_GETCOUNT, 0, 0);

    // set the current selection to the nearest size
    for (i = 0; i < nCount; i++)
    {
        if ((LONG)SendMessage(hCombo, CB_GETITEMDATA, i, 0) <= nSizeSelected)
        {
            nNearest = i;
        }
    }
    SendMessage(hCombo, CB_SETCURSEL, nNearest, 0);
    // set the current text
    _itot(nSizeSelected, szBuffer, 10);
    SetWindowText(hCombo, szBuffer);

    return nCount;
}

/// <summary>
/// Populates the list of font sizes in a combo box.
/// </summary>
/// <param name="hCombo">Handle to the combo box.</param>
/// <param name="rgbColorSelected">Color to be selected.</param>
/// <returns>Total number of elements in the combo box.</returns>
UINT PopulateColorComboBox(HWND hCombo, COLORREF rgbColorSelected)
{
    UINT nCount = sizeof (g_rgbCustomColors) / sizeof (g_rgbCustomColors[0]);
    UINT i;
    INT nIndex;
    BOOL bCustomSelected = TRUE;

    // reset contents
    SendMessage(hCombo, CB_RESETCONTENT, 0, 0);

    // add all the colors from the array
    for (i = 0; i < nCount; i++)
    {
        nIndex = (INT)SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)g_lpszCustomColorNames[i]);
        SendMessage(hCombo, CB_SETITEMDATA, nIndex, g_rgbCustomColors[i]);
        // set the current selection to the specified color
        if (g_rgbCustomColors[i] == rgbColorSelected)
        {
            SendMessage(hCombo, CB_SETCURSEL, i, 0);
            bCustomSelected = FALSE;
        }
    }

    // add additional item for custom color
    nIndex = (INT)SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)g_lpszCustomColorNames[nCount]);
    SendMessage(hCombo, CB_SETITEMDATA, nIndex, rgbColorSelected);
    if (bCustomSelected)
    {
        // if no other item was selected, select this one
        SendMessage(hCombo, CB_SETCURSEL, nIndex, 0);
    }

    return nCount;
}

/// <summary>
/// Procedure to enumerate font names.
/// </summary>
static INT CALLBACK EnumFontNames(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, DWORD dwFontType, LPARAM lParam)
{
    HWND hCombo = (HWND)lParam;
    LPTSTR lpszName = lpelfe->elfLogFont.lfFaceName;
    INT nIndex;
    BOOL bFixed;
    BOOL bTrueType;

    // make sure font name doesn't already exist in our list
    if (CB_ERR == SendMessage(hCombo, CB_FINDSTRING, 0, (LPARAM)lpszName))
    {
        // record the font's attributes (fixed width and TrueType)
        bFixed = (FIXED_PITCH & lpelfe->elfLogFont.lfPitchAndFamily) ? TRUE : FALSE;
        bTrueType = (OUT_STROKE_PRECIS == lpelfe->elfLogFont.lfOutPrecision) ? TRUE : FALSE;

        // add the font
        nIndex = (INT)SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)lpszName);
        SendMessage(hCombo, CB_SETITEMDATA, nIndex, MAKELPARAM(bFixed, bTrueType));
    }

    // continue enumeration
    return 1;
}

/// <summary>
/// Procedure to enumerate font sizes.
/// </summary>
static INT CALLBACK EnumFontSizes(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, DWORD dwFontType, LPARAM lParam)
{
    static INT nTTSizes[] = { 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72 };

    HWND hCombo = (HWND)lParam;
    TCHAR szBuffer[12];
    UINT nCount = sizeof (nTTSizes) / sizeof (nTTSizes[0]);
    UINT i;
    INT nIndex;
    INT nSize;
    BOOL bTrueType = (OUT_STROKE_PRECIS == lpelfe->elfLogFont.lfOutPrecision) ? TRUE : FALSE;

    if (bTrueType)
    {
        // if it's TrueType, add all font sizes from the array
        for (i = 0; i < nCount; i++)
        {
            wsprintf(szBuffer, _T("%d"), nTTSizes[i]);
            nIndex = (INT)SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)szBuffer);
            SendMessage(hCombo, CB_SETITEMDATA, nIndex, nTTSizes[i]);
        }

        // stop enumeration
        return 0;
    }
    else
    {
        // if it isn't, add font sizes one by one
        nSize = LogicalToPoints(lpntme->ntmTm.tmHeight);
        wsprintf(szBuffer, _T("%d"), nSize);

        // make sure font size doesn't already exist in our list
        if (CB_ERR == SendMessage(hCombo, CB_FINDSTRING, 0, (LPARAM)szBuffer))
        {
            nCount = (INT)SendMessage(hCombo, CB_GETCOUNT, 0, 0);
            // find the position to insert the new size
            for (i = 0; i < nCount; i++)
            {
                if (nSize < (INT)SendMessage(hCombo, CB_GETITEMDATA, i, 0))
                {
                    break;
                }
            }
            // insert it
            nIndex = (INT)SendMessage(hCombo, CB_INSERTSTRING, i, (LPARAM)szBuffer);
            SendMessage(hCombo, CB_SETITEMDATA, nIndex, nSize);
        }

        // continue enumeration
        return 1;
    }
}

/// <summary>
/// Adjust the height of the items in a combo box.
/// </summary>
/// <param name="hCombo">Handle to the combo box.</param>
/// <param name="nMinHeight">Minimum height of an item.</param>
VOID SetComboItemHeight(HWND hCombo, INT nMinHeight)
{
    TEXTMETRIC tm;
    HDC hDC = GetDC(hCombo);

    GetTextMetrics(hDC, &tm);
    nMinHeight = max(tm.tmHeight, nMinHeight);

    SendMessage(hCombo, CB_SETITEMHEIGHT, -1, nMinHeight);
    SendMessage(hCombo, CB_SETITEMHEIGHT, 0, nMinHeight);
}

/// <summary>
/// Convert size from logical units to points.
/// </summary>
/// <param name="nLogicalSize">Size in logical units.</param>
/// <returns>Size in points.</returns>
INT LogicalToPoints(INT nLogicalSize)
{
    HDC hDC = GetDC(NULL);
    INT nPointSize = MulDiv(nLogicalSize, 72, GetDeviceCaps(hDC, LOGPIXELSY));

    ReleaseDC(NULL, hDC);

    return nPointSize;
}

/// <summary>
/// Convert size from points to logical units.
/// </summary>
/// <param name="nPointSize">Size in points.</param>
/// <returns>Size in logical units.</returns>
INT PointsToLogical(INT nPointSize)
{
    HDC hDC = GetDC(NULL);
    INT nLogicalSize = -MulDiv(nPointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);

    ReleaseDC(NULL, hDC);

    return nLogicalSize;
}

/// <summary>
/// Shows an Open common dialog box that lets the user choose a file.
/// </summary>
/// <param name="hWnd">Handle to the parent window.</param>
/// <param name="lpszFilter">Filter pattern.</param>
/// <param name="lpszFileName">Handle to the parent window.</param>
/// <returns>If the dialog was closed pressing OK, the return value is nonzero.
/// Else, the return value is zero.</returns>
BOOL FileChooser(HWND hWnd, LPCTSTR lpszFilter, LPTSTR lpszFileName)
{
    OPENFILENAME ofn;

    ZeroMemory(&ofn, sizeof (OPENFILENAME));
    ofn.lStructSize = sizeof (OPENFILENAME);
    ofn.hwndOwner   = hWnd;
    ofn.lpstrFilter = lpszFilter;
    ofn.lpstrFile   = lpszFileName;
    ofn.nMaxFile    = MAX_PATH;
    ofn.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    // show open common dialog
    if (GetOpenFileName(&ofn))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/// <summary>
/// Shows a Color common dialog box that lets the user choose a color.
/// </summary>
/// <param name="hWnd">Handle to the parent window.</param>
/// <param name="rgbColor">Selected color.</param>
/// <returns>If the dialog was closed pressing OK, the return value is nonzero.
/// Else, the return value is zero.</returns>
BOOL ColorChooser(HWND hWnd, COLORREF *rgbColor)
{
    CHOOSECOLOR cc;

    ZeroMemory(&cc, sizeof (CHOOSECOLOR));
    cc.lStructSize  = sizeof (CHOOSECOLOR);
    cc.hwndOwner    = hWnd;
    cc.lpCustColors = g_rgbCustomColors;
    cc.rgbResult    = *rgbColor;
    cc.Flags        = CC_FULLOPEN | CC_RGBINIT;

    if (ChooseColor(&cc))
    {
        *rgbColor = cc.rgbResult;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/// <summary>
/// Displays a message box with an error.
/// </summary>
/// <param name="hWnd">Handle to the parent window.</param>
/// <param name="hSatDLL">Handle to the DLL with the resources.</param>
/// <param name="uErrTextID">Error text identifier.</param>
/// <param name="uErrTitleID">Error title identifier.</param>
VOID DisplayError(HWND hWnd, HMODULE hSatDLL, UINT uErrTextID, UINT uErrTitleID)
{
    static TCHAR szErrorTitle[MAX_LOADSTRING];
    static TCHAR szErrorText[MAX_LOADSTRING];

    // load error strings
    LoadString(hSatDLL, uErrTextID, szErrorText, MAX_LOADSTRING);
    LoadString(hSatDLL, uErrTitleID, szErrorTitle, MAX_LOADSTRING);
    // show error
    MessageBox(hWnd, szErrorText, szErrorTitle,
        MB_OK | MB_ICONERROR);
}

/// <summary>
/// Gets the specified argument from the command line.
/// </summary>
/// <param name="lpszArg">Buffer which will hold the null-terminated argument.
/// </param>
/// <param name="lpszCmdLine">Command line, probably obtained with
/// GetCommandLine().</param>
/// <param name="nIndex">Index of the argument to get. Index 0 corresponds to
/// the program name, in case GetCommandLine() was used.</param>
/// <example>This gets the first argument passed to our program:
/// <code>
/// TCHAR szFileName[MAX_PATH];
/// LPCTSTR lpszCmdLine = GetCommandLine();
/// GetCommandLineArgument(szFileName, lpszCmdLine, 1);
/// </code>
/// </example>
VOID GetCommandLineArgument(LPCTSTR lpszCmdLine, UINT nIndex, LPTSTR lpszArg)
{
    UINT i;
    LPTSTR lpszTemp = lpszArg;

    // skip all tokens until the one specified
    for (i = 0; i < nIndex; i++)
    {
        if (_T('"') == *lpszCmdLine)
        {
            lpszCmdLine++;
            // quoted program name, skip until another quote or a null
            // is encountered
            while ((_T('"') != *lpszCmdLine) && (_T('\0') != *lpszCmdLine))
            {
                lpszCmdLine++;
            }
            // if we stopped on a quote, skip over it
            if (_T('"') == *lpszCmdLine)
            {
                lpszCmdLine++;
            }
        }
        else
        {
            // program name wasn't quoted, skip until a white space or a null
            // is encountered
            while (!_istspace(*lpszCmdLine) && (_T('\0') != *lpszCmdLine))
            {
                lpszCmdLine++;
            }
        }

        // skip past any white space preceeding the second token.
        while (_istspace(*lpszCmdLine) && (_T('\0') != *lpszCmdLine))
        {
            lpszCmdLine++;
        }
    }

    // we got to the token we wanted
    if (_T('"') == *lpszCmdLine)
    {
        lpszCmdLine++;
        // copy to the destination string until a quote or a null is
        // encountered
        while ((_T('"') != *lpszCmdLine) && (_T('\0') != *lpszCmdLine))
        {
            *lpszTemp++ = *lpszCmdLine++;
        }
        *lpszTemp = _T('\0');
    }
    else
    {
        // copy to the destination string until a white space or a null is
        // encountered
        while (!_istspace(*lpszCmdLine) && (_T('\0') != *lpszCmdLine))
        {
            *lpszTemp++ = *lpszCmdLine++;
        }
        *lpszTemp = _T('\0');
    }
}

/// <summary>
/// Replaces UNIX (LF) and MAC (CR) linebreaks in the source string with
/// DOS (CR+LF) linebreaks, and saves the result to the destination string.
/// </summary>
/// <param name="lpszDst">Buffer which will hold the resulting null-terminated
/// string. It should be twice the size of the original string, in case all
/// characters are UNIX linebreaks for example.</param>
/// <param name="lpszSrc">Buffer which holds the null-terminated string to fix.</param>
VOID AdjustLineBreaks(LPTSTR lpszDst, LPCTSTR lpszSrc)
{
    UINT nSrc;
    UINT nDst;
    TCHAR chPrev = _T('\0');

    for (nSrc = 0, nDst = 0; _T('\0') != lpszSrc[nSrc]; nSrc++)
    {
        // LF -> CR+LF
        if (_T('\n') == lpszSrc[nSrc] && _T('\r') != chPrev)
        {
            lpszDst[nDst++] = _T('\r');
        }
        // CR -> CR+LF
        else if (_T('\n') != lpszSrc[nSrc] && _T('\r') == chPrev)
        {
            lpszDst[nDst++] = _T('\n');
        }
        lpszDst[nDst++] = lpszSrc[nSrc];
        chPrev = lpszSrc[nSrc];
    }
}
