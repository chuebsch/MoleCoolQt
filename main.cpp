// beispiele/qteditor/main.cpp
#include <QApplication>
#include "MyWindow.h"
#include <QMessageBox>
QAction *nmbx=NULL;
void mh(QtMsgType type, const char *msg){
  switch (type){
  case QtDebugMsg:
    printf("%s \n",msg);fflush(stdout);
#if defined Q_WS_WIN  ||  defined Q_WS_MAC
   if (nmbx!=NULL) if ((!nmbx->isChecked())) QMessageBox::information(0,msg,msg);
#endif
    break;
  case QtWarningMsg:
    printf("%s\n",msg);fflush(stdout);
#if defined Q_WS_WIN  ||  defined Q_WS_MAC
    if (nmbx!=NULL) if ((!nmbx->isChecked())) QMessageBox::warning(0,"warning",msg);
#endif
    break;
  case QtCriticalMsg:
    printf("%s\n",msg);fflush(stdout);
#if defined Q_WS_WIN  ||  defined Q_WS_MAC
    QMessageBox::critical(0,"critical",msg);
#endif
    break;
  case QtFatalMsg:
    printf("%s\n",msg);fflush(stdout);
#if defined Q_WS_WIN  ||  defined Q_WS_MAC
    QMessageBox::critical(0,"fatal",msg);
#endif
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
   nmbx=window->noMessBox;
   app.setWindowIcon (QIcon(":/images/icon1.png") );
   window->show();
   return app.exec();
}

