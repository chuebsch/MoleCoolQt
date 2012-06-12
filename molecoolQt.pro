######################################################################
# Automatically generated by qmake (2.01a) Sa 22. Dez 19:00:33 2007
######################################################################

TEMPLATE = app
TARGET = molecoolQt
DEPENDPATH += .
INCLUDEPATH += . 
QT += opengl
win32: RC_FILE =molecoolQt.rc
mac {
ICON = molecoolQt.icns
}

QMAKE_CXXFLAGS += -fopenmp
LIBS += -lfftw3f -fopenmp
unix {
LIBS += -lGLU
}
# Input
HEADERS += MyWindow.h cubeGL.h molekul.h invariomDlg.h inames.h  XDDlg.h highlighter.h doxd.h eacDlg.h \
moliso.h gradDlg.h molisoStartDlg.h fourmcq.h psewidget.h
SOURCES += main.cpp MyWindow.cpp cubeGL.cpp molekul.cpp invariomDlg.cpp inames.cpp XDDlg.cpp  highlighter.cpp \
doxd.cpp eacDlg.cpp moliso.cpp gradDlg.cpp molisoStartDlg.cpp fourmcq.cpp psewidget.cpp
RESOURCES += icons.qrc
