// beispiele/qteditor/main.cpp
#include <QApplication>
#include "MyWindow.h"
#include <QMessageBox>

void mh(QtMsgType type, const char *msg){
  switch (type){
  case QtDebugMsg:
    QMessageBox::information(0,msg,msg);
    break;
  case QtWarningMsg:
    QMessageBox::warning(0,"warning",msg);
    break;
  case QtCriticalMsg:
    QMessageBox::critical(0,"critical",msg);
    break;
  case QtFatalMsg:
    QMessageBox::critical(0,"fatal",msg);
    abort();
  }
}

int main(int argc, char *argv[])  {

   QApplication app(argc, argv);
   /*
   QTranslator translator;
   translator.load("mcl_de");
   app.installTranslator(&translator); 
   */
   qInstallMsgHandler(mh);
   MyWindow *window = new MyWindow();
   app.setWindowIcon (QIcon(":/images/icon1.png") );
   window->show();
   return app.exec();
}

