
set QTDIR=C:\Qt\evrerywhere\qt-everywhere-opensource-src-4.8.6
set PATH=C:\Qt\evrerywhere\qt-everywhere-opensource-src-4.8.6\bin
set PATH=C:\mingw-w64\mingw64\bin
set PATH=%PATH%;%SystemRoot%\System32
set QMAKESPEC=win32-g++-4.6
cd C:\Molecoolqt
C:\Qt\evrerywhere\qt-everywhere-opensource-src-4.8.6\bin\qmake.exe
C:\mingw-w64\mingw64\bin\mingw32-make.exe distclean
echo "bin noch da"
C:\Qt\evrerywhere\qt-everywhere-opensource-src-4.8.6\bin\qmake.exe
C:\mingw-w64\mingw64\bin\mingw32-make.exe release
copy release\molecoolqt.exe 64\
cd installer
"C:\Program Files (x86)\NSIS\makensis.exe" molecoolqt64.nsi
cd C:\Molecoolqt