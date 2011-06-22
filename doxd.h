
#ifndef DOXD_H
#define DOXD_H

#include <QSettings>
#include <QDialog>
#include <QString>
#include <QProcess>
#include <QLineEdit>
#include <QPushButton>
#include <QObject>
#include <QTextBrowser>

class settingsDlg;

class Doxd:public QObject{
Q_OBJECT 
 public:
  Doxd(void);
  void runXdLsm(QString cid, QString mid="");
  void runXdProp(QString cid, QString mid="");
  void presentDialog();
  public slots:
  void jobstart();
  void jobdone( int exitCode, QProcess::ExitStatus exitStatus );
  void findNext();
  void findText();
  void findPrev();
 signals:
  void reloadXD();
 private:
  void searchchanged(bool current,bool back);
  bool isFirstTime;
  QString selectedFilter;
  QSettings *settings;
  QLineEdit *search;
  QTextEdit *editor; 
  QString xd_data_dir;
  QString xdPropPath;
  QString xdLsmPath;
  QProcess *runner;
  settingsDlg *sdlg;
};

class settingsDlg: public QDialog{
Q_OBJECT
  public:
  QStringList path;
  settingsDlg(QString xd_data_dir,QString xdLsmPath,QString xdPropPath);
 private:  
  QLineEdit *dataL,*lsmL,*propL;
  QString selectedFilter;
  QPushButton *dataBrws,*lsmBrws,*propBrws;
  public slots:
  void brwsData();
  void brwsLsm();
  void brwsProp();
  protected slots:
  void accept () ;
};
#endif
