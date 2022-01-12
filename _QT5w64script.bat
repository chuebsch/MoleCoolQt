set PATH=%PATH%;C:\Qt\qt5\5.13.0\mingw73_64\bin
set PATH=%PATH%;C:/Qt/qt5/Tools/mingw730_64\bin
set PATH=%PATH%;%SystemRoot%\System32
cd C:\MoleCoolQt\MoleCoolQt\
C:\qt\Qt-5.13.0\bin\qmake.exe
C:\Qt\qt5\Tools\mingw730_64\bin\mingw32-make.exe distclean
echo "bin noch da"
C:\qt\Qt-5.13.0\bin\qmake.exe
C:\Qt\qt5\Tools\mingw730_64\bin\mingw32-make.exe release

move release\molecoolqt.exe QT564\molecoolqtQt5-64.exe
"C:\Program Files (x86)\Windows Kits\10\bin\10.0.22000.0\x64\signtool.exe" sign /n "Open Source Developer, Christian Huebschle" /t "http://time.certum.pl" /fd sha1 /v QT564\molecoolqtQt5-64.exe
"C:\Program Files (x86)\Windows Kits\10\bin\10.0.22000.0\x64\signtool.exe" sign /n "Open Source Developer, Christian Huebschle" /as /fd sha256 /tr "http://time.certum.pl" /td SHA256 /v QT564\molecoolqtQt5-64.exe

cd installer
"C:\Program Files (x86)\NSIS\makensis.exe" molecoolqt64.nsi
cd C:\MoleCoolQt\MoleCoolQt\