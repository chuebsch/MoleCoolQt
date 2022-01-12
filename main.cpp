// beispiele/qteditor/main.cpp
#include <QApplication>
#include "MyWindow.h"
#include <QMessageBox>
QAction *nmbx=NULL;
#if (QT_VERSION >= 0x050000)
void mh(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  QByteArray localMsg = msg.toLocal8Bit();
    QString pth=QDir::homePath()+"/ShelXle_message.txt";
    FILE *er=fopen(pth.toStdString().c_str(),"at");
    fprintf(er,"======%s\n%s\n======\n",
        QDateTime::currentDateTime().toString().toStdString().c_str(),
        localMsg.constData());
    fclose(er);
    if (strstr(localMsg.constData(),"QGLShader::link:")!=NULL) {
      printf("%s\n",localMsg.constData());fflush(stdout);
      return; // ignore useless error messages with shitty intel graphic cards reporting \"No error."
    }
  if (strstr(localMsg.constData(),"QAccessibleWidget")!=NULL) {
    printf("%s\n",localMsg.constData());fflush(stdout);
    return; // ignore useless error messages on win with touch screens
  }
  if (strstr(localMsg.constData(),"QGLShader::link:")!=NULL) {
    printf("%s\n",localMsg.constData());fflush(stdout);
    return; // ignore useless error messages
  }

  switch (type) {
    case QtDebugMsg:
      fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
// Q_WS_* was droppen in QT5 in favor of Q_OS_*
#if defined Q_WS_WIN  ||  defined Q_OS_WIN 
    QMessageBox::information(0,"information",msg);
#endif
      break;
    case QtInfoMsg:
      fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
#if defined Q_WS_WIN  ||  defined Q_OS_WIN 
    QMessageBox::information(0,"information",msg);
#endif
      break;
    case QtWarningMsg:
      fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
#if defined Q_WS_WIN  ||  defined Q_OS_WIN 
    QMessageBox::warning(0,"warning",msg);
#endif
      break;
    case QtCriticalMsg:
      fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
#if defined Q_WS_WIN  || defined Q_OS_WIN 
    QMessageBox::critical(0,"critical",msg);
#endif
      break;
    case QtFatalMsg:
      fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
#if defined Q_WS_WIN  || defined Q_OS_WIN 
    QMessageBox::critical(0,"fatal",msg);
#endif
      abort();
  }
}
#else
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
#endif

int main(int argc, char *argv[])  {

   QApplication app(argc, argv);
   /*
   QTranslator translator;
   translator.load("mcl_de");
   app.installTranslator(&translator); 
   */
#if (QT_VERSION >= 0x050000)
  qInstallMessageHandler(mh);
#else
   qInstallMsgHandler(mh);
#endif
   MyWindow *window = new MyWindow();
   nmbx=window->noMessBox;
   app.setWindowIcon (QIcon(":/images/icon1.png") );
   window->show();
   return app.exec();
}

