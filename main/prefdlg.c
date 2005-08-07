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
#include "miscutils.h"
#include "resource.h"

static VOID ColorComboBox_DrawItem(LPDRAWITEMSTRUCT, BOOL);
static VOID DefaultColors_DrawItem(LPDRAWITEMSTRUCT);
static VOID PaintFrameRect(HDC, LPRECT, COLORREF, COLORREF);

/// <summary>
/// Preferences dialog procedure.
/// </summary>
BOOL CALLBACK PrefDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
        // populate languages
        PopulateLanguageComboBox(GetDlgItem(hDlg, IDC_COMBO_LANG), g_langUI, g_szLangPath);

        // check if NFO files are associated to the app
        SendDlgItemMessage(hDlg, IDC_CHECK_NFO, BM_SETCHECK,
            IsAssociatedExtToApp(_T(".nfo"), g_szNFOName), 0);
        // check if DIZ files are associated to the app
        SendDlgItemMessage(hDlg, IDC_CHECK_DIZ, BM_SETCHECK,
            IsAssociatedExtToApp(_T(".diz"), g_szDIZName), 0);

        // populate font names
        PopulateFontNameComboBox(GetDlgItem(hDlg, IDC_COMBO_FONTNAME), g_szFontName);
        // populate font sizes
        PopulateFontSizeComboBox(GetDlgItem(hDlg, IDC_COMBO_FONTSIZE), g_szFontName, g_nFontSize);

        // init custom color names
        InitCustomColorNames(g_hSatDLL, IDS_COLOR_BLACK);
        // populate foreground color combo box
        SetComboItemHeight(GetDlgItem(hDlg, IDC_COMBO_COLORFG), 14);
        PopulateColorComboBox(GetDlgItem(hDlg, IDC_COMBO_COLORFG), g_rgbEditFg);
        // populate background color combo box
        SetComboItemHeight(GetDlgItem(hDlg, IDC_COMBO_COLORBG), 14);
        PopulateColorComboBox(GetDlgItem(hDlg, IDC_COMBO_COLORBG), g_rgbEditBg);
        break;

    case WM_DRAWITEM:
        switch (wParam)
        {
        case IDC_COMBO_COLORFG:
        case IDC_COMBO_COLORBG:
            // draw item
            ColorComboBox_DrawItem((LPDRAWITEMSTRUCT)lParam, TRUE);
            return TRUE;
        }
        return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_COMBO_FONTSIZE:
            switch (HIWORD(wParam))
            {
            case CBN_DROPDOWN:
                {
                    HWND hFontNameCombo = GetDlgItem(hDlg, IDC_COMBO_FONTNAME);
                    HWND hFontSizeCombo = (HWND)lParam;
                    TCHAR szFontName[LF_FACESIZE];

                    // populate the sizes for the current font name
                    GetWindowText(hFontNameCombo, szFontName, LF_FACESIZE);
                    PopulateFontSizeComboBox(hFontSizeCombo, szFontName, g_nFontSize);
                }
                break;
            }
            break;
        case IDC_COMBO_COLORFG:
        case IDC_COMBO_COLORBG:
            switch (HIWORD(wParam))
            {
            case CBN_SELCHANGE:
                {
                    HWND hCombo = (HWND)lParam;
                    INT nIndex;
                    COLORREF rgbColor;

                    // see if the selected element is the custom item (the last one)
                    nIndex = (INT)SendMessage(hCombo, CB_GETCOUNT, 0, 0) - 1;
                    if ((INT)SendMessage(hCombo, CB_GETCURSEL, 0, 0) == nIndex)
                    {
                        // display a color chooser
                        rgbColor = (COLORREF)SendMessage(hCombo, CB_GETITEMDATA, nIndex, 0);
                        ColorChooser(hDlg, &rgbColor);
                        // change the color of the custom item
                        SendMessage(hCombo, CB_SETITEMDATA, nIndex, rgbColor);
                        // reselect the custom item so that it gets redrawn
                        SendMessage(hCombo, CB_SETCURSEL, -1, 0);
                        SendMessage(hCombo, CB_SETCURSEL, nIndex, 0);
                    }
                }
                break;
            }
            break;

        case IDOK:
            {
                INT nIndex;
                LANGID langID;
                BOOL bChecked;
                TCHAR szFontName[LF_FACESIZE];
                LONG nFontSize;
                COLORREF rgbEditFg;
                COLORREF rgbEditBg;

                // change the language if needed
                nIndex = (INT)SendDlgItemMessage(hDlg, IDC_COMBO_LANG, CB_GETCURSEL, 0, 0);
                if (CB_ERR != nIndex)
                {
                    langID = (LANGID)SendDlgItemMessage(hDlg, IDC_COMBO_LANG, CB_GETITEMDATA, nIndex, 0);
                    if ((CB_ERR != langID) && (g_langUI != langID))
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

                // get the font name
                GetDlgItemText(hDlg, IDC_COMBO_FONTNAME, szFontName, LF_FACESIZE);
                // get the font size
                nFontSize = GetDlgItemInt(hDlg, IDC_COMBO_FONTSIZE, NULL, FALSE);
                // change the font if needed
                if (lstrcmp(g_szFontName, szFontName) || (g_nFontSize != nFontSize))
                {
                    ChangeFont(GetParent(hDlg), szFontName, nFontSize);
                }

                // get the foreground color
                nIndex = (INT)SendDlgItemMessage(hDlg, IDC_COMBO_COLORFG, CB_GETCURSEL, 0, 0);
                if (CB_ERR != nIndex)
                {
                    rgbEditFg = (COLORREF)SendDlgItemMessage(hDlg, IDC_COMBO_COLORFG, CB_GETITEMDATA, nIndex, 0);
                }
                // get the background color
                nIndex = (INT)SendDlgItemMessage(hDlg, IDC_COMBO_COLORBG, CB_GETCURSEL, 0, 0);
                if (CB_ERR != nIndex)
                {
                    rgbEditBg = (COLORREF)SendDlgItemMessage(hDlg, IDC_COMBO_COLORBG, CB_GETITEMDATA, nIndex, 0);
                }
                // change the color if needed
                if (CB_ERR != rgbEditFg && CB_ERR != rgbEditBg &&
                    ((g_rgbEditFg != rgbEditFg) || (g_rgbEditBg != rgbEditBg)))
                {
                    ChangeColor(GetParent(hDlg), rgbEditFg, rgbEditBg);
                }

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

/// <summary>
/// Color combo box owner-draw. Combo box must have the CBS_HASSTRINGS style set!
/// </summary>
/// <param name="dis">Pointer to the DRAWITEMSTRUCT.</param>
/// <param name="bSelectImage">True if you want the image to be included in the
/// selection. False otherwise.</param>
static VOID ColorComboBox_DrawItem(LPDRAWITEMSTRUCT dis, BOOL bSelectImage)
{
    RECT rect = dis->rcItem;
    INT nBoxsize = (dis->rcItem.bottom - dis->rcItem.top) - 4;
    INT x;
    INT y;
    TCHAR szBuffer[MAX_LOADSTRING];
    TEXTMETRIC tm;

    // add the image to the selection or not
    if (!bSelectImage)
    {
        rect.left += nBoxsize + 4;
    }

    if (dis->itemAction & ODA_FOCUS && !(dis->itemState & ODS_NOFOCUSRECT))
    {
        DrawFocusRect(dis->hDC, &rect);
    }
    else
    {
        // get the item text
        if (-1 == dis->itemID)
        {
            SendMessage(dis->hwndItem, WM_GETTEXT, 0, (LPARAM)szBuffer);
        }
        else
        {
            SendMessage(dis->hwndItem, CB_GETLBTEXT, dis->itemID, (LPARAM)szBuffer);
        }

        // set text colour and background based on current state
        DefaultColors_DrawItem(dis);

        // draw the text (centered vertically)
        GetTextMetrics(dis->hDC, &tm);
        x = dis->rcItem.left + nBoxsize + 4 + 4;
        y = dis->rcItem.top  + (dis->rcItem.bottom - dis->rcItem.top - tm.tmHeight) / 2;

        ExtTextOut(dis->hDC, x, y, ETO_CLIPPED | ETO_OPAQUE,
            &rect, szBuffer, lstrlen(szBuffer), 0);

        if ((dis->itemState & ODS_FOCUS) && !(dis->itemState & ODS_NOFOCUSRECT))
        {
            DrawFocusRect(dis->hDC, &rect);
        }

        // paint the color rectangle
        rect = dis->rcItem;
        InflateRect(&rect, -2, -2);
        rect.right = rect.left + nBoxsize;
        // check if it is disabled
        if (dis->itemState & ODS_DISABLED)
        {
            PaintFrameRect(dis->hDC, &rect, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DFACE));
        }
        else
        {
            PaintFrameRect(dis->hDC, &rect, RGB(0, 0, 0), REALIZE_SYSCOLOR(dis->itemData));
        }
    }
}

/// <summary>
/// Default color owner-draw.
/// </summary>
/// <param name="dis">Pointer to the DRAWITEMSTRUCT.</param>
static VOID DefaultColors_DrawItem(LPDRAWITEMSTRUCT dis)
{
    if (dis->itemState & ODS_DISABLED)
    {
        SetTextColor(dis->hDC, GetSysColor(COLOR_3DSHADOW));
        SetBkColor(dis->hDC, GetSysColor(COLOR_3DFACE));
    }
    else
    {
        if (dis->itemState & ODS_SELECTED)
        {
            SetTextColor(dis->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
            SetBkColor(dis->hDC, GetSysColor(COLOR_HIGHLIGHT));
        }
        else
        {
            SetTextColor(dis->hDC, GetSysColor(COLOR_WINDOWTEXT));
            SetBkColor(dis->hDC, GetSysColor(COLOR_WINDOW));
        }
    }
}

/// <summary>
/// Draw a color rectangle with border.
/// </summary>
/// <param name="hDC">Handle to the device context.</param>
/// <param name="rect">Pointer to the rectangle to paint.</param>
/// <param name="rgbBorder">Border color.</param>
/// <param name="rgbFill">Fill color.</param>
static VOID PaintFrameRect(HDC hDC, LPRECT rect, COLORREF rgbBorder, COLORREF rgbFill)
{
    HBRUSH hBorder = CreateSolidBrush(rgbBorder);
    HBRUSH hFill = CreateSolidBrush(rgbFill);

    FrameRect(hDC, rect, hBorder);
    InflateRect(rect, -1, -1);
    FillRect(hDC, rect,  hFill);
    InflateRect(rect, 1, 1);

    DeleteObject(hBorder);
    DeleteObject(hFill);
}
