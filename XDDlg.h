#ifndef XDDLG_H
#define XDDLG_H 1
#include <QtCore>
#include <QtGui>
#include <QDialog>
#include <QtOpenGL>
#include "molekul.h"

#ifndef MAXSELECT 
#define MAXSELECT 300001
#endif

class xdEditDlg;

class XDDlg: public QGLWidget {
Q_OBJECT
 public:
  XDDlg(CEnvironment ch,const Connection cll,CEnvironment lc,CEnvironment *all,QWidget *parent = 0,xdEditDlg *id=0 );
  QString name;
  Connection cl;
  QStringList dkilllist;
  QStringList duminuse;
  CEnvironment locCo;
  V3 mitsav; 
public slots:
  void setLinMode();
  void setXprMode();
  void killDuMode();
 protected:
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
 private:
  CEnvironment *alle;
  int imFokus;
  int oldindex;
  int dummyMode;
  void draw();   
  int durchlauf;
  V3 dpos;
  QString Dlab;
  CEnvironment ce;
  QPoint lastPos;
  xdEditDlg *papi;
  GLuint selectBuf[MAXSELECT];
  GLint vp[4];
  GLuint index;
  CEnvironment sel;
 signals:
  void message(const QString &);
  void addatom(const MyAtom &);;
  void delatom(const MyAtom &);
};

class xdEditDlg: public QDialog{
Q_OBJECT
	private:
  QCheckBox *X_1,*Y_1,*Z_1,*X2,*Xm,*X3,*X_3,*X4,*X_4,
  *Y2,*Ym,*Y3,*Y_3,*Y4,*Y_4,
  *Z2,*Zm,*Z3,*Z_3,*Z4,*Z_4;   
 QGroupBox * sRGroupBox ;
 MyAtom oa;
	public:
  QLabel sLabel;
  QStringList duminuse;
  QLabel statlab;
  QLineEdit *keyLineEdit;
  QString keyline;
  QLabel legend;
  MyAtom ato;
  int atooo;
  QString reststr;
  CEnvironment *alle;
  QComboBox *ax1lab;
  QComboBox *ax2lab;
  QComboBox *ax1type;
  QComboBox *ax2type;
  QCheckBox *lhand;
  int ic1,ic2,iax1,iax2;
  void symmetryReducer(QString text);
  void molman(QString text);
  CEnvironment *chm;
  xdEditDlg(CEnvironment *ch,const Connection *cl,CEnvironment *all,MyAtom orgAtom);
  QDoubleSpinBox tol;
  QLabel messi;
  QString checkSymmOnAx(V3 ax,double TOL=0.03);
  CEnvironment locCo;
  XDDlg *id;
  CEnvironment calcAxis(QString masstr,CEnvironment *all);
  QStatusBar *statbar;
public slots:
  void setKeyLine(const QString &s);	
  void updateAtoms(const MyAtom& atm);
  void updateAtoms2(const MyAtom& atm);
  void updateStatusBar(const QString& s);
  void updatesLabel();
  void updatesLabel2();
  protected slots:
  void accept () ;
};

#endif
