Name "ILWIS 4"

# General Symbol Definitions
!define VERSION "4.0.0 Alpha"
!define COMPANY "52N GmbH"
!define URL www.52n.org

# MUI Symbol Definitions
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_UNFINISHPAGE_NOAUTOCLOSE
!define LICENCE_FILE "LICENSE-2.0.txt"

!define PYTHONDEFAULTDIR "C:\Python33"
Var pythonDir

# Included files
!include MUI2.nsh

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "bin\${LICENCE_FILE}"
!define MUI_DIRECTORYPAGE_TEXT_TOP "Please select Python 3.3 directory!"
!define MUI_DIRECTORYPAGE_VARIABLE $pythonDir
!insertmacro MUI_PAGE_DIRECTORY
!define MUI_DIRECTORYPAGE_TEXT_TOP "Please select installation directory!"
!define MUI_DIRECTORYPAGE_VARIABLE $INSTDIR
!insertmacro MUI_PAGE_DIRECTORY
!define MUI_COMPONENTSPAGE_NODESC
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_COMPONENTS
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE English

# Installer attributes
OutFile setup.exe
InstallDir $PROGRAMFILES32\52n\Ilwis4
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
RequestExecutionLevel admin

Function .onInit
    StrCpy $pythonDir ${PYTHONDEFAULTDIR}
FunctionEnd

Function .onVerifyInstDir
    IfFileExists "$pythondir\*.*" 0 NoNext
    IfFileExists "$pythondir\python.exe" Next NoNext
    NoNext:
        Abort
    Next:
FunctionEnd


; include for some of the windows messages defines
!include "winmessages.nsh"
; HKLM (all users) vs HKCU (current user) defines
!define ENV_HKLM 'HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"'
!define ENV_HKCU 'HKCU "Environment"'

Function appendPathEnv path
    ; set variable
    WriteRegExpandStr ${ENV_HKLM} PATH $path
    ; make sure windows knows about the change
    SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
FunctionEnd

# Installer sections
Section "ILWIS 4 (required)"
    SectionIn RO
    SetOutPath $INSTDIR
    SetOverwrite on

    File /r bin\extensions\*
    File /r bin\qtplugins\*
    File /r bin\resources\*
    File bin\geos.dll
    File bin\icudt51.dll
    File bin\icuin51.dll
    File bin\icuuc51.dll
    File bin\ilwiscore.dll
    File bin\libgcc_s_dw2-1.dll
    File bin\libstdc++-6.dll
    File bin\libwinpthread-1.dll
    File bin\Qt5Core.dll
    File bin\Qt5Sql.dll
    File bin\${LICENCE_FILE}
    WriteUninstaller $INSTDIR\uninstall.exe
SectionEnd

Section "Start Menu Shortcuts"
    SetOutPath $SMPROGRAMS
    CreateDirectory "$SMPROGRAMS\$(^Name)"
    CreateShortcut  "$SMPROGRAMS\$(^Name)\Uninstall $(^Name).lnk" $INSTDIR\uninstall.exe
    CreateShortcut  "$SMPROGRAMS\$(^Name)\$(^Name).lnk" "python $INSTDIR\extensions\pythonapi\test.py"
SectionEnd

Section "Python API"
    Var ALREADY_INSTALLED
    !insertmacro InstallLib REGDLL $ALREADY_INSTALLED NOREBOOT_NOTPROTECTED ilwiscore.dll $INSTDIR\ilwiscore.dll $INSTDIR

    SetOutPath $pythonDir
    WriteINIStr qt.conf Paths Prefix $INSTDIR
    WriteINIStr ilwisobjects.conf Paths ilwisDir "D:/Profiles/KiesslingHG/ILWIS/projects/output/win32debug/bin/"
SectionEnd

# Uninstaller sections
Section "un.Python API"
    !insertmacro UnInstallLib REGDLL SHARED NOREBOOT_NOTPROTECTED $INSTDIR\ilwiscore.dll
    Delete "$pythonDir\ilwisobjects.conf"
    Delete "$pythonDir\qt.conf"
SectionEnd

Section "un.ILWIS 4"
    RmDir /r $INSTDIR
SectionEnd

Section "un.Start Menu Shortcuts"
    RmDir /r /REBOOTOK "$SMPROGRAMS\$(^Name)"
SectionEnd
