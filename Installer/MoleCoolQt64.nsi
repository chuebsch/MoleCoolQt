;NSIS Modern User Interface
;Basic Example Script
;Written by Joost Verburg and changed by Christian B. H�bschle

SetCompressor /SOLID lzma

;InstType "full"
;InstType "minimal"

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "MoleCoolQt"
  OutFile "MoleCoolQt64_setup.exe"

  ;Default installation folder
  InstallDir "$LOCALAPPDATA\MoleCoolQt64"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\MoleCoolQt64" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel user

;--------------------------------
;Variables

  Var StartMenuFolder

;--------------------------------
;Interface Settings

 
  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "banner.bmp" ; optional
  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "..\LICENSE"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY

  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\MoleCoolQt" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  
  !insertmacro MUI_PAGE_INSTFILES
  !define MUI_FINISHPAGE_LINK "Visit the MoleCoolQt site for latest news and versions"
  !define MUI_FINISHPAGE_LINK_LOCATION "http://www.molecoolqt.de" 
  !define MUI_FINISHPAGE_RUN "$INSTDIR\molecoolQt.exe"
  !define MUI_FINISHPAGE_NOREBOOTSUPPORT
  !insertmacro MUI_PAGE_FINISH
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "MoleCoolQt" SecMoleCoolQt

  SetOutPath "$INSTDIR"
  
 ; SectionIn 1 2 RO
  ;ADD YOUR OWN FILES HERE...
  File ..\64\molecoolQt.exe
  File ..\64\QtCore4.dll
  File ..\64\QtGui4.dll
  File ..\64\QtNetwork4.dll
  File ..\64\libstdc++-6.dll
  File ..\64\libfftw3f-3.dll
  File ..\64\libgomp-1.dll
  File ..\64\libwinpthread-1.dll
  File ..\64\libgcc_s_seh-1.dll
  File ..\64\QtOpenGL4.dll
  CreateDirectory "$INSTDIR\imageformats"
  SetOutPath "$INSTDIR\imageformats"  
  File ..\64\imageformats\qgif4.dll
  File ..\64\imageformats\qico4.dll
  File ..\64\imageformats\qjpeg4.dll
  File ..\64\imageformats\qmng4.dll
  File ..\64\imageformats\qtiff4.dll

  
  ;Store installation folder
  WriteRegStr HKCU "Software\MoleCoolQt" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\MoleCoolQt.lnk" "$INSTDIR\molecoolQt.exe"
    CreateShortCut "$DESKTOP\MoleCoolQt.lnk" "$INSTDIR\molecoolQt.exe"                     
    CreateShortCut "$QUICKLAUNCH\MoleCoolQt.lnk" "$INSTDIR\molecoolQt.exe"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecMoleCoolQt ${LANG_ENGLISH} "MoleCoolQt a molecule viewer for charge density related science"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecMoleCoolQt} $(DESC_SecMoleCoolQt)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...
  Delete "$INSTDIR\molecoolQt.exe"
  Delete "$INSTDIR\QtCore4.dll"  
  Delete "$INSTDIR\QtGui4.dll"  
  Delete "$INSTDIR\QtNetwork4.dll"
  Delete "$INSTDIR\mingwm10.dll"	  
  Delete "$INSTDIR\libgcc_s_seh-1.dll"
  Delete "$INSTDIR\libstdc++-6.dll"
  Delete "$INSTDIR\libgomp-1.dll"
  Delete "$INSTDIR\libpthread-2.dll"
  Delete "$INSTDIR\libwinpthread-1.dll"
  Delete "$INSTDIR\libfftw3f-3.dll"	
  Delete "$INSTDIR\QtOpenGL4.dll"
  Delete "$INSTDIR\imageformats\qgif4.dll"
  Delete "$INSTDIR\imageformats\qico4.dll"
  Delete "$INSTDIR\imageformats\qjpeg4.dll"
  Delete "$INSTDIR\imageformats\qmng4.dll"
  Delete "$INSTDIR\imageformats\qtiff4.dll"
  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR\imageformats"
  RMDir "$INSTDIR"
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder

  Delete "$SMPROGRAMS\$StartMenuFolder\MoleCoolQt.lnk"
  Delete "$DESKTOP\MoleCoolQt.lnk"
  Delete "$QUICKLAUNCH\MoleCoolQt.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  DeleteRegKey /ifempty HKCU "Software\MoleCoolQt"

SectionEnd