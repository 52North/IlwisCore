Name "ILWIS Objects"

#=======General Symbol Definitions==========
!define VERSION "4.0.0 Alpha"
!define COMPANY "52North GmbH"
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
Var verifyDir

#=======Included files=======================
!include MUI2.nsh
!include "WinMessages.nsh"
!include "WordFunc.nsh"
!include "LogicLib.nsh"
!include "x64.nsh"

#=======Installer pages======================
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "bin\${LICENCE_FILE}"
!define MUI_DIRECTORYPAGE_TEXT_TOP "Please select installation directory!"
!define MUI_DIRECTORYPAGE_VARIABLE $INSTDIR
!define MUI_PAGE_CUSTOMFUNCTION_PRE prepareInstallDirVerify
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE leaveInstallDirVerify
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
!define MUI_COMPONENTSPAGE_NODESC
!insertmacro MUI_PAGE_COMPONENTS
!define MUI_DIRECTORYPAGE_TEXT_TOP "Please select the directory of you Python 3.3.3 (32bit) installation! $\n Here you can download Python: $\n$\n $\t http://www.python.org/ftp/python/3.3.3/python-3.3.3.msi"
!define MUI_DIRECTORYPAGE_VARIABLE $pythonDir
!define MUI_PAGE_CUSTOMFUNCTION_PRE preparePythonDirVerify
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE leavePythonDirVerify
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_COMPONENTS
!insertmacro MUI_UNPAGE_INSTFILES

#=======Installer languages==================
!insertmacro MUI_LANGUAGE English

#=======Installer attributes=================
OutFile "ILWISObjectsSetup.exe"
InstallDir $PROGRAMFILES32\52n\ILWISObjects
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
    IntOp $verifyDir 0 &
FunctionEnd

Function un.onInit
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuGroup
FunctionEnd

Function .onVerifyInstDir
    IntCmp $verifyDir 1 0 Next PyCheck ; IntCmp <Var> <value> <isEqualJump> <isLessJump> <isMoreJump>
       # IfFileExists $INSTDIR NoNext Next
       # test on valid INSTDIR?

    PyCheck:
    IntCmp $verifyDir 2 0 Next Next ; only if python directory is about to be selected
        IfFileExists "$pythondir\*.*" 0 NoNext
        IfFileExists "$pythondir\python.exe" 0 NoNext
        IfFileExists "$pythondir\README.txt" 0 NoNext

        ClearErrors
        FileOpen $0 "$pythondir\README.txt" r
        IfErrors NoNext
        FileRead $0 $1
        FileClose $0
        StrCmpS $1 "This is Python version 3.3.3$\r$\n" Next NoNext

    NoNext:
        Abort
    Next:
FunctionEnd

#======Regular functions====================
!define ENV_HKCU 'HKCU "Environment"'
!define ENV_HKLM 'HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"'
!define WIN32BIT_MAX_STRLEN 2047

#64 bit version might need SetRegView 32|64
Function setPathEnv
    Exch $0
    Push $1
    Push $2
    Push $3
    Push $4
    ReadRegStr $1 ${ENV_HKLM} PATH # returns empty string if more than 8192(1024) characters
    StrCmp $1 "" error # if PATH was already more than ${NSIS_MAX_STRLEN}
        StrLen $3 $0
        StrLen $4 $1
        IntOP $3 $3 + $4 # combined strlen
        ${If} ${RunningX64}
            IntCmp $3 ${NSIS_MAX_STRLEN} error 0 error # error if combined strlen is equal or more that ${NSIS_MAX_STRLEN}
        ${Else}
            IntCmp $3 ${WIN32BIT_MAX_STRLEN} error 0 error # error if combined strlen is equal or more that ${WIN32BIT_MAX_PATH_STRLEN}
        ${EndIf}
            ${WordAdd} $1 ";" "+$0" $2
            WriteRegExpandStr ${ENV_HKLM} PATH "$2"
            SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
            Goto done
    error:
        MessageBox MB_OK "Your PATH Environment contains/would contain zero or more than ${NSIS_MAX_STRLEN} character. Please add '$0' manually to your PATH environment!"
    done:
    Pop $4
    Pop $3
    Pop $2
    Pop $1
    Pop $0
FunctionEnd

Function un.setPathEnv
    Exch $0
    Push $1
    Push $2
    StrLen $1 $0
    IntCmp $1 ${NSIS_MAX_STRLEN} error 0 error # skip if IlwisPath path is too long
    ReadRegStr $1 ${ENV_HKLM} PATH
    StrCmp $1 "" error
        ${WordAdd} $1 ";" "-$0" $2
        WriteRegExpandStr ${ENV_HKLM} PATH "$2"
        SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
        Goto done
    error:
        MessageBox MB_OK "Your PATH Environment contains zero or more than ${NSIS_MAX_STRLEN} character. Please remove '$0' manually to your PATH environment!"
    done:
    Pop $2
    Pop $1
    Pop $0
FunctionEnd

#converter slash <-> backslash
#Push $INSTDIR
#Push "\" <OR> Push "/" ; to indicate bad slash, which will be replaced by the other
#Call StrSlash
#Pop $R0
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
Section "ILWIS Objects (required)" IOSecID
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
    File bin\libstdc*.dll
    File bin\libwinpthread-1.dll
    File bin\Qt5Core*.dll
    File bin\Qt5Sql*.dll
    File bin\Qt5Gui*.dll
    File bin\${LICENCE_FILE}

    WriteUninstaller "$INSTDIR\ILWISObjectsUninstall.exe"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayVersion "${VERSION}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Publisher "${COMPANY}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" URLInfoAbout "${URL}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon $INSTDIR\ILWISObjectsUninstall.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString $INSTDIR\ILWISObjectsUninstall.exe
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1
SectionEnd

Section "Python Extension" pySecID
    Push $INSTDIR
    Call setPathEnv
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

Section "Start Menu Shortcuts" ShCSecID
    SetOutPath "$SMPROGRAMS"
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$StartMenuGroup"
    CreateShortcut  "$SMPROGRAMS\$StartMenuGroup\Uninstall $(^Name).lnk" "$INSTDIR\ILWISObjectsUninstall.exe"
    SectionGetFlags ${pySecID} $0
    IntCmp $0 ${SF_SELECTED} 0 noPy
        CreateShortcut  "$SMPROGRAMS\$StartMenuGroup\$(^Name) Python Extension Test.lnk" '"$pythonDir\python.exe"' '-i "$INSTDIR\extensions\pythonapi\test.py"'
        noPy:
    !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

#=======Uninstaller sections=======================
Section "un.Python Extension"
    Push $INSTDIR
    Call un.setPathEnv

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
    Delete /REBOOTOK $INSTDIR\ILWISObjectsUninstall.exe
    DeleteRegValue HKLM "${REGKEY}" StartMenuGroup
    DeleteRegValue HKLM "${REGKEY}" Path
    DeleteRegKey /IfEmpty HKLM "${REGKEY}\Components"
    DeleteRegKey /IfEmpty HKLM "${REGKEY}"
    RmDir /REBOOTOK $SMPROGRAMS\$StartMenuGroup
    done:
SectionEnd

Section "un.Log and config files"
    Delete $INSTDIR\log\logfile.txt
    Delete $INSTDIR\log\logfile_ext.txt
    RmDir $INSTDIR\log
SectionEnd

Section "un.ILWIS Objects"
    RmDir /r $INSTDIR\extensions
    RmDir /r $INSTDIR\qtplugins
    RmDir /r $INSTDIR\resources
    Delete $INSTDIR\geos.dll
    Delete $INSTDIR\icudt51.dll
    Delete $INSTDIR\icuin51.dll
    Delete $INSTDIR\icuuc51.dll
    Delete $INSTDIR\ilwiscore.dll
    Delete $INSTDIR\libgcc_s_dw2-1.dll
    Delete $INSTDIR\libstdc*.dll
    Delete $INSTDIR\libwinpthread-1.dll
    Delete $INSTDIR\Qt5Core*.dll
    Delete $INSTDIR\Qt5Sql*.dll
    Delete $INSTDIR\Qt5Gui*.dll
    Delete $INSTDIR\${LICENCE_FILE}
    RmDir /REBOOTOK $INSTDIR
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
SectionEnd

#======Page callback functions=======================
Function preparePythonDirVerify
    IntOp $verifyDir 2 |
    SectionGetFlags ${pySecID} $0 # Section Python Extension selected in Components page?
    IntCmp $0 ${SF_SELECTED} +2 # scipt Python path selection if not selected
        Abort
FunctionEnd

Function leavePythonDirVerify
    IntOp $verifyDir 0 &

    # ExecShell "open" "http://www.python.org/ftp/python/3.3.3/python-3.3.3.msi"

FunctionEnd

Function prepareInstallDirVerify
    IntOp $verifyDir 1 |
FunctionEnd

Function leaveInstallDirVerify
    IntOp $verifyDir 0 &
    IfFileExists "$INSTDIR\*.*" NoNext Next
    NoNext:
        MessageBox MB_YESNO "The installation directory already exists! Do you want to continue the installation using this directoy (and possibly overwriting or deleting files contained)?" IDYES Next IDNO 0
        Abort
    Next:
FunctionEnd

Function .onSelChange
    SectionGetFlags ${pySecID} $0
    IntCmp $0 ${SF_SELECTED} 0 noPy
        GetDlgItem $R0 $HWNDPARENT 1
        SendMessage $R0 ${WM_SETTEXT} 0 "STR:Next >" /TIMEOUT=0
        Goto done
    noPy:
        GetDlgItem $R0 $HWNDPARENT 1
        SendMessage $R0 ${WM_SETTEXT} 0 "STR:Install" /TIMEOUT=0
    done:
FunctionEnd
