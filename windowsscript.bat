
set QTDIR=C:\Qt\4.8.6
set PATH=C:\Qt\4.8.6\bin
set PATH=%PATH%;C:\mingw32\bin
set PATH=%PATH%;%SystemRoot%\System32
set QMAKESPEC=win32-g++-4.6
cd C:\Molecoolqt
qmake
C:\MinGW32\bin\mingw32-make.exe distclean
echo "bin noch da"
qmake
C:\mingw32\bin\mingw32-make.exe release
cd installer
"C:\Program Files (x86)\NSIS\makensis.exe" molecoolqt.nsi
cd C:\Molecoolqt