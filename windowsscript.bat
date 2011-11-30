set QTDIR=C:\QtSdk\desktop\Qt\4.7.4
set PATH=C:\\QtSdk\desktop\Qt\4.7.4\mingw\bin
set PATH=%PATH%;C:\mingw\bin
set PATH=%PATH%;%SystemRoot%\System32
set QMAKESPEC=win32-g++
cd C:\Molecoolqt\Molecoolqt
qmake
C:\MinGW\bin\mingw32-make.exe distclean
echo "bin noch da"
qmake
C:\mingw\bin\mingw32-make.exe release
cd installer
"C:\Program Files\NSIS\makensis.exe" molecoolqt.nsi
cd C:\Molecoolqt\Molecoolqt