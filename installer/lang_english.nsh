
!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_ENGLISH}

; License
LicenseLangString LicenseFile         ${CURLANG} "license.txt"

; Installer sections
LangString InstTypeTypicalName        ${CURLANG} "Typical"
LangString InstTypeFullName           ${CURLANG} "Full"
LangString InstTypeMinimalName        ${CURLANG} "Minimal"
LangString MainSectionName            ${CURLANG} "iNFO (required)"
LangString LangSectionName            ${CURLANG} "Languages"
LangString EnglishLangSectionName     ${CURLANG} "English"
LangString SpanishLangSectionName     ${CURLANG} "Spanish"
LangString AssocSectionName           ${CURLANG} "File Associations"
LangString NFOAssocSectionName        ${CURLANG} "NFO"
LangString DIZAssocSectionName        ${CURLANG} "DIZ"
LangString StartMenuSectionName       ${CURLANG} "Start Menu Folder"
LangString DesktopIconSectionName     ${CURLANG} "Desktop Icon"
LangString QuickLaunchIconSectionName ${CURLANG} "Quick Launch Icon"

; Descriptions
LangString MainSectionDesc            ${CURLANG} "Install essential files."
LangString LangSectionDesc            ${CURLANG} "Add localized resources for the following languages."
LangString EnglishLangSectionDesc     ${CURLANG} "Install English language files."
LangString SpanishLangSectionDesc     ${CURLANG} "Install Spanish language files."
LangString AssocSectionDesc           ${CURLANG} "Associate the application with the following file extensions."
LangString NFOAssocSectionDesc        ${CURLANG} "Register as default viewer for .nfo files."
LangString DIZAssocSectionDesc        ${CURLANG} "Register as default viewer for .diz files."
LangString StartMenuSectionDesc       ${CURLANG} "Add shortcuts to the start menu."
LangString DesktopIconSectionDesc     ${CURLANG} "Add a desktop icon."
LangString QuickLaunchIconSectionDesc ${CURLANG} "Add a quick launch icon."

; Intermediate page
LangString IntermediatePageTitle      ${CURLANG} "Install $(^Name)"
LangString IntermediatePageSubtitle   ${CURLANG} "$(^Name) is ready to be installed on your computer."
LangString IntermediatePageText       ${CURLANG} "All the required information has been gathered. Click Install to start the installation."

; Uninstaller
LangString un.UninstQuestionRemove    ${CURLANG} "Are you sure you want to completely remove $(^Name) and all of its components?"
LangString un.UninstSuccessfulRemove  ${CURLANG} "$(^Name) was successfully removed from your computer."
