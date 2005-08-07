
;--------------------------------
; Include Modern UI

  !include "MUI.nsh"
  !include "Sections.nsh"
  !include "FileFunc.nsh"

;--------------------------------
; Configuration

  SetCompressor lzma

  SetOverwrite on
  AutoCloseWindow false
  ShowInstDetails show
  ShowUninstDetails show

  Caption "$(^Name) Install"
  BrandingText "$(^Name)"

  ; Default installation folder
  InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"

  ; Get installation folder from registry if available
  InstallDirRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_SHORT_NAME}.exe" ""

;--------------------------------
; Variables

  Var STARTMENU_FOLDER
  Var REFRESH_SHELL_ICONS
  Var INTERMEDIATE_HWND

;--------------------------------
; Macros

  !insertmacro RefreshShellIcons
  !insertmacro un.RefreshShellIcons

;--------------------------------
; Modern UI configuration

  !define MUI_ICON   "app_install.ico"
  !define MUI_UNICON "app_uninstall.ico"

  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_RIGHT
  !define MUI_HEADERIMAGE_BITMAP "header.bmp"

  !define MUI_COMPONENTSPAGE_SMALLDESC

  !define MUI_ABORTWARNING

  ; Remember installer language
  !define MUI_LANGDLL_REGISTRY_ROOT HKLM
  !define MUI_LANGDLL_REGISTRY_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

  ; Remember Start Menu folder
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

;--------------------------------
; Pages

  ; Welcome
  !insertmacro MUI_PAGE_WELCOME
  ; License
  !insertmacro MUI_PAGE_LICENSE "$(LicenseFile)"
  ; Select components
  !insertmacro MUI_PAGE_COMPONENTS
  ; Select directory
  !insertmacro MUI_PAGE_DIRECTORY
  ; Start Menu folder
  !define MUI_STARTMENUPAGE_DEFAULTFOLDER "${PRODUCT_NAME}"
  !define MUI_STARTMENUPAGE_NODISABLE
  !define MUI_PAGE_CUSTOMFUNCTION_PRE StartMenuSkip
  !insertmacro MUI_PAGE_STARTMENU "Application" $STARTMENU_FOLDER
  ; Intermediate page
  Page custom IntermediatePage
  ; Install files
  !insertmacro MUI_PAGE_INSTFILES
  ; Finish
  !define MUI_FINISHPAGE_RUN "$INSTDIR\${PRODUCT_SHORT_NAME}.exe"
  !insertmacro MUI_PAGE_FINISH

  ; Uninstall
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
; Languages

  !insertmacro MUI_LANGUAGE "English"
  !include "lang_english.nsh"

  ;!insertmacro MUI_LANGUAGE "Spanish"
  ;!include "lang_spanish.nsh"

;--------------------------------
; Reserve Files

  ;!insertmacro MUI_RESERVEFILE_LANGDLL

  ReserveFile "page_intermediate.ini"
  !insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

;--------------------------------
; Installer sections

InstType "$(InstTypeFullName)"
InstType "$(InstTypeTypicalName)"
InstType "$(InstTypeMinimalName)"

Section "!$(MainSectionName)" MainSection
SectionIn 1 2 3 RO

  SetOutPath "$INSTDIR"
  File "license.txt"
  File "changelog.txt"
  File "..\${PRODUCT_BIN_FOLDER}\${PRODUCT_SHORT_NAME}.exe"

  ; Add the font
  SetOutPath "$FONTS"
  File "ibmpc.fon"
  System::Call "gdi32.dll::AddFontResource(t) i ('ibmpc.fon') .s"
  Pop $0
  SendMessage ${HWND_BROADCAST} ${WM_FONTCHANGE} 0 0

SectionEnd

SectionGroup /e "$(LangSectionName)" LangSection

  Section "$(EnglishLangSectionName)" EnglishLangSection
  SectionIn 1 2

    SetOutPath "$INSTDIR\lang\9"
    File "..\${PRODUCT_BIN_FOLDER}\lang\9\res.dll"

  SectionEnd

  Section "$(SpanishLangSectionName)" SpanishLangSection
  SectionIn 1

    SetOutPath "$INSTDIR\lang\10"
    File "..\${PRODUCT_BIN_FOLDER}\lang\10\res.dll"

  SectionEnd

SectionGroupEnd

SectionGroup /e "$(AssocSectionName)" AssocSection

  Section "$(NFOAssocSectionName)" NFOAssocSection
  SectionIn 1 2

    ; Create root entry
    WriteRegStr HKCR "${PRODUCT_NAME}.NFO" "" "NFO File"
    WriteRegStr HKCR "${PRODUCT_NAME}.NFO\DefaultIcon" "" "$INSTDIR\${PRODUCT_SHORT_NAME}.exe"
    WriteRegStr HKCR "${PRODUCT_NAME}.NFO\shell\open\command" "" `"$INSTDIR\${PRODUCT_SHORT_NAME}.exe" "%1"`
    ; Backup previous association for NFO files before writing new one
    ReadRegStr $0 HKCR ".nfo" ""
    StrCmp $0 "${PRODUCT_NAME}.NFO" no_do_nfo do_nfo
    do_nfo:
    WriteRegStr HKCR ".nfo" "Backup" $0
    WriteRegStr HKCR ".nfo" "" "${PRODUCT_NAME}.NFO"
    StrCpy $REFRESH_SHELL_ICONS "true"
    no_do_nfo:
    WriteRegStr HKCR ".nfo" "PerceivedType" "text"

  SectionEnd

  Section "$(DIZAssocSectionName)" DizAssocSection
  SectionIn 1 2

    ; Create root entry
    WriteRegStr HKCR "${PRODUCT_NAME}.DIZ" "" "DIZ File"
    WriteRegStr HKCR "${PRODUCT_NAME}.DIZ\DefaultIcon" "" "$INSTDIR\${PRODUCT_SHORT_NAME}.exe"
    WriteRegStr HKCR "${PRODUCT_NAME}.DIZ\shell\open\command" "" `"$INSTDIR\${PRODUCT_SHORT_NAME}.exe" "%1"`
    ; Backup previous association for DIZ files before writing new one
    ReadRegStr $0 HKCR ".diz" ""
    StrCmp $0 "${PRODUCT_NAME}.DIZ" no_do_diz do_diz
    do_diz:
    WriteRegStr HKCR ".diz" "Backup" $0
    WriteRegStr HKCR ".diz" "" "${PRODUCT_NAME}.DIZ"
    StrCpy $REFRESH_SHELL_ICONS "true"
    no_do_diz:
    WriteRegStr HKCR ".diz" "PerceivedType" "text"

  SectionEnd

SectionGroupEnd

Section "$(StartMenuSectionName)" StartMenuSection
SectionIn 1 2

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_SHORT_NAME}.exe"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\uninst.exe"
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

Section "$(DesktopIconSectionName)" DesktopIconSection
SectionIn 1

    CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_SHORT_NAME}.exe"

SectionEnd

Section "$(QuickLaunchIconSectionName)" QuickLaunchIconSection
SectionIn 1

    CreateShortCut "$QUICKLAUNCH\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_SHORT_NAME}.exe"

SectionEnd

Section "-Post"

  ; Write some registry keys
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKCR "Applications\${PRODUCT_SHORT_NAME}.exe" "" ""
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_SHORT_NAME}.exe" "" "$INSTDIR\${PRODUCT_SHORT_NAME}.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayName" "$(^Name)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayIcon" "$INSTDIR\${PRODUCT_SHORT_NAME}.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "UninstallString" "$INSTDIR\uninst.exe"

  ; If we changed file associations, we have to refresh
  StrCmp $REFRESH_SHELL_ICONS "true" 0 +2
  ${RefreshShellIcons}

SectionEnd

;--------------------------------
; Installer functions

Function .onInit

  ;!insertmacro MUI_LANGDLL_DISPLAY

  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "page_intermediate.ini"

  ; Standard installation by default
  SetCurInstType 1

FunctionEnd

Function IntermediatePage

  !insertmacro MUI_HEADER_TEXT "$(IntermediatePageTitle)" "$(IntermediatePageSubtitle)"
  !insertmacro MUI_INSTALLOPTIONS_WRITE "page_intermediate.ini" "Field 1" "Text" "$(IntermediatePageText)"

  !insertmacro MUI_INSTALLOPTIONS_INITDIALOG "page_intermediate.ini"
  Pop $INTERMEDIATE_HWND
  !insertmacro MUI_INSTALLOPTIONS_SHOW

FunctionEnd

Function StartMenuSkip

  ; If Start Menu section wasn't selected, skip Start Menu page
  SectionGetFlags ${StartMenuSection} $0
  IntOp $0 $0 & ${SF_SELECTED}
  StrCmp $0 ${SF_SELECTED} +2 0
  Abort

FunctionEnd

;--------------------------------
; Descriptions

  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${MainSection}            "$(MainSectionDesc)"
    !insertmacro MUI_DESCRIPTION_TEXT ${LangSection}            "$(LangSectionDesc)"
    !insertmacro MUI_DESCRIPTION_TEXT ${EnglishLangSection}     "$(EnglishLangSectionDesc)"
    !insertmacro MUI_DESCRIPTION_TEXT ${SpanishLangSection}     "$(SpanishLangSectionDesc)"
    !insertmacro MUI_DESCRIPTION_TEXT ${AssocSection}           "$(AssocSectionDesc)"
    !insertmacro MUI_DESCRIPTION_TEXT ${NFOAssocSection}        "$(NFOAssocSectionDesc)"
    !insertmacro MUI_DESCRIPTION_TEXT ${DizAssocSection}        "$(DIZAssocSectionDesc)"
    !insertmacro MUI_DESCRIPTION_TEXT ${StartMenuSection}       "$(StartMenuSectionDesc)"
    !insertmacro MUI_DESCRIPTION_TEXT ${DesktopIconSection}     "$(DesktopIconSectionDesc)"
    !insertmacro MUI_DESCRIPTION_TEXT ${QuickLaunchIconSection} "$(QuickLaunchIconSectionDesc)"
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
; Uninstaller section

Section "Uninstall"

  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $STARTMENU_FOLDER

  ; Delete program files
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\license.txt"
  Delete "$INSTDIR\changelog.txt"
  Delete "$INSTDIR\${PRODUCT_SHORT_NAME}.exe"
  Delete "$INSTDIR\lang\9\res.dll"
  RMDir "$INSTDIR\lang\9"
  Delete "$INSTDIR\lang\10\res.dll"
  RMDir "$INSTDIR\lang\10"
  RMDir "$INSTDIR\lang"
  RMDir "$INSTDIR"

  ; Delete the font
  System::Call "gdi32.dll::RemoveFontResource(t) i ('ibmpc.fon') .s"
  Pop $0
  SendMessage ${HWND_BROADCAST} ${WM_FONTCHANGE} 0 0
  Delete "$FONTS\ibmpc.fon"

  ; Restore backup association for NFO files
  ReadRegStr $0 HKCR ".nfo" ""
  StrCmp $0 "${PRODUCT_NAME}.NFO" undo_nfo no_undo_nfo
  undo_nfo:
  ReadRegStr $0 HKCR ".nfo" "Backup"
  WriteRegStr HKCR ".nfo" "" $0
  StrCpy $REFRESH_SHELL_ICONS "true"
  DeleteRegValue HKCR ".nfo" "Backup"
  no_undo_nfo:
  ; Delete root entry
  DeleteRegKey HKCR "${PRODUCT_NAME}.NFO"

  ; Restore backup association for DIZ files
  ReadRegStr $0 HKCR ".diz" ""
  StrCmp $0 "${PRODUCT_NAME}.DIZ" undo_diz no_undo_diz
  undo_diz:
  ReadRegStr $0 HKCR ".diz" "Backup"
  WriteRegStr HKCR ".diz" "" $0
  StrCpy $REFRESH_SHELL_ICONS "true"
  DeleteRegValue HKCR ".diz" "Backup"
  no_undo_diz:
  ; Delete root entry
  DeleteRegKey HKCR "${PRODUCT_NAME}.DIZ"

  ; Delete shortcuts
  Delete "$SMPROGRAMS\$STARTMENU_FOLDER\${PRODUCT_NAME}.lnk"
  Delete "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk"
  RMDir "$SMPROGRAMS\$STARTMENU_FOLDER"
  Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
  Delete "$QUICKLAUNCH\${PRODUCT_NAME}.lnk"

  ; Delete registry keys
  ;DeleteRegKey HKLM "Software\${PRODUCT_NAME}"
  DeleteRegKey HKCR "Applications\${PRODUCT_SHORT_NAME}.exe"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_SHORT_NAME}.exe"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"

  ; If we changed file associations, we have to refresh
  StrCmp $REFRESH_SHELL_ICONS "true" 0 +2
  ${un.RefreshShellIcons}

  SetAutoClose true

SectionEnd

;--------------------------------
; Uninstaller functions

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE

  ;MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "$(un.UninstQuestionRemove)" IDYES +2
  ;Abort

FunctionEnd

Function un.onUninstSuccess

  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(un.UninstSuccessfulRemove)"

FunctionEnd
