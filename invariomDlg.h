#ifndef INVARIOMDLG_H
#define INVARIOMDLG_H 1
#include <QList>
#include <QString>
#include <QDialog>
#include <QLabel>
#include <QGLWidget>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <iostream>
#include "molekul.h"

#ifndef MAXSELECT 
#define MAXSELECT 300001
#endif

class invDlg;

class invariomDlg: public QGLWidget {
Q_OBJECT
 public:
  invariomDlg(CEnvironment ch,const Connection cll, QWidget *parent = 0,invDlg *id=0 );
  QString name;
  Connection cl;
  V3 mitsav;
 protected:
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);
  void contextMenuEvent(QContextMenuEvent *event);
 private:
  void draw();   
  CEnvironment ce;
  QPoint lastPos;
  invDlg *papi;
  GLuint selectBuf[MAXSELECT];
  GLint vp[4];
  GLuint index;
  QString oldName;
  CEnvironment sel;
signals:
  void newName();
public  slots:
  void to1(); 
  void to15();
  void to2();
  void to3();
 public :
  void saturate();
};

class invDlg: public QDialog{
Q_OBJECT
 public:
  QLabel sLabel;
  QLabel found;
  QLabel legend;
  QStringList base;
  invDlg(CEnvironment *ch,const Connection *cl,QStringList _base);
  invariomDlg *id;
  public slots:
  void updateLabel(){
    sLabel.setText("<b>"+id->name+"</b>");
    if (base.size()>0){
      if (base.contains(id->name))
	found.setText("<font color=\"green\">...found in the data base.</font>");
      else
	found.setText("<font color=\"red\">...NOT found in the data base.</font>");
    }else
      found.setText("No invariom data base loaded yet");
  }
  protected slots:
  void accept () ;
};

#endif
