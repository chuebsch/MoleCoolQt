// beispiele/opengl/mainWindow.h
#ifndef CUBEGL_H
#define CUBEGL_H 1
#include <QGLWidget>
#if defined (Q_WS_MAC)
#include <glu.h>
#else
#include <GL/glu.h>
#endif

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
  double L;
  bool pause;
  int stereo_mode;
  QAction *Istda;
  QTimer *moving;
  QAction *changeGDAct;
  QCheckBox *invertMouseZoom,*minus;
  QAction *rCenter;
  QAction *foact,*fofcact,*f1f2act;
  int cdens, cwid,awidth;
  QAction *addBond,*killBond,*clearSelection,*centerSelection,*addCoordi,*delCoordi; 
  bool molisoTransparence;
  bool zebra;
  bool noWaitLabel;
  double mixdim,miydim,mizdim;
  int wirbas,cbas, bas, ibas, iSel, foubas[5];
  int rotze;
  int expandatom;
  double milsize;
  bool back_grad;
  bool drawAt,elli,drawBo,drawLa,drawHb,Luftschlange,invEditAble,moveLab,moveLeg,xdSetupMode,reSe,depthCueing,moai,horizont,monochrom,rename;
  QStringList invariomsComplete, invariomsUnique;
  MyAtom rn;
  QFont myFont;
  QFont MLegendFont;
  GLdouble MM[16];
  GLclampf bgCR, bgCG, bgCB, bgCA;
  GLclampf tCR, tCG, tCB, tCA;
  int _win_width,_win_height;
  bool drawAx,drawUz;
  void zoom(double speed);
  void rotY(double speed);
  void rotX(double speed);
  void moveX(double speed);
  void moveY(double speed);
  void resetENV();
  QString inv2moproaxes(int index);
  void inv2XDaxes(int index,int maxat);
  QString symm2Key(QString sym);
  QList<CEnvironment> knoepfe;
  int maxmol;
  QString afilename,CID;
  void checkTC();
  bool MIS,MILe;
  QList<INP> selectedAtoms;
  QList<V3> pole;
  QList<V3> poleOrg;
  QList<QColor> farben;
  int faceCull;
public slots:
  void rotCenter();
  void zalman();
  void parallel();
  void crosseye();
  void nostereo();
  void homeXY();
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
  void togglGrow(bool on);
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
  void editInv(const QUrl & link );
  void loadDataBase();
  void loadDataBase(QString filename);
  void exportMoProFiles();
  void exportXDFiles();
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
  void reloadFile();
  void mconf();
  void neuemitte(V3 mitte);
  void diffscroll(int numsteps,int diff);
  void inimibas();
protected:
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);
  void contextMenuEvent(QContextMenuEvent *event);

private:  
  int imFokus;
  QString selectedFilter;
  double mlsc;
  int labToMove;
  void draw();
  void dieDiPole(V3 org);
  bool atomsClickable,growIt;
  double getHDist(int index);
  GLuint selectBuf[MAXSELECT];
  GLfloat feedBuf[3000];
  GLint vp[4];
  V3 altemitte;
  QPoint lastPos;
  CEnvironment ce;
  CEnvironment exportDummys;
  QStringList exportLabels;
  Connection cl;
  QStringList dataBase;
  QList<DABA> entries;
};
#endif

