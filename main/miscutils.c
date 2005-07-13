///////////////////////////////////////////////////////////////////////////////
///
/// iNFO - Utility to view NFO and DIZ files
/// Copyright (C) 2005 Daniel Pérez Álvarez <unindented@gmail.com>
///
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include "miscutils.h"

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
void GetCommandLineArgument(LPTSTR lpszArg, LPCTSTR lpszCmdLine, UINT nIndex)
{
    UINT i;

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
        LPTSTR lpszTemp = lpszArg;

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
        LPTSTR lpszTemp = lpszArg;

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
/// <param name="lpszSrc">Buffer which holds the null-terminated string to fix.
/// </param>
void AdjustLineBreaks(LPTSTR lpszDst, LPCTSTR lpszSrc)
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
