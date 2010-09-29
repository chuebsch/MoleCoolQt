#ifndef EACDLG_H
#define EACDLG_H
#include <QtGui>
#include "molekul.h"
class EacGLW: public QGLWidget {
Q_OBJECT
  public:
  molekul m;
  QList<INP> xd;
  EacGLW();
  QFont myFont;
  QStringList pse;
 protected:
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();
 public slots:
  void changeAtom(int OZ);
  void colorDLG();
  void wallA(int st);
  void ringA(int st);
  void spheA(int st);
  void soldA(int st);
  void nolaA(int st);
  void whitA(int st);
 private:
  GLint vp[4];
  void draw();
};
class EacDlg: public QDialog {
Q_OBJECT
  public:
  EacDlg();
  public slots:
  void  updateLEs(int OZ);
  void checkWall();
  void checkSolid();
  void checkBall();
  protected slots:
  void accept () ;
 private:
  EacGLW *eacGLW; 
  QGridLayout *eacGLO;
  QComboBox *eacCB;
  QLineEdit *eacRAD;
  QLineEdit *eacKRAD;
  QCheckBox *walbx;
  QCheckBox *rinbx;
  QCheckBox *sphbx;
  QCheckBox *sldbx;
  QCheckBox *nlabx;
};
#endif
