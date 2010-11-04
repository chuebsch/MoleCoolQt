// beispiele/opengl/mainWindow.h
#ifndef CUBEGL_H
#define CUBEGL_H 1
#include <QGLWidget>
#include "invariomDlg.h"
#include "moliso.h"
//#include "molekul.h"
#include <QList>
#ifndef MAXSELECT 
#define MAXSELECT 300001
#endif

class CubeGL : public QGLWidget {
Q_OBJECT
public:
  CubeGL(QWidget *parent = 0);
  MolIso *moliso;
  int stereo_mode;
  QAction *changeGDAct;
  QCheckBox *invertMouseZoom,*minus;
  QAction *rCenter;
  int cdens, cwid,awidth;
  QAction *addBond,*killBond,*clearSelection,*centerSelection,*addCoordi,*delCoordi; 
  bool molisoTransparence;
  bool zebra;
  double mixdim,miydim,mizdim;
  int cbas, bas, ibas, iSel;
  int rotze;
  int expandatom;
  double milsize;
  bool back_grad;
  bool drawAt,elli,drawBo,drawLa,drawHb,Luftschlange,invEditAble,moveLab,moveLeg,xdSetupMode,reSe,depthCueing,moai,horizont,monochrom;
 // GLuint tex[2];
 // int texti;
  QStringList invariomsComplete, invariomsUnique;
  QFont myFont;
  QFont MLegendFont;
 // QString Tex1FileName;
 // QString Tex2FileName;
  GLdouble MM[16];
//  int mibas;
  GLclampf bgCR, bgCG, bgCB, bgCA;
  GLclampf tCR, tCG, tCB, tCA;
  int _win_width,_win_height;
  bool drawAx,drawUz;
  // void setMole(molekul m);
  void zoom(double speed);
  void rotY(double speed);
  void rotX(double speed);
  void moveX(double speed);
  void moveY(double speed);
  void resetENV();
  QString inv2moproaxes(int index);
  QList<CEnvironment> knoepfe;
  int maxmol;
  QString afilename;
  void checkTC();
  bool MIS,MILe;
  //  INP *xdinpC;
  // molekul mokl;
  QList<INP> selectedAtoms;
  QList<V3> pole;
  QList<QColor> farben;
//  double MIMIN,MIMAX,MIFIXMAX,MIFIXMIN;
  int faceCull;
public slots:
  void rotCenter();
  void zalman();
  void parallel();
  void crosseye();
  void nostereo();
  void saveMISettings();
  void loadMISettings();
  void noneCull(bool b);
  void backCull(bool b);
  void frontCull(bool b);
  void scaleLegend(int size);
  void setContourCnt(int cnt);
  void setContourWidth(int width);
  void togglContours(bool b);
  void togglMolisoTransparence(bool b);  
  void setMLFont(QFont fo);
  void setMLFontSize(int size);
  void setFont();
  /*
  void changeTexture();
  void loadTexture(); 
  // */
  void setupTexture();
//  void togglAt();
//  void togglBo();
//  void togglLa();
  void setAtom(bool on);
  void setNoAtom(bool on);
  void setBond(bool on);
  void setNoBond(bool on);
  void setLabels(bool on);
  void setAxes(bool on);
  void setUnitCell(bool on);
  void setEllipsoid(bool on);
  void setEllipsoidNoUpdate(bool on);
  void setHydrogenBond(bool on);
  void setHelices(bool on);
  void setDepthCueing(bool on);
  void setAtomsClickable(bool on);
  void setLabelsMovable(bool on);
  void setLegendMovable(bool on);
  void setIsosurface(bool on);
  void setLegend(bool on);
  void toggleMolisoLegendDirection();
  void togglMonoChrom(bool);
  void toggInvEdit(bool on);
  void toggXDSetup(bool on);
  void togglReSe(bool on);
  void togglBGG(bool on);
  void togglMOAI(bool on);
  void homeLabels();
  void showMatrix();
  void setMatrix();
  void along100();
  void along010();
  void along001();
  void changeBColor();
  void changeTColor();
  void changeBondColor();
  void fontSizeUp() ;
  void fontSizeDown() ;
  void invariomExport();
  QString translateSymm2MP(QString idbs);
//  void unsetXDSetup();
  void editInv(const QUrl & link );
  void loadDataBase();
  void exportMoProFiles();
  void updateBondActions();
  void disSelection();
  void connectSelection();
  void disConnectSelection();
  void coordinativeBonds();
  void delCoordinative();
signals:
  void message(const QString &);
  void bigmessage(const QString &);
  void reloadXD();
  void mconf();
protected:
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);
  void contextMenuEvent(QContextMenuEvent *event);

  //  void keyPressEvent(QKeyEvent *event);
private:  
  int imFokus;
  double mlsc;
  int labToMove;
  void draw();
  void dieDiPole();
  bool atomsClickable;
  double getHDist(int index);
  GLuint selectBuf[MAXSELECT];
  GLfloat feedBuf[3000];
  GLint vp[4];
 // GLuint texture[1];
  QPoint lastPos;
  CEnvironment ce;
  Connection cl;
  QStringList dataBase;
  QList<DABA> entries;
};
#endif

