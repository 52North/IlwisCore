Name "ILWIS 4"

# General Symbol Definitions
!define VERSION "4.0.0 Alpha"
!define COMPANY "52N GmbH"
!define URL www.52n.org

# MUI Symbol Definitions
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

# Included files
!include MUI2.nsh

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "bin\LICENSE-2.0.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE English

# Installer attributes
OutFile setup.exe
InstallDir $PROGRAMFILES\52n\Ilwis4
CRCCheck on
XPStyle on
ShowInstDetails show
VIProductVersion 4.0.0.0
VIAddVersionKey ProductName ILWIS
VIAddVersionKey ProductVersion "${VERSION}"
VIAddVersionKey CompanyName "${COMPANY}"
VIAddVersionKey CompanyWebsite "${URL}"
VIAddVersionKey FileVersion "${VERSION}"
VIAddVersionKey FileDescription ""
VIAddVersionKey LegalCopyright ""
ShowUninstDetails show
RequestExecutionLevel user

# Installer sections
Section Main
    SetOutPath $INSTDIR
    SetOverwrite on

    File /r bin\*
    WriteUninstaller $INSTDIR\uninstall.exe

    SetOutPath $SMPROGRAMS
    CreateDirectory "$SMPROGRAMS\$(^Name)"
    CreateShortcut  "$SMPROGRAMS\$(^Name)\Uninstall $(^Name).lnk" $INSTDIR\uninstall.exe
    CreateShortcut  "$SMPROGRAMS\$(^Name)\$(^Name).lnk" "python $INSTDIR\test.py"
SectionEnd

# Uninstaller sections
Section un.Main
    RmDir /r $INSTDIR
    RmDir /r /REBOOTOK "$SMPROGRAMS\$(^Name)"
SectionEnd

