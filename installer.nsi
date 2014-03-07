Name "ILWIS 4"

#=======General Symbol Definitions==========
!define VERSION "4.0.0 Alpha"
!define COMPANY "52N GmbH"
!define URL www.52n.org
!define REGKEY "SOFTWARE\$(^Name)"
!define LICENCE_FILE "LICENSE-2.0.txt"

#=======MUI Symbol Definitions==============
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_UNFINISHPAGE_NOAUTOCLOSE
!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM
!define MUI_STARTMENUPAGE_REGISTRY_KEY ${REGKEY}
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME StartMenuGroup
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "$(^Name)"
Var StartMenuGroup

#=======Python Directory Definitions=========
!define PYTHONDEFAULTDIR "C:\Python33"
Var pythonDir
Var verifyPythonDir

#=======Included files=======================
!include MUI2.nsh
!include Sections.nsh

#=======Installer pages======================
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "bin\${LICENCE_FILE}"
!define MUI_DIRECTORYPAGE_TEXT_TOP "Please select installation directory!"
!define MUI_DIRECTORYPAGE_VARIABLE $INSTDIR
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
!define MUI_COMPONENTSPAGE_NODESC
!insertmacro MUI_PAGE_COMPONENTS
!define MUI_DIRECTORYPAGE_TEXT_TOP "Please select Python 3.3 directory!"
!define MUI_DIRECTORYPAGE_VARIABLE $pythonDir
!define MUI_PAGE_CUSTOMFUNCTION_PRE preparePythonDirVerify
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE leavePythonDirVerify
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_COMPONENTS
!insertmacro MUI_UNPAGE_INSTFILES

#=======Installer languages==================
!insertmacro MUI_LANGUAGE English

#=======Installer attributes=================
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

#======Callback functions====================
Function .onInit
    StrCpy $pythonDir ${PYTHONDEFAULTDIR}
    IntOp $verifyPythonDir 0 &
FunctionEnd

Function un.onInit
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuGroup
FunctionEnd

Function .onVerifyInstDir
    IntCmp $verifyPythonDir 0 Next
        IfFileExists "$pythondir\*.*" 0 NoNext
        IfFileExists "$pythondir\python.exe" 0 NoNext
        IfFileExists "$pythondir\README.txt" Next NoNext

        ClearErrors
        FileOpen $0 "$pythondir\README.txt" r
        IfErrors NoNext
        FileRead $0 $1
        FileClose $0
        StrCmp $1 "This is Python version 3.3.3" Next NoNext

        NoNext:
            Abort
    Next:
FunctionEnd

Function preparePythonDirVerify
    IntOp $verifyPythonDir 1 |
FunctionEnd
Function leavePythonDirVerify
    IntOp $verifyPythonDir 0 &
FunctionEnd

#======Regular functions====================
!include "winmessages.nsh"
!define ENV_HKCU 'HKCU "Environment"'

Function appendPathEnv
    Pop $0
    WriteRegExpandStr ${ENV_HKCU} PATH $0
    SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
FunctionEnd

Function StrSlash
      Exch $R3 ; $R3 = needle ("\" or "/")
      Exch
      Exch $R1 ; $R1 = String to replacement in (haystack)
      Push $R2 ; Replaced haystack
      Push $R4 ; $R4 = not $R3 ("/" or "\")
      Push $R6
      Push $R7 ; Scratch reg
      StrCpy $R2 ""
      StrLen $R6 $R1
      StrCpy $R4 "\"
      StrCmp $R3 "/" loop
      StrCpy $R4 "/"
    loop:
      StrCpy $R7 $R1 1
      StrCpy $R1 $R1 $R6 1
      StrCmp $R7 $R3 found
      StrCpy $R2 "$R2$R7"
      StrCmp $R1 "" done loop
    found:
      StrCpy $R2 "$R2$R4"
      StrCmp $R1 "" done loop
    done:
      StrCpy $R3 $R2
      Pop $R7
      Pop $R6
      Pop $R4
      Pop $R2
      Pop $R1
      Exch $R3
FunctionEnd

#=======Installer sections==================
Section "ILWIS 4 (required)"
    SectionIn RO
    SetOverwrite on

    SetOutPath "$INSTDIR\extensions"
    File /r bin\extensions\*
    SetOutPath "$INSTDIR\qtplugins"
    File /r bin\qtplugins\*
    SetOutPath "$INSTDIR\resources"
    File /r bin\resources\*
    SetOutPath "$INSTDIR"
    File bin\geos.dll
    File bin\icudt51.dll
    File bin\icuin51.dll
    File bin\icuuc51.dll
    File bin\ilwiscore.dll
    File bin\libgcc_s_dw2-1.dll
    File bin\libstdc++-6.dll
    File bin\libwinpthread-1.dll
    File bin\Qt5Core*.dll
    File bin\Qt5Sql*.dll
    File bin\Qt5Gui*.dll
    File bin\${LICENCE_FILE}

    WriteUninstaller $INSTDIR\uninstall.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayVersion "${VERSION}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Publisher "${COMPANY}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" URLInfoAbout "${URL}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon $INSTDIR\uninstall.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString $INSTDIR\uninstall.exe
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1
SectionEnd

Section "Python API"
    Push $INSTDIR
    Call appendPathEnv
    Push $INSTDIR
    Push "\"
    Call StrSlash
    Pop $R0
    DetailPrint "Create $pythonDir\qt.conf"
    WriteINIStr $pythonDir\qt.conf Paths Prefix "$R0/"
    WriteINIStr $pythonDir\qt.conf Paths Plugins "qtplugins"
    DetailPrint "Create $pythonDir\ilwisobjects.conf"
    WriteINIStr $pythonDir\ilwisobjects.conf Paths ilwisDir "$R0/"

    SetOutPath $pythonDir\Lib\site-packages
    DetailPrint "Copy to $pythonDir\Lib\site-packages\_ilwisobjects.pyd"
    File bin\extensions\pythonapi\_ilwisobjects.pyd
    DetailPrint "Copy to $pythonDir\Lib\site-packages\ilwisobjects.py"
    File bin\extensions\pythonapi\ilwisobjects.py

    WriteRegStr HKLM "${REGKEY}\python" Path $pythonDir
SectionEnd

Section "Start Menu Shortcuts"
    SetOutPath "$SMPROGRAMS"
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$StartMenuGroup"
    CreateShortcut  "$SMPROGRAMS\$StartMenuGroup\Uninstall $(^Name).lnk" "$INSTDIR\uninstall.exe"
    CreateShortcut  "$SMPROGRAMS\$StartMenuGroup\$(^Name) Python API Test.lnk" '"$pythonDir\python.exe"' '"$INSTDIR\extensions\pythonapi\test.py"'
    !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

#=======Uninstaller sections=======================
Section "un.Python API"
    ReadRegStr $pythonDir HKLM "${REGKEY}\python" Path
    DeleteRegValue HKLM "${REGKEY}\python" Path
    DeleteRegKey /IfEmpty HKLM "${REGKEY}\python"

    DetailPrint "Delete $pythonDir\ilwisobjects.conf"
    Delete /REBOOTOK $pythonDir\ilwisobjects.conf
    DetailPrint "Delete $pythonDir\qt.conf"
    Delete /REBOOTOK $pythonDir\qt.conf

    DetailPrint "Delete $pythonDir\Lib\site-packages\_ilwisobjects.pyd"
    Delete /REBOOTOK $pythonDir\Lib\site-packages\_ilwisobjects.pyd
    DetailPrint "Delete $pythonDir\Lib\site-packages\ilwisobjects.py"
    Delete /REBOOTOK $pythonDir\Lib\site-packages\ilwisobjects.py
SectionEnd

Section "un.Start Menu Shortcuts"
    StrCmp $StartMenuGroup "" done
    RmDir /r /REBOOTOK "$SMPROGRAMS\$StartMenuGroup"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\Uninstall $(^Name).lnk"
    Delete /REBOOTOK $INSTDIR\uninstall.exe
    DeleteRegValue HKLM "${REGKEY}" StartMenuGroup
    DeleteRegValue HKLM "${REGKEY}" Path
    DeleteRegKey /IfEmpty HKLM "${REGKEY}\Components"
    DeleteRegKey /IfEmpty HKLM "${REGKEY}"
    RmDir /REBOOTOK $SMPROGRAMS\$StartMenuGroup
    done:
SectionEnd

Section "un.ILWIS 4"
    RmDir /r /REBOOTOK $INSTDIR
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
SectionEnd
