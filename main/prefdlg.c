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
#include "resource.h"

/// <summary>
/// Preferences dialog procedure.
/// </summary>
BOOL CALLBACK PrefDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
        {
            PopulateLanguagesComboBox(GetDlgItem(hDlg, IDC_COMBO_LANG), g_langUI, g_szLangPath);

            // check if NFO files are associated to the app
            SendDlgItemMessage(hDlg, IDC_CHECK_NFO, BM_SETCHECK,
                IsAssociatedExtToApp(_T(".nfo"), g_szNFOName), 0);
            // check if DIZ files are associated to the app
            SendDlgItemMessage(hDlg, IDC_CHECK_DIZ, BM_SETCHECK,
                IsAssociatedExtToApp(_T(".diz"), g_szDIZName), 0);
        }
        return TRUE;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDOK:
            {
                UINT nIndex;
                LANGID langID;
                BOOL bChecked;

                nIndex = (UINT)SendDlgItemMessage(hDlg, IDC_COMBO_LANG, CB_GETCURSEL, 0, 0);
                if (nIndex != CB_ERR)
                {
                    langID = (LANGID)SendDlgItemMessage(hDlg, IDC_COMBO_LANG, CB_GETITEMDATA, nIndex, 0);
                    if ((langID != CB_ERR) && (langID != g_langUI))
                    {
                        ChangeLanguage(GetParent(hDlg), langID);
                    }
                }

                // check if NFO files have to be associated to the app or not
                bChecked = (BOOL)SendDlgItemMessage(hDlg, IDC_CHECK_NFO, BM_GETCHECK, 0, 0) ;
                AssociateExtToApp(_T(".nfo"), bChecked ? g_szNFOName : _T(""), g_szNFODesc, _T("text"), NULL);
                // check if DIZ files have to be associated to the app or not
                bChecked = (BOOL)SendDlgItemMessage(hDlg, IDC_CHECK_DIZ, BM_GETCHECK, 0, 0) ;
                AssociateExtToApp(_T(".diz"), bChecked ? g_szDIZName : _T(""), g_szDIZDesc, _T("text"), NULL);

                EndDialog(hDlg, IDOK);
            }
            break;

        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            break;
        }
        break;

    default:
        return FALSE;
    }
    return TRUE;
}
