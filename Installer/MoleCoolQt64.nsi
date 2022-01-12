;NSIS Modern User Interface
;Basic Example Script
;Written by Joost Verburg and changed by Christian B. Hübschle

SetCompressor /SOLID lzma

;InstType "full"
;InstType "minimal"

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "MoleCoolQt64"
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
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\MoleCoolQt64" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  
  !insertmacro MUI_PAGE_INSTFILES
  !define MUI_FINISHPAGE_LINK "Visit the MoleCoolQt site for latest news and versions"
  !define MUI_FINISHPAGE_LINK_LOCATION "http://www.molecoolqt.de" 
  !define MUI_FINISHPAGE_RUN "$INSTDIR\molecoolqtQt5-64.exe"
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

Section "MoleCoolQt64" SecMoleCoolQt

  SetOutPath "$INSTDIR"
  
 ; SectionIn 1 2 RO
  ;ADD YOUR OWN FILES HERE...
  File ..\QT564\libgcc_s_seh-1.dll
  File ..\QT564\libgomp-1.dll
  File ..\QT564\libstdc++-6.dll
  File ..\QT564\libwinpthread-1.dll
  File ..\QT564\molecoolqtQt5-64.exe
  File ..\QT564\Qt5Core.dll
  File ..\QT564\Qt5Gui.dll
  File ..\QT564\Qt5Network.dll
  File ..\QT564\Qt5OpenGL.dll
  File ..\QT564\Qt5Svg.dll
  File ..\QT564\Qt5Widgets.dll
  Createdirectory "$instdir\platforms"
  SetOutPath "$INSTDIR\platforms"
  File ..\QT564\qwindows.dll   
  CreateDirectory "$INSTDIR\imageformats"
  SetOutPath "$INSTDIR\imageformats"
  File ..\QT564\imageformats\qgif.dll
  File ..\QT564\imageformats\qicns.dll
  File ..\QT564\imageformats\qico.dll
  File ..\QT564\imageformats\qjpeg.dll
  File ..\QT564\imageformats\qsvg.dll
  File ..\QT564\imageformats\qtiff.dll

  CreateDirectory "$INSTDIR\iconengines"  
  SetOutPath "$INSTDIR\iconengines"
  File ..\QT564\iconengines\qsvgicon.dll
  CreateDirectory "$INSTDIR\styles"
  SetOutPath "$INSTDIR\styles"
  File ..\QT564\styles\qwindowsvistastyle.dll

  
  ;Store installation folder
  WriteRegStr HKCU "Software\MoleCoolQt64" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\MoleCoolQt64.lnk" "$INSTDIR\molecoolqtQt5-64.exe"
    CreateShortCut "$DESKTOP\MoleCoolQt64.lnk" "$INSTDIR\molecoolqtQt5-64.exe"                     
    CreateShortCut "$QUICKLAUNCH\MoleCoolQt64.lnk" "$INSTDIR\molecoolqtQt5-64.exe"
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
  Delete "$INSTDIR\libfftw3-3.dll"	
  Delete "$INSTDIR\QtOpenGL4.dll"
  Delete "$INSTDIR\imageformats\qgif4.dll"
  Delete "$INSTDIR\imageformats\qico4.dll"
  Delete "$INSTDIR\imageformats\qjpeg4.dll"
  Delete "$INSTDIR\imageformats\qmng4.dll"
  Delete "$INSTDIR\imageformats\qtiff4.dll"
  Delete "$INSTDIR\Uninstall.exe"
  
  
  Delete "$INSTDIR\libgcc_s_seh-1.dll"
  Delete "$INSTDIR\libgomp-1.dll"
  Delete "$INSTDIR\libstdc++-6.dll"
  Delete "$INSTDIR\libwinpthread-1.dll"
  Delete "$INSTDIR\molecoolqtQt5-64.exe"
  Delete "$INSTDIR\Qt5Core.dll"
  Delete "$INSTDIR\Qt5Gui.dll"
  Delete "$INSTDIR\Qt5Network.dll"
  Delete "$INSTDIR\Qt5OpenGL.dll"
  Delete "$INSTDIR\Qt5Svg.dll"
  Delete "$INSTDIR\Qt5Widgets.dll"
  Delete "$INSTDIR\platforms\qwindows.dll"   
  Delete "$INSTDIR\imageformats\qgif.dll"
  Delete "$INSTDIR\imageformats\qicns.dll"
  Delete "$INSTDIR\imageformats\qico.dll"
  Delete "$INSTDIR\imageformats\qjpeg.dll"
  Delete "$INSTDIR\imageformats\qsvg.dll"
  Delete "$INSTDIR\imageformats\qtiff.dll"
  Delete "$INSTDIR\iconengines\qsvgicon.dll"
  Delete "$INSTDIR\styles\qwindowsvistastyle.dll"

  RMDir "$INSTDIR\imageformats"
  RMDir "$INSTDIR\iconengines"
  RMDir "$INSTDIR\platforms"
  RMDir "$INSTDIR\styles"
  RMDir "$INSTDIR"
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder

  Delete "$SMPROGRAMS\$StartMenuFolder\MoleCoolQt64.lnk"
  Delete "$DESKTOP\MoleCoolQt64.lnk"
  Delete "$QUICKLAUNCH\MoleCoolQt64.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  DeleteRegKey /ifempty HKCU "Software\MoleCoolQt64"

SectionEnd
