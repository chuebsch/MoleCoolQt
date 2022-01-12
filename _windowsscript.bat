
set QTDIR=C:\Qt\4.8.6
set PATH=C:\Qt\4.8.6\bin
set PATH=%PATH%;C:\mingw32\bin
set PATH=%PATH%;%SystemRoot%\System32
set QMAKESPEC=win32-g++-4.6
cd C:\MoleCoolQt\MoleCoolQt\
qmake
C:\MinGW32\bin\mingw32-make.exe distclean
echo "bin noch da"
qmake
C:\mingw32\bin\mingw32-make.exe release

"C:\Program Files (x86)\Windows Kits\10\bin\10.0.22000.0\x64\signtool.exe" sign /n "Open Source Developer, Christian Huebschle" /t "http://time.certum.pl" /fd sha1 /v release\molecoolqt.exe
"C:\Program Files (x86)\Windows Kits\10\bin\10.0.22000.0\x64\signtool.exe" sign /n "Open Source Developer, Christian Huebschle" /as /fd sha256 /tr "http://time.certum.pl" /td SHA256 /v release\molecoolqt.exe

cd installer
"C:\Program Files (x86)\NSIS\makensis.exe" molecoolqt.nsi
cd C:\MoleCoolQt\MoleCoolQt\
