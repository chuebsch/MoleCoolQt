######################################################################
# Automatically generated by qmake (2.01a) Sa 22. Dez 19:00:33 2007
######################################################################

TEMPLATE = app
TARGET = molecoolqt
DEPENDPATH += .
INCLUDEPATH += . 
QT += opengl
QT += network
QT += widgets
win32: RC_FILE =molecoolQt.rc
mac {
ICON = molecoolQt.icns
}
target.path = /usr/bin
INSTALLS += target
icon.path = /usr/share/pixmaps
icon.files = molecoolqt.png
INSTALLS += icon
desktop.path = /usr/share/applications
desktop.files = molecoolqt.desktop
INSTALLS += desktop

QMAKE_CXXFLAGS += -fopenmp
#LIBS += -lfftw3f -fopenmp -lfftw3
LIBS +=  -fopenmp 
!mac{
unix {
LIBS += -lGLU
}}
# Input
HEADERS += MyWindow.h cubeGL.h molekul.h invariomDlg.h inames.h  XDDlg.h highlighter.h doxd.h eacDlg.h \
moliso.h gradDlg.h molisoStartDlg.h fourmcq.h psewidget.h ewaldsphere.h hirsh.h
SOURCES += main.cpp MyWindow.cpp cubeGL.cpp molekul.cpp invariomDlg.cpp inames.cpp XDDlg.cpp  highlighter.cpp \
doxd.cpp eacDlg.cpp moliso.cpp gradDlg.cpp molisoStartDlg.cpp fourmcq.cpp psewidget.cpp ewaldsphere.cpp hirsh.cpp\
kissfft/kiss_fftnd.c \
kissfft/kiss_fft.c \

RESOURCES += icons.qrc
